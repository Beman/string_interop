//  boost/string_interop/string_interop.hpp  --------------------------------------------------//

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

   * make_string needs to add a (defaulted) allocator arguments for the output string.

   * Add make_ or to_* alias functions. See commented out experiments at end of this
     header and at end of smoke_test2.cpp.

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

#include <boost/string_interop/detail/config.hpp>
#include <boost/assert.hpp>
#include <stdexcept>
#include <boost/string_interop/cxx11_char_types.hpp>
#include <boost/string_interop/detail/is_iterator.hpp>
//#include <boost/cstdint.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/decay.hpp>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <limits.h> // CHAR_BIT

#include <boost/config/abi_prefix.hpp> // must be the last #include

#ifndef BOOST_NO_DEFAULTED_FUNCTIONS
# define BOOST_DEFAULTED = default;
#else
# define BOOST_DEFAULTED {}
#endif

namespace boost
{
namespace string_interop
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
  typedef detail::generic_utf16<char16_t>    utf16;     // UTF-16 encoding for char16_t
  typedef detail::generic_utf32<char32_t>    utf32;     // UTF-32 encoding for char32_t
  class default_codec;

//  select_codec type selector

  template <class charT> struct select_codec;
  template <> struct select_codec<char>    { typedef narrow type; };
  template <> struct select_codec<wchar_t> { typedef wide type; };
  template <> struct select_codec<char16_t>   { typedef utf16 type; };
  template <> struct select_codec<char32_t>   { typedef utf32 type; };

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
  template <class ToCodec, class FromCodec, class InputIterator>
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
//  InputIterator must meet the DefaultCtorEndIterator requirements.
//  iterator_traits<InputIterator>::value_type must be char32_t.

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
   BOOST_STRING_INTEROP_THROW(e);
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

  template <class InputIterator>
  class from_iterator
    : public boost::iterator_facade<from_iterator<InputIterator>,
        charT, std::input_iterator_tag, const charT> 
  {
    BOOST_STATIC_ASSERT_MSG((boost::is_same<typename std::iterator_traits<InputIterator>::value_type,
      charT>::value),
      "InputIterator value_type must be same as codec value_type");
    InputIterator  m_begin;
    InputIterator  m_end;
    bool             m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(InputIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<InputIterator>::value_type();
           ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) {}

    // by_size
    from_iterator(InputIterator begin, std::size_t sz)
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

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
      charT, std::input_iterator_tag, const charT>
  {
    InputIterator m_itr;
  public:
    to_iterator() : m_itr(InputIterator()) {}
    to_iterator(InputIterator itr) : m_itr(itr) {}
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

  template <class InputIterator>
  class from_iterator
   : public boost::iterator_facade<from_iterator<InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(char32_t, read_pending = 0xffffffffu);

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, charT>::value),
       "InputIterator value_type must be charT for this from_iterator");
//     BOOST_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
     BOOST_STATIC_ASSERT(sizeof(char32_t)*CHAR_BIT == 32);

