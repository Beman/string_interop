//  boost/interop/string_interop.hpp  --------------------------------------------------//

//  Copyright Beman Dawes 2011, 2012
//  Copyright (c) 2004 John Maddock

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//         Conversions to enable interoperation between character strings               //
//                      of different types and encodings.                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
//  John Maddock's boost/regex/pending/unicode_iterator.hpp introduced me to the idea   //
//  of performing conversion via iterator adapters. The code below for the UTF-8        //
//  to/from UTF-32 and UTF-16 to/from UTF-32 adapers was based on that header.          //
//--------------------------------------------------------------------------------------//

/*
   TODO

   * Add error handling options

   * Add make_* functions. See commented out experiments at end of this header and at end
     of smoke_test2.cpp.

   * Replace hacks with <cuchar> based implementation for narrow, wide.

     Codec    Windows    POSIX
     ------   -------    -----
     narrow   hack       hack    Windows assume codepage 437, POSIX assume UTF-8 
     wide     done       hack    POSIX assume wchar_t is UTF-16 or UTF-32
     utf8     done       done
     utf16    done       done
     utf32    done       done

*/


#if !defined(BOOST_STRING_INTEROP_HPP)
#define BOOST_STRING_INTEROP_HPP

#include <boost/interop/detail/config.hpp>
#include <boost/assert.hpp>
#include <stdexcept>
#include <boost/interop/cxx11_char_types.hpp>
#include <boost/interop/detail/is_iterator.hpp>
//#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/decay.hpp>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <limits.h> // CHAR_BIT

#ifndef BOOST_NO_DEFAULTED_FUNCTIONS
# define BOOST_DEFAULTED = default;
#else
# define BOOST_DEFAULTED {}
#endif

namespace boost
{
namespace interop
{
namespace detail
{
template <class charT>
  class generic_utf32;  // i.e. utf-32 <--> utf-32
template <class charT>
  class generic_utf16;  // i.e. wide and utf16
}

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  codecs
  class utf8;                                        // UTF-8 encoding for char
#ifdef BOOST_WINDOWS_API
  class narrow;                                      // native encoding for char
  typedef detail::generic_utf16<wchar_t>  wide;      // UTF-16 encoding for wchar_t
#else
  // hack: assume POSIX narrow encoding is UTF-8 
  typedef utf8 narrow;
  // hack: assume POSIX wide encoding is UTF-16 or UTF-32
# if WCHAR_MAX == 0xffff
  typedef detail::generic_utf16<wchar_t>  wide;      // UTF-16 encoding for wchar_t
# else
  typedef detail::generic_utf32<wchar_t>  wide;      // UTF-32 encoding for wchar_t
# endif
#endif
  typedef detail::generic_utf16<char16>    utf16;     // UTF-16 encoding for char16_t
  typedef detail::generic_utf32<char32>    utf32;     // UTF-32 encoding for char32_t
  class default_codec;

//  select_codec type selector

  template <class charT> struct select_codec;
  template <> struct select_codec<char>    { typedef narrow type; };
  template <> struct select_codec<wchar_t> { typedef wide type; };
  template <> struct select_codec<char16>   { typedef utf16 type; };
  template <> struct select_codec<char32>   { typedef utf32 type; };

//  default_codec pseudo codec
//
//  provides lazy select_codec selection so that codec template parameters with defaults
//  can appear before the template parameter that determines charT.  

  class default_codec
  {
  public:
    template <class charT>
    struct codec
    { 
      typedef typename select_codec<charT>::type type; 
    };

  };

  //  conversion_iterator
  template <class ToCodec, class FromCodec, class ForwardIterator>
    class conversion_iterator;

//  see make_string() functions below

//---------------------------------  Requirements  -------------------------------------//
//
//  DefaultCtorEndIterator:
//
//  For an iterator of type T, T() constructs the end iterator.
//
//  Codec:
//
//  from_iterator meets the DefaultCtorEndIterator requirements.
//  iterator_traits<from_iterator>::value_type is char32_t.
//
//  to_iterator meets the DefaultCtorEndIterator requirements.
//  ForwardIterator must meet the DefaultCtorEndIterator requirements.
//  iterator_traits<ForwardIterator>::value_type must be char32_t.

//--------------------------------------------------------------------------------------//
//                                  Implementation                                      //
//--------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
//                                      codecs                                          //
//--------------------------------------------------------------------------------------//

//------------------------------------  helpers  ---------------------------------------//

namespace detail{

static const ::boost::uint16_t high_surrogate_base = 0xD7C0u;
static const ::boost::uint16_t low_surrogate_base = 0xDC00u;
static const ::boost::uint32_t ten_bit_mask = 0x3FFu;

inline bool is_high_surrogate(::boost::uint16_t v)
{
   return (v & 0xFFFFFC00u) == 0xd800u;
}
inline bool is_low_surrogate(::boost::uint16_t v)
{
   return (v & 0xFFFFFC00u) == 0xdc00u;
}
template <class T>
inline bool is_surrogate(T v)
{
   return (v & 0xFFFFF800u) == 0xd800;
}

inline unsigned utf8_byte_count(boost::uint8_t c)
{
   // if the most significant bit with a zero in it is in position
   // 8-N then there are N bytes in this UTF-8 sequence:
   boost::uint8_t mask = 0x80u;
   unsigned result = 0;
   while(c & mask)
   {
      ++result;
      mask >>= 1;
   }
   return (result == 0) ? 1 : ((result > 4) ? 4 : result);
}

inline unsigned utf8_trailing_byte_count(boost::uint8_t c)
{
   return utf8_byte_count(c) - 1;
}

#ifdef BOOST_MSVC
# pragma warning(push)
# pragma warning(disable:4100)
#endif
inline void invalid_utf32_code_point(::boost::uint32_t val)
{
   std::stringstream ss;
   ss << "Invalid UTF-32 code point U+" << std::showbase << std::hex << val
      << " encountered while trying to encode UTF-16 sequence";
   std::out_of_range e(ss.str());
   BOOST_INTEROP_THROW(e);
}
#ifdef BOOST_MSVC
# pragma warning(pop)
#endif

//--------------------------------------------------------------------------------------//
//                                   generic_utf32                                      //
//--------------------------------------------------------------------------------------//

template <class charT>
class generic_utf32
{
public:
  typedef charT value_type;
  template <class charT2> struct codec { typedef generic_utf32<charT> type; };

  //  generic_utf32::from_iterator  ---------------------------------------------------//

  template <class ForwardIterator>
  class from_iterator
    : public boost::iterator_facade<from_iterator<ForwardIterator>,
        charT, std::input_iterator_tag, const charT> 
  {
    BOOST_STATIC_ASSERT_MSG((boost::is_same<typename std::iterator_traits<ForwardIterator>::value_type,
      charT>::value),
      "ForwardIterator value_type must be same as codec value_type");
    ForwardIterator  m_begin;
    ForwardIterator  m_end;
    bool             m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(ForwardIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<ForwardIterator>::value_type();
           ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(ForwardIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<ForwardIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) {}

    // by_size
    from_iterator(ForwardIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin), m_default_end(false)
    {
      std::advance(m_end, sz);
    }

    charT dereference() const
    {
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to dereference end iterator");
      return *m_begin;
    }

    bool equal(const from_iterator& that) const
    {
      if (m_default_end || m_begin == m_end)
        return that.m_default_end || that.m_begin == that.m_end;
      if (that.m_default_end || that.m_begin == that.m_end)
        return false;
      return m_begin == that.m_begin;
    }

    void increment()
    {
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to increment end iterator");
      ++m_begin;
    }
  };

  //  generic_utf32::::to_iterator  ---------------------------------------------------//

  template <class ForwardIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<ForwardIterator>,
      charT, std::input_iterator_tag, const charT>
  {
    ForwardIterator m_itr;
  public:
    to_iterator() : m_itr(ForwardIterator()) {}
    to_iterator(ForwardIterator itr) : m_itr(itr) {}
    charT dereference() const { return *m_itr; }
    bool equal(const to_iterator& that) const {return m_itr == that.m_itr;}
    void increment() { ++m_itr; }
  };

};

//--------------------------------------------------------------------------------------//
//                                   generic_utf16                                      //
//--------------------------------------------------------------------------------------//

template <class charT>
class generic_utf16
{
public:
  typedef charT value_type;
  template <class charT2> struct codec { typedef generic_utf16<charT> type; };

  //  generic_utf16::from_iterator  ----------------------------------------------------//