     InputIterator  m_begin;   // current position
     InputIterator  m_end;  
     mutable char32_t    m_value;     // current value or read_pending
     bool             m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(InputIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<InputIterator>::value_type();
           ++m_end) {}
      m_value = read_pending;
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type = 0)
      : m_begin(begin), m_end(end), m_default_end(false) { m_value = read_pending; }

    // by_size
    from_iterator(InputIterator begin, std::size_t sz)
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
        BOOST_STRING_INTEROP_THROW(e);
     }
     static void invalid_sequence()
     {
        std::out_of_range e(
          "Invalid UTF-16 sequence encountered while trying to encode UTF-32 character");
        BOOST_STRING_INTEROP_THROW(e);
     }
     void extract_current() const
     {
        m_value = static_cast<char32_t>(static_cast< ::boost::uint16_t>(*m_begin));
        // if the last value is a high surrogate then adjust m_begin and m_value as needed:
        if(detail::is_high_surrogate(*m_begin))
        {
           // precondition; next value must have be a low-surrogate:
           InputIterator next(m_begin);
           char16_t t = *++next;
           if((t & 0xFC00u) != 0xDC00u)
              invalid_code_point(t);
           m_value = (m_value - detail::high_surrogate_base) << 10;
           m_value |= (static_cast<char32_t>(
             static_cast<char16_t>(t)) & detail::ten_bit_mask);
        }
        // postcondition; result must not be a surrogate:
        if(detail::is_surrogate(m_value))
           invalid_code_point(static_cast<char16_t>(m_value));
     }
  };

  //  generic_utf16::to_iterator  ------------------------------------------------------//

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
      charT, std::input_iterator_tag, const charT>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       charT, std::input_iterator_tag, const charT> base_type;

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
//       "InputIterator value_type must be char32_t for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//     BOOST_ASSERT(sizeof(charT)*CHAR_BIT == 16);

     InputIterator   m_begin;
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
     to_iterator() : m_begin(InputIterator()), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
     }
     to_iterator(InputIterator b) : m_begin(b), m_current(2)
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
  BOOST_STRING_INTEROP_DECL extern const char16_t  to_utf16[];  
  BOOST_STRING_INTEROP_DECL extern const unsigned char to_char[];
  BOOST_STRING_INTEROP_DECL extern const boost::uint8_t slice_index[];
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

  template <class InputIterator>  
  class from_iterator
   : public boost::iterator_facade<from_iterator<InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t>
  {
    typedef boost::iterator_facade<from_iterator<InputIterator>,
      char32_t, std::input_iterator_tag, const char32_t> base_type;

    typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

    BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char>::value),
      "InputIterator value_type must be char for this from_iterator");
    BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
    BOOST_STATIC_ASSERT(sizeof(char32_t)*CHAR_BIT == 32);
    
    InputIterator  m_begin;
    InputIterator  m_end;
    bool             m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(InputIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<InputIterator>::value_type();
           ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) {}

    // by_size
    from_iterator(InputIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin), m_default_end(false) {std::advance(m_end, sz);}

    char32_t dereference() const
    {
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to dereference end iterator");
      unsigned char c = static_cast<unsigned char>(*m_begin);
      return static_cast<char32_t>(string_interop::detail::to_utf16[c]);
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

  template <class InputIterator>  
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
       "InputIterator value_type must be char32_t for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     InputIterator m_begin;

  public:
    // construct:
    to_iterator() : m_begin(InputIterator()) {}
    to_iterator(InputIterator begin) : m_begin(begin) {}

    char dereference() const
    {
      BOOST_ASSERT_MSG(m_begin != InputIterator(),
        "Attempt to dereference end iterator");
      char32_t c = *m_begin;
      if (c & 0xFFFF0000U)
        return '?';
      //cout << "*** c is " << hex << c << '\n';
      //cout << "    to_slice[c >> 7] << 7 is "
      //  << unsigned int(string_interop::detail::slice_index[c >> 7] << 7) << '\n';
      return static_cast<char>(string_interop::detail::to_char
        [
          (string_interop::detail::slice_index[c >> 7] << 7) | (c & 0x7f)
        ]);
    }

    bool equal(const to_iterator& that) const
    {
      return m_begin == that.m_begin;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_begin != InputIterator(),
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

  template <class InputIterator>
  class from_iterator
   : public boost::iterator_facade<from_iterator<InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(char32_t, read_pending = 0xffffffffu);

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char>::value),
//       "InputIterator value_type must be char for this from_iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
     BOOST_STATIC_ASSERT(sizeof(char32_t)*CHAR_BIT == 32);

     InputIterator  m_begin;  // current position
     InputIterator  m_end;
     mutable char32_t    m_value;    // current value or read_pending
     bool             m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // by_null
    from_iterator(InputIterator begin) : m_begin(begin), m_end(begin),
      m_default_end(false) 
    {
      for (;
           *m_end != typename std::iterator_traits<InputIterator>::value_type();
           ++m_end) {}
      m_value = read_pending;
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_default_end(false) { m_value = read_pending; }

    // by_size
    from_iterator(InputIterator begin, std::size_t sz)
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
        BOOST_STRING_INTEROP_THROW(e);
     }
     void extract_current()const
     {
        BOOST_ASSERT_MSG(m_begin != m_end,
          "Internal logic error: extracting from end iterator");
        m_value = static_cast<char32_t>(static_cast< ::boost::uint8_t>(*m_begin));
        // we must not have a continuation character:
        if((m_value & 0xC0u) == 0x80u)
           invalid_sequence();
        // see how many extra byts we have:
        unsigned extra = detail::utf8_trailing_byte_count(*m_begin);
        // extract the extra bits, 6 from each extra byte:
        InputIterator next(m_begin);
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
        if(m_value > static_cast<char32_t>(0x10FFFFu))
           invalid_sequence();
     }
  };

  //  utf8::to_iterator  ---------------------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