  template <class ForwardIterator>
  class from_iterator
   : public boost::iterator_facade<from_iterator<ForwardIterator>,
       char32, std::input_iterator_tag, const char32>
  {
     typedef boost::iterator_facade<from_iterator<ForwardIterator>,
       char32, std::input_iterator_tag, const char32> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(char32, read_pending = 0xffffffffu);

     typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, charT>::value),
       "ForwardIterator value_type must be charT for this from_iterator");
//     BOOST_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
     BOOST_STATIC_ASSERT(sizeof(char32)*CHAR_BIT == 32);

     ForwardIterator  m_begin;   // current position
     ForwardIterator  m_end;  
     mutable char32    m_value;     // current value or read_pending
     bool             m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(ForwardIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<ForwardIterator>::value_type();
           ++m_end) {}
      m_value = read_pending;
    }

    // by range
    template <class T>
    from_iterator(ForwardIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<ForwardIterator, T>, void* >::type = 0)
      : m_begin(begin), m_end(end), m_default_end(false) { m_value = read_pending; }

    // by_size
    from_iterator(ForwardIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin), m_default_end(false)
    {
      std::advance(m_end, sz);
      m_value = read_pending;
    }

     typename base_type::reference
        dereference() const
     {
        BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
          "Attempt to dereference end iterator");
        if (m_value == read_pending)
           extract_current();
        return m_value;
     }

     bool equal(const from_iterator& that) const 
     {
       if (m_default_end || m_begin == m_end)
         return that.m_default_end || that.m_begin == that.m_end;
       if (that.m_default_end || that.m_begin == that.m_end)
         return false;
       return m_begin == that.m_begin;
     }

     void increment()
     {
       BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
         "Attempt to increment end iterator");
       // skip high surrogate first if there is one:
       if(detail::is_high_surrogate(*m_begin))
         ++m_begin;
       ++m_begin;
       m_value = read_pending;
     }

  private:
     static void invalid_code_point(::boost::uint16_t val)
     {
        std::stringstream ss;
        ss << "Misplaced UTF-16 surrogate U+" << std::showbase << std::hex << val
           << " encountered while trying to encode UTF-32 sequence";
        std::out_of_range e(ss.str());
        BOOST_INTEROP_THROW(e);
     }
     static void invalid_sequence()
     {
        std::out_of_range e(
          "Invalid UTF-16 sequence encountered while trying to encode UTF-32 character");
        BOOST_INTEROP_THROW(e);
     }
     void extract_current() const
     {
        m_value = static_cast<char32>(static_cast< ::boost::uint16_t>(*m_begin));
        // if the last value is a high surrogate then adjust m_begin and m_value as needed:
        if(detail::is_high_surrogate(*m_begin))
        {
           // precondition; next value must have be a low-surrogate:
           ForwardIterator next(m_begin);
           char16 t = *++next;
           if((t & 0xFC00u) != 0xDC00u)
              invalid_code_point(t);
           m_value = (m_value - detail::high_surrogate_base) << 10;
           m_value |= (static_cast<char32>(
             static_cast<char16>(t)) & detail::ten_bit_mask);
        }
        // postcondition; result must not be a surrogate:
        if(detail::is_surrogate(m_value))
           invalid_code_point(static_cast<char16>(m_value));
     }
  };

  //  generic_utf16::to_iterator  ------------------------------------------------------//

  template <class ForwardIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<ForwardIterator>,
      charT, std::input_iterator_tag, const charT>
  {
     typedef boost::iterator_facade<to_iterator<ForwardIterator>,
       charT, std::input_iterator_tag, const charT> base_type;

     typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32>::value),
//       "ForwardIterator value_type must be char32 for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//     BOOST_ASSERT(sizeof(charT)*CHAR_BIT == 16);

     ForwardIterator   m_begin;
     mutable charT   m_values[3];
     mutable unsigned  m_current;

  public:

     typename base_type::reference
     dereference()const
     {
        if(m_current == 2)
           extract_current();
        return m_values[m_current];
     }
     bool equal(const to_iterator& that)const
     {
        if(m_begin == that.m_begin)
        {
           // Both m_currents must be equal, or both even
           // this is the same as saying their sum must be even:
           return (m_current + that.m_current) & 1u ? false : true;
        }
        return false;
     }
     void increment()
     {
        // if we have a pending read then read now, so that we know whether
        // to skip a position, or move to a low-surrogate:
        if(m_current == 2)
        {
           // pending read:
           extract_current();
        }
        // move to the next surrogate position:
        ++m_current;
        // if we've reached the end skip a position:
        if(m_values[m_current] == 0)
        {
           m_current = 2;
           ++m_begin;
        }
     }

     // construct:
     to_iterator() : m_begin(ForwardIterator()), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
     }
     to_iterator(ForwardIterator b) : m_begin(b), m_current(2)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
    }
  private:

     void extract_current()const
     {
        // begin by checking for a code point out of range:
        ::boost::uint32_t v = *m_begin;
        if(v >= 0x10000u)
        {
           if(v > 0x10FFFFu)
              detail::invalid_utf32_code_point(*m_begin);
           // split into two surrogates:
           m_values[0] = static_cast<charT>(v >> 10) + detail::high_surrogate_base;
           m_values[1] = static_cast<charT>(v & detail::ten_bit_mask)
             + detail::low_surrogate_base;
           m_current = 0;
           BOOST_ASSERT(detail::is_high_surrogate(m_values[0]));
           BOOST_ASSERT(detail::is_low_surrogate(m_values[1]));
        }
        else
        {
           // 16-bit code point:
           m_values[0] = static_cast<charT>(*m_begin);
           m_values[1] = 0;
           m_current = 0;
           // value must not be a surrogate:
           if(detail::is_surrogate(m_values[0]))
              detail::invalid_utf32_code_point(*m_begin);
        }
     }
  };

};

} // namespace detail

//--------------------------------------------------------------------------------------//
//                                    narrow codec                                      //
//--------------------------------------------------------------------------------------//

#ifdef BOOST_WINDOWS_API
namespace detail
{
  //  for this proof-of-concept implementation, use codepage 437 tables at end of header
  extern const boost::char16  to_utf16[];  
  extern const unsigned char to_char[];
  extern const boost::uint8_t slice_index[];
}

class narrow
{
public:
  typedef char value_type;

  template <class charT>
  struct codec { typedef narrow type; };

  //  narrow::from_iterator  -----------------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  template <class ForwardIterator>  
  class from_iterator
   : public boost::iterator_facade<from_iterator<ForwardIterator>,
       char32, std::input_iterator_tag, const char32>
  {
    typedef boost::iterator_facade<from_iterator<ForwardIterator>,
      char32, std::input_iterator_tag, const char32> base_type;

    typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

    BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char>::value),
      "ForwardIterator value_type must be char for this from_iterator");
    BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
    BOOST_STATIC_ASSERT(sizeof(char32)*CHAR_BIT == 32);
    
    ForwardIterator  m_begin;
    ForwardIterator  m_end;
    bool             m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(ForwardIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<ForwardIterator>::value_type();
           ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(ForwardIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<ForwardIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) {}

    // by_size
    from_iterator(ForwardIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin), m_default_end(false) {std::advance(m_end, sz);}

    char32 dereference() const
    {
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to dereference end iterator");
      unsigned char c = static_cast<unsigned char>(*m_begin);
      return static_cast<char32>(interop::detail::to_utf16[c]);
    }

    bool equal(const from_iterator& that) const
    {
      if (m_default_end || m_begin == m_end)
        return that.m_default_end || that.m_begin == that.m_end;
      if (that.m_default_end || that.m_begin == that.m_end)
        return false;
      return m_begin == that.m_begin;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to increment end iterator");
      ++m_begin;
    }
  };

  //  narrow::to_iterator  -------------------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  template <class ForwardIterator>  
  class to_iterator
   : public boost::iterator_facade<to_iterator<ForwardIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<ForwardIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char32>::value),
       "ForwardIterator value_type must be char32_t for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     ForwardIterator m_begin;

  public:
    // construct:
    to_iterator() : m_begin(ForwardIterator()) {}
    to_iterator(ForwardIterator begin) : m_begin(begin) {}

    char dereference() const
    {
      BOOST_ASSERT_MSG(m_begin != ForwardIterator(),
        "Attempt to dereference end iterator");
      char32 c = *m_begin;
      if (c & 0xFFFF0000U)
        return '?';
      //cout << "*** c is " << hex << c << '\n';
      //cout << "    to_slice[c >> 7] << 7 is "
      //  << unsigned int(interop::detail::slice_index[c >> 7] << 7) << '\n';
      return static_cast<char>(interop::detail::to_char
        [
          (interop::detail::slice_index[c >> 7] << 7) | (c & 0x7f)
        ]);
    }

    bool equal(const to_iterator& that) const
    {
      return m_begin == that.m_begin;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_begin != ForwardIterator(),
        "Attempt to increment end iterator");
      ++m_begin;  // may change m_begin to end iterator
    }

  };  // to_iterator
};  // narrow