//       "InputIterator value_type must be char32_t for this iterator");
     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     InputIterator   m_begin;
     mutable char32_t    m_values[5];
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
     to_iterator() : m_begin(InputIterator()), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
     }
     to_iterator(InputIterator b) : m_begin(b), m_current(4)
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
//  into a single iterator that adapts an InputIterator to FromCodec's value_type to 
//  behave as an iterator to the ToCodec's value_type.

template <class ToCodec, class FromCodec, class InputIterator>
class conversion_iterator
  : public ToCodec::template to_iterator<
      typename FromCodec::template from_iterator<InputIterator> >
{
public:
  typedef typename FromCodec::template from_iterator<InputIterator>  from_iterator_type;
  typedef typename ToCodec::template to_iterator<from_iterator_type>   to_iterator_type;

  conversion_iterator() BOOST_DEFAULTED

  conversion_iterator(InputIterator begin)
    : to_iterator_type(from_iterator_type(begin)) {}

  template <class U>
  conversion_iterator(InputIterator begin, U end,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, U>, void* >::type = 0)
    : to_iterator_type(from_iterator_type(begin, end)) {}

  conversion_iterator(InputIterator begin, std::size_t sz)
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

  return ToString(iter_type(s.begin(), s.end()), iter_type());
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
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type make_string(InputIterator begin)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  return ToString(iter_type(begin), iter_type());
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
          class InputIterator> inline
ToString make_string(InputIterator begin, std::size_t sz)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  return ToString(iter_type(begin, sz), iter_type());
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
          class InputIterator, class InputIterator2>
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type make_string(InputIterator begin, InputIterator2 end)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  return ToString(iter_type(begin, end), iter_type());
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
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type
to_narrow(InputIterator begin) {return make_string<narrow, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
ToString to_narrow(InputIterator begin, std::size_t sz)
  {return make_string<narrow, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator, class InputIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type to_narrow(InputIterator begin, InputIterator2 end)
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
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type
to_wide(InputIterator begin) {return make_string<wide, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
ToString to_wide(InputIterator begin, std::size_t sz)
  {return make_string<wide, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<wchar_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator, class InputIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type to_wide(InputIterator begin, InputIterator2 end)
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
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type
to_utf8(InputIterator begin) {return make_string<utf8, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
ToString to_utf8(InputIterator begin, std::size_t sz)
  {return make_string<utf8, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator, class InputIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type to_utf8(InputIterator begin, InputIterator2 end)
  {return make_string<utf8, FromCodec, ToString>(begin, end);}

//--------------------------------  to_utf16()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16_t>,
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
          class ToString = std::basic_string<char16_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type
to_utf16(InputIterator begin) {return make_string<utf16, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
ToString to_utf16(InputIterator begin, std::size_t sz)
  {return make_string<utf16, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char16_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator, class InputIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type to_utf16(InputIterator begin, InputIterator2 end)
  {return make_string<utf16, FromCodec, ToString>(begin, end);}

//--------------------------------  to_utf32()  -------------------------------------//
//  container
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32_t>,
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
          class ToString = std::basic_string<char32_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
  // enable_if resolves ambiguity with FromString overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToString>::type
to_utf32(InputIterator begin) {return make_string<utf32, FromCodec, ToString>(begin);}

//  iterator, size
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator> inline
ToString to_utf32(InputIterator begin, std::size_t sz)
  {return make_string<utf32, FromCodec, ToString>(begin, sz);}

//  iterator range
template <
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = default_codec,
          class ToString = std::basic_string<char32_t>,
# else
          class FromCodec, class ToString,
# endif
          class InputIterator, class InputIterator2> inline
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToString>::type to_utf32(InputIterator begin, InputIterator2 end)
  {return make_string<utf32, FromCodec, ToString>(begin, end);}

}  // namespace string_interop
}  // namespace boost

//----------------------------------------------------------------------------//

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif  // BOOST_STRING_INTEROP_HPP