#endif

//--------------------------------------------------------------------------------------//
//                                     utf8 codec                                       //
//--------------------------------------------------------------------------------------//

class utf8
{
public:
  typedef char value_type;
  template <class charT> struct codec { typedef utf8 type; };

  //  utf8::from_iterator  -------------------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  template <class ForwardIterator>
  class from_iterator
   : public boost::iterator_facade<from_iterator<ForwardIterator>,
       char32, std::input_iterator_tag, const char32>
  {
     typedef boost::iterator_facade<from_iterator<ForwardIterator>,
       char32, std::input_iterator_tag, const char32> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(char32, read_pending = 0xffffffffu);

     typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char>::value),
//       "ForwardIterator value_type must be char for this from_iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
     BOOST_STATIC_ASSERT(sizeof(char32)*CHAR_BIT == 32);

     ForwardIterator  m_begin;  // current position
     ForwardIterator  m_end;
     mutable char32    m_value;    // current value or read_pending
     bool             m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(ForwardIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<ForwardIterator>::value_type();
           ++m_end) {}
      m_value = read_pending;
    }

    // by range
    template <class T>
    from_iterator(ForwardIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<ForwardIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) { m_value = read_pending; }

    // by_size
    from_iterator(ForwardIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin), m_default_end(false)
    {
      std::advance(m_end, sz);
      m_value = read_pending;
    }

     typename base_type::reference
        dereference() const
     {
        BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
          "Attempt to dereference end iterator");
        if (m_value == read_pending)
           extract_current();
        return m_value;
     }

     bool equal(const from_iterator& that) const
     {
       if (m_default_end || m_begin == m_end)
         return that.m_default_end || that.m_begin == that.m_end;
       if (that.m_default_end || that.m_begin == that.m_end)
         return false;
       return m_begin == that.m_begin;
     }

     void increment()
     {
        BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
          "Attempt to increment end iterator");
        unsigned count = detail::utf8_byte_count(*m_begin);
        std::advance(m_begin, count);
        m_value = read_pending;
     }
  private:
     static void invalid_sequence()
     {
        std::out_of_range e(
          "Invalid UTF-8 sequence encountered while trying to encode UTF-32 character");
        BOOST_INTEROP_THROW(e);
     }
     void extract_current()const
     {
        BOOST_ASSERT_MSG(m_begin != m_end,
          "Internal logic error: extracting from end iterator");
        m_value = static_cast<char32>(static_cast< ::boost::uint8_t>(*m_begin));
        // we must not have a continuation character:
        if((m_value & 0xC0u) == 0x80u)
           invalid_sequence();
        // see how many extra byts we have:
        unsigned extra = detail::utf8_trailing_byte_count(*m_begin);
        // extract the extra bits, 6 from each extra byte:
        ForwardIterator next(m_begin);
        for(unsigned c = 0; c < extra; ++c)
        {
           ++next;
           m_value <<= 6;
           m_value += static_cast<boost::uint8_t>(*next) & 0x3Fu;
        }
        // we now need to remove a few of the leftmost bits, but how many depends
        // upon how many extra bytes we've extracted:
        static const boost::uint32_t masks[4] = 
        {
           0x7Fu,
           0x7FFu,
           0xFFFFu,
           0x1FFFFFu,
        };
        m_value &= masks[extra];
        // check the result:
        if(m_value > static_cast<char32>(0x10FFFFu))
           invalid_sequence();
     }
  };

  //  utf8::to_iterator  ---------------------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  template <class ForwardIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<ForwardIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<ForwardIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32>::value),
//       "ForwardIterator value_type must be char32_t for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     ForwardIterator   m_begin;
     mutable char32    m_values[5];
     mutable unsigned  m_current;

  public:

     typename base_type::reference
     dereference()const
     {
        if(m_current == 4)
           extract_current();
        return m_values[m_current];
     }
     bool equal(const to_iterator& that)const
     {
        if(m_begin == that.m_begin)
        {
           // either the m_begin's must be equal, or one must be 0 and 
           // the other 4: which means neither must have bits 1 or 2 set:
           return (m_current == that.m_current)
              || (((m_current | that.m_current) & 3) == 0);
        }
        return false;
     }
     void increment()
     {
        // if we have a pending read then read now, so that we know whether
        // to skip a position, or move to a low-surrogate:
        if(m_current == 4)
        {
           // pending read:
           extract_current();
        }
        // move to the next surrogate position:
        ++m_current;
        // if we've reached the end skip a position:
        if(m_values[m_current] == 0)
        {
           m_current = 4;
           ++m_begin;
        }
     }

     // construct:
     to_iterator() : m_begin(ForwardIterator()), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
     }
     to_iterator(ForwardIterator b) : m_begin(b), m_current(4)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
    }
  private:

     void extract_current()const
     {
        boost::uint32_t c = *m_begin;
        if(c > 0x10FFFFu)
           detail::invalid_utf32_code_point(c);
        if(c < 0x80u)
        {
           m_values[0] = static_cast<unsigned char>(c);
           m_values[1] = static_cast<unsigned char>(0u);
           m_values[2] = static_cast<unsigned char>(0u);
           m_values[3] = static_cast<unsigned char>(0u);
        }
        else if(c < 0x800u)
        {
           m_values[0] = static_cast<unsigned char>(0xC0u + (c >> 6));
           m_values[1] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
           m_values[2] = static_cast<unsigned char>(0u);
           m_values[3] = static_cast<unsigned char>(0u);
        }
        else if(c < 0x10000u)
        {
           m_values[0] = static_cast<unsigned char>(0xE0u + (c >> 12));
           m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
           m_values[2] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
           m_values[3] = static_cast<unsigned char>(0u);
        }
        else
        {
           m_values[0] = static_cast<unsigned char>(0xF0u + (c >> 18));
           m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 12) & 0x3Fu));
           m_values[2] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
           m_values[3] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
        }
        m_current= 0;
     }
  };

};

//--------------------------------------------------------------------------------------//
//                                 conversion_iterator                                  //
//--------------------------------------------------------------------------------------//

//  A conversion_iterator composes a ToCodec's to_iterator and a FromCodec's from_iterator
//  into a single iterator that adapts an ForwardIterator to FromCodec's value_type to 
//  behave as an iterator to the ToCodec's value_type.

template <class ToCodec, class FromCodec, class ForwardIterator>
class conversion_iterator
  : public ToCodec::template to_iterator<
      typename FromCodec::template from_iterator<ForwardIterator> >
{
public:
  typedef typename FromCodec::template from_iterator<ForwardIterator>  from_iterator_type;
  typedef typename ToCodec::template to_iterator<from_iterator_type>   to_iterator_type;

  conversion_iterator() BOOST_DEFAULTED

  conversion_iterator(ForwardIterator begin)
    : to_iterator_type(from_iterator_type(begin)) {}

  template <class U>
  conversion_iterator(ForwardIterator begin, U end,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<ForwardIterator, U>, void* >::type = 0)
    : to_iterator_type(from_iterator_type(begin, end)) {}

  conversion_iterator(ForwardIterator begin, std::size_t sz)
    : to_iterator_type(from_iterator_type(begin, sz)) {}
};

//--------------------------------------------------------------------------------------//
//                               make_string function                                   //
//--------------------------------------------------------------------------------------//

//  container
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToString,
# endif
          class FromString> inline
  // enable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type make_string(const FromString& s)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template codec<typename FromString::value_type>::type,
    typename FromString::const_iterator>
      iter_type;

  ToString tmp;

  iter_type iter(s.begin(), s.end());
  std::copy(iter, iter_type(), std::back_insert_iterator<ToString>(tmp));
  return tmp;
}

//  null terminated iterator
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type make_string(ForwardIterator begin)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<ForwardIterator>::value_type>::type,
    ForwardIterator>
      iter_type;

  ToString tmp;
  iter_type itr(begin);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//  iterator, size
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToString,
# endif
          class ForwardIterator> inline
ToString make_string(ForwardIterator begin, std::size_t sz)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<ForwardIterator>::value_type>::type,
    ForwardIterator>
      iter_type;

  ToString tmp;
  iter_type itr(begin, sz);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//  iterator range
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToString,
# endif
          class ForwardIterator, class ForwardIterator2>
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type make_string(ForwardIterator begin, ForwardIterator2 end)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<ForwardIterator>::value_type>::type,
    ForwardIterator>
      iter_type;

  ToString tmp;
  iter_type itr(begin, end);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//--------------------------------------------------------------------------------------//
//                               make_string aliases                                    //
//--------------------------------------------------------------------------------------//

//--------------------------------  to_narrow()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class FromString> inline
  // disable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type
to_narrow(const FromString& s) {return make_string<narrow, FromCodec, ToString>(s);}

//  null terminated iterator
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type
to_narrow(ForwardIterator begin) {return make_string<narrow, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
ToString to_narrow(ForwardIterator begin, std::size_t sz)
  {return make_string<narrow, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator, class ForwardIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type to_narrow(ForwardIterator begin, ForwardIterator2 end)
  {return make_string<narrow, FromCodec, ToString>(begin, end);}

//--------------------------------  to_wide()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class FromString> inline
  // disable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type
to_wide(const FromString& s) {return make_string<wide, FromCodec, ToString>(s);}

//  null terminated iterator
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type
to_wide(ForwardIterator begin) {return make_string<wide, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
ToString to_wide(ForwardIterator begin, std::size_t sz)
  {return make_string<wide, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator, class ForwardIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type to_wide(ForwardIterator begin, ForwardIterator2 end)
  {return make_string<wide, FromCodec, ToString>(begin, end);}

//--------------------------------  to_utf8()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class FromString> inline
  // disable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type
to_utf8(const FromString& s) {return make_string<utf8, FromCodec, ToString>(s);}

//  null terminated iterator
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type
to_utf8(ForwardIterator begin) {return make_string<utf8, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
ToString to_utf8(ForwardIterator begin, std::size_t sz)
  {return make_string<utf8, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator, class ForwardIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type to_utf8(ForwardIterator begin, ForwardIterator2 end)
  {return make_string<utf8, FromCodec, ToString>(begin, end);}

//--------------------------------  to_utf16()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16>,
# else
          class FromCodec, class ToString,
# endif
          class FromString> inline
  // disable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type
to_utf16(const FromString& s) {return make_string<utf16, FromCodec, ToString>(s);}

//  null terminated iterator
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type
to_utf16(ForwardIterator begin) {return make_string<utf16, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
ToString to_utf16(ForwardIterator begin, std::size_t sz)
  {return make_string<utf16, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator, class ForwardIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type to_utf16(ForwardIterator begin, ForwardIterator2 end)
  {return make_string<utf16, FromCodec, ToString>(begin, end);}

//--------------------------------  to_utf32()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32>,
# else
          class FromCodec, class ToString,
# endif
          class FromString> inline
  // disable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<typename boost::decay<FromString>::type>,
ToString>::type
to_utf32(const FromString& s) {return make_string<utf32, FromCodec, ToString>(s);}

//  null terminated iterator
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<ForwardIterator>,
ToString>::type
to_utf32(ForwardIterator begin) {return make_string<utf32, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator> inline
ToString to_utf32(ForwardIterator begin, std::size_t sz)
  {return make_string<utf32, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32>,
# else
          class FromCodec, class ToString,
# endif
          class ForwardIterator, class ForwardIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<ForwardIterator2>,
ToString>::type to_utf32(ForwardIterator begin, ForwardIterator2 end)
  {return make_string<utf32, FromCodec, ToString>(begin, end);}

#ifdef BOOST_WINDOWS_API
namespace detail
{

/****************** tables generated by tools/table_generator.cpp ***********************/

/****************************** DO NOT EDIT BY HAND *************************************/

const boost::char16  to_utf16[] =
{
0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0xc7,0xfc,0xe9,0xe2,0xe4,0xe0,0xe5,0xe7,0xea,0xeb,0xe8,0xef,0xee,0xec,0xc4,0xc5,
0xc9,0xe6,0xc6,0xf4,0xf6,0xf2,0xfb,0xf9,0xff,0xd6,0xdc,0xa2,0xa3,0xa5,0x20a7,0x192,
0xe1,0xed,0xf3,0xfa,0xf1,0xd1,0xaa,0xba,0xbf,0x2310,0xac,0xbd,0xbc,0xa1,0xab,0xbb,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255d,0x255c,0x255b,0x2510,
0x2514,0x2534,0x252c,0x251c,0x2500,0x253c,0x255e,0x255f,0x255a,0x2554,0x2569,0x2566,0x2560,0x2550,0x256c,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256b,0x256a,0x2518,0x250c,0x2588,0x2584,0x258c,0x2590,0x2580,
0x3b1,0xdf,0x393,0x3c0,0x3a3,0x3c3,0xb5,0x3c4,0x3a6,0x398,0x3a9,0x3b4,0x221e,0x3c6,0x3b5,0x2229,
0x2261,0xb1,0x2265,0x2264,0x2320,0x2321,0xf7,0x2248,0xb0,0x2219,0xb7,0x221a,0x207f,0xb2,0x25a0,0xa0
};

BOOST_STATIC_ASSERT(sizeof(to_utf16) == 2*256);

const unsigned char to_char[] =
{
//  slice 0 - characters with no codepage representation
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 1 - 0x0
0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,

//  slice 2 - 0x80
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xff,0xad,0x9b,0x9c,'?',0x9d,'?','?','?','?',0xa6,0xae,0xaa,'?','?','?',
0xf8,0xf1,0xfd,'?','?',0xe6,'?',0xfa,'?','?',0xa7,0xaf,0xac,0xab,'?',0xa8,
'?','?','?','?',0x8e,0x8f,0x92,0x80,'?',0x90,'?','?','?','?','?','?',
'?',0xa5,'?','?','?','?',0x99,'?','?','?','?','?',0x9a,'?','?',0xe1,
0x85,0xa0,0x83,'?',0x84,0x86,0x91,0x87,0x8a,0x82,0x88,0x89,0x8d,0xa1,0x8c,0x8b,
'?',0xa4,0x95,0xa2,0x93,'?',0x94,0xf6,'?',0x97,0xa3,0x96,0x81,'?','?',0x98,

//  slice 3 - 0x180
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?',0x9f,'?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 4 - 0x380
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?',0xe2,'?','?','?','?',0xe9,'?','?','?','?','?','?','?',
'?','?','?',0xe4,'?','?',0xe8,'?','?',0xea,'?','?','?','?','?','?',
'?',0xe0,'?','?',0xeb,0xee,'?','?','?','?','?','?','?','?','?','?',
0xe3,'?','?',0xe5,0xe7,'?',0xed,'?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 5 - 0x2000
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',0xfc,

//  slice 6 - 0x2080
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?',0x9e,'?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 7 - 0x2200
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?',0xf9,0xfb,'?','?','?',0xec,'?',
'?','?','?','?','?','?','?','?','?',0xef,'?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?',0xf7,'?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?',0xf0,'?','?',0xf3,0xf2,'?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 8 - 0x2300
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xa9,'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xf4,0xf5,'?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 9 - 0x2500
0xc4,'?',0xb3,'?','?','?','?','?','?','?','?','?',0xda,'?','?','?',
0xbf,'?','?','?',0xc0,'?','?','?',0xd9,'?','?','?',0xc3,'?','?','?',
'?','?','?','?',0xb4,'?','?','?','?','?','?','?',0xc2,'?','?','?',
'?','?','?','?',0xc1,'?','?','?','?','?','?','?',0xc5,'?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xcd,0xba,0xd5,0xd6,0xc9,0xb8,0xb7,0xbb,0xd4,0xd3,0xc8,0xbe,0xbd,0xbc,0xc6,0xc7,
0xcc,0xb5,0xb6,0xb9,0xd1,0xd2,0xcb,0xcf,0xd0,0xca,0xd8,0xd7,0xce,'?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 10 - 0x2580
0xdf,'?','?','?',0xdc,'?','?','?',0xdb,'?','?','?',0xdd,'?','?','?',
0xde,0xb0,0xb1,0xb2,'?','?','?','?','?','?','?','?','?','?','?','?',
0xfe,'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

};

BOOST_STATIC_ASSERT(sizeof(to_char) == 1408);

const boost::uint8_t slice_index[] =
{
1,2,0,3,0,0,0,4,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
5,6,0,0,7,0,8,0,0,0,9,10,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

};

BOOST_STATIC_ASSERT(sizeof(slice_index) == 512);

}  // namesapce detail
#endif

}  // namespace interop
}  // namespace boost

#endif  // BOOST_STRING_INTEROP_HPP
