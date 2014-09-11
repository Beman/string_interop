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

//  Validate implementation expectations

BOOST_STATIC_ASSERT_MSG(sizeof(char)*CHAR_BIT == 8, "Implementation expects 8-bit char");
BOOST_STATIC_ASSERT_MSG(sizeof(char16_t)*CHAR_BIT == 16, "Implementation expects 16-bit char16_t");
BOOST_STATIC_ASSERT_MSG(sizeof(char32_t)*CHAR_BIT == 32, "Implementation expects 32-bit char32_t");
BOOST_STATIC_ASSERT_MSG(sizeof(wchar_t)*CHAR_BIT == 8
  || sizeof(wchar_t)*CHAR_BIT == 16 || sizeof(wchar_t)*CHAR_BIT == 32,
       "Implementation expects 8, 16, or 32-bit wchar_t");

// forward declarations
struct utf8;
struct utf16;
struct utf32;

namespace detail
{
  //  Helper to find actual encoding of wchar_t
  template <std::size_t> struct wide_encoding;
  template<> struct wide_encoding<1> { typedef utf8  actual; };
  template<> struct wide_encoding<2> { typedef utf16 actual; };
  template<> struct wide_encoding<4> { typedef utf32 actual; };
}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                     Synopsis                                         //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//  Encodings supply encoded character type (i.e. value_type) and actual encoding.
//  The actual encoding for wide varies depending on sizeof(value_type).

struct narrow
{ 
  typedef char      value_type;
  typedef narrow    actual_encoding;
  template <class EcharT>
    struct encoding { typedef narrow type; };
};
struct utf8
{
  typedef char      value_type;
  typedef utf8      actual_encoding;
  template <class EcharT>
    struct encoding { typedef utf8 type; };
};
struct utf16
{
  typedef char16_t  value_type;
  typedef utf16     actual_encoding;
  template <class EcharT>
    struct encoding { typedef utf16 type; };
};
struct utf32
{
  typedef char32_t  value_type;
  typedef utf32     actual_encoding;
  template <class EcharT>
    struct encoding { typedef utf32 type; };
};
struct wide
{ 
  typedef wchar_t  value_type;
  typedef detail::wide_encoding<sizeof(wchar_t)>::actual actual_encoding;
  template <class EcharT>
    struct encoding { typedef narrow type; };
};

//  Encoded character types (i.e. value types) and their default encodings

template <class EcharT>
struct default_encoding;

template<> struct default_encoding<char>     { typedef narrow  type; };
template<> struct default_encoding<wchar_t>  { typedef wide    type; };
template<> struct default_encoding<char16_t> { typedef utf16   type; };
template<> struct default_encoding<char32_t> { typedef utf32   type; };


//  Psuedo-encoding used as a default template parameter that parameter precedes the
//  parameter that determines the value type.

struct defaulted
{
public:
  template <class EcharT>
  struct encoding
  {
    typedef typename default_encoding<EcharT>::type type;
  };
};

//--------------------------------------------------------------------------------------//
//              from_iterator - converts the FromEncoding to UTF-32                     //
//--------------------------------------------------------------------------------------//

template <class FromEncoding, class FromCharT, class InputIterator>  // primary template;
class from_iterator;                                      //  specializations do the work

//--------------------------------------------------------------------------------------//
//              to_iterator - converts UTF-32 to the ToEncoding                         //
//--------------------------------------------------------------------------------------//

template <class ToEncoding, class ToCharT, class InputIterator>       // primary template;
class to_iterator;                                         //  specializations do the work

//--------------------------------------------------------------------------------------//
//                               conversion_iterator                                    //
//--------------------------------------------------------------------------------------//

template <class ToEncoding, class FromEncoding, class InputIterator>
class conversion_iterator;

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                 End of Synopsis                                      //
//                                                                                      //
//--------------------------------------------------------------------------------------//





//---------------------------------  Requirements  -------------------------------------//
//                                                                                      //
//  DefaultCtorEndIterator:                                                             //
//                                                                                      //
//  For an iterator of type T, T() constructs the end iterator.                         //
//                                                                                      //
//  from_iterator meets the DefaultCtorEndIterator requirements.                        //
//  iterator_traits<from_iterator>::value_type is char32_t.                             //
//                                                                                      //
//  to_iterator meets the DefaultCtorEndIterator requirements.                          //
//  InputIterator must meet the DefaultCtorEndIterator requirements.                    //
//  iterator_traits<InputIterator>::value_type must be char32_t.                        //
//                                                                                      //
//--------------------------------------------------------------------------------------//





//--------------------------------------------------------------------------------------//
//                                  Implementation                                      //
//--------------------------------------------------------------------------------------//

//------------------------------------  helpers  ---------------------------------------//

namespace detail
{

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
    while (c & mask)
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

}  // namespace detail

//--------------------------------------------------------------------------------------//
//              from_iterator - converts the FromEncoding to UTF-32                     //
//--------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------//
//                                                                                      //
//                          from_iterator implementation                                //
//                                                                                      //
//   Adapts InputIterator with value_type FromCharT and FromEncoding into an iterator   //
//   with value_type char32_t and UTF-32 encoding.                                      //
//                                                                                      //
//--------------------------------------------------------------------------------------//

template <class FromEncoding, class FromCharT, class InputIterator>  // primary template;
class from_iterator;                                      //  specializations do the work

//  Remark: FromCharT is not stricktly necessary, but allows early detection of mismatch
//  between encoding and InputIterator value_type.

//-----------------------------  UTF-8 from_iterator  ----------------------------------//

template <class FromCharT, class InputIterator>
//  FromCharT may be char or, if sizeof(wchar_t) is 1, wchar_t
class from_iterator<utf8, FromCharT, InputIterator>
  : public boost::iterator_facade<from_iterator<utf8, FromCharT, InputIterator>,
      char32_t, std::input_iterator_tag, const char32_t>
{
  typedef boost::iterator_facade<from_iterator<utf8, FromCharT, InputIterator>,
       char32_t, std::input_iterator_tag, const char32_t> base_type;

  // special values for pending iterator reads:
  BOOST_STATIC_CONSTANT(char32_t, read_pending = 0xffffffffu);

  typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

  BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, FromCharT>::value),
         "InputIterator value_type must be FromCharT for this from_iterator");

  InputIterator     m_first;    // current position
  InputIterator     m_last;
  mutable char32_t  m_value;    // current value or read_pending
  bool              m_default_end;

public:

  // end iterator
  from_iterator() : m_default_end(true) {}

  // by_null
  from_iterator(InputIterator first) : m_first(first), m_last(first),
    m_default_end(false)
  {
    for (;
      *m_last != typename std::iterator_traits<InputIterator>::value_type();
      ++m_last)
    {
    }
    m_value = read_pending;
  }

  // by range
  template <class T>
  from_iterator(InputIterator first, T last,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type = 0)
    : m_first(first), m_last(end), m_default_end(false)
  {
    m_value = read_pending;
  }

  // by_size
  from_iterator(InputIterator first, std::size_t sz)
    : m_first(first), m_last(first), m_default_end(false)
  {
    std::advance(m_last, sz);
    m_value = read_pending;
  }

  typename base_type::reference
    dereference() const
  {
    BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
      "Attempt to dereference end iterator");
    if (m_value == read_pending)
      extract_current();
    return m_value;
  }

  bool equal(const from_iterator& that) const
  {
    if (m_default_end || m_first == m_last)
      return that.m_default_end || that.m_first == that.m_last;
    if (that.m_default_end || that.m_first == that.m_last)
      return false;
    return m_first == that.m_first;
  }

  void increment()
  {
    BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
      "Attempt to increment end iterator");
    unsigned count = detail::utf8_byte_count(*m_first);
    std::advance(m_first, count);
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
    BOOST_ASSERT_MSG(m_first != m_last,
      "Internal logic error: extracting from end iterator");
    m_value = static_cast<char32_t>(static_cast<::boost::uint8_t>(*m_first));
    // we must not have a continuation character:
    if ((m_value & 0xC0u) == 0x80u)
      invalid_sequence();
    // see how many extra byts we have:
    unsigned extra = detail::utf8_trailing_byte_count(*m_first);
    // extract the extra bits, 6 from each extra byte:
    InputIterator next(m_first);
    for (unsigned c = 0; c < extra; ++c)
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
    if (m_value > static_cast<char32_t>(0x10FFFFu))
      invalid_sequence();
  }
};

//-------------------------------  UTF-16 from_iterator  -------------------------------//

template <class FromCharT, class InputIterator>
//  FromCharT may be char16_t or, if sizeof(wchar_t) is 2, wchar_t
class from_iterator<utf16, FromCharT, InputIterator>
  : public boost::iterator_facade<from_iterator<utf16, FromCharT, InputIterator>,
      char32_t, std::input_iterator_tag, const char32_t>
{
    typedef boost::iterator_facade<from_iterator<utf16, FromCharT, InputIterator>,
      char32_t, std::input_iterator_tag, const char32_t> base_type;

    // special values for pending iterator reads:
    BOOST_STATIC_CONSTANT(char32_t, read_pending = 0xffffffffu);

    typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

    BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, FromCharT>::value),
      "InputIterator value_type must be FromCharT for this from_iterator");

    InputIterator     m_first;   // current position
    InputIterator     m_last;  
    mutable char32_t  m_value;     // current value or read_pending
    bool              m_default_end;

  public:

  // end iterator
  from_iterator() : m_default_end(true) {}

  // by_null
  from_iterator(InputIterator first) : m_first(first), m_last(first),
    m_default_end(false) 
  {
    for (;
          *m_last != typename std::iterator_traits<InputIterator>::value_type();
          ++m_last) {}
    m_value = read_pending;
  }

  // by range
  template <class T>
  from_iterator(InputIterator first, T last,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type = 0)
    : m_first(first), m_last(end), m_default_end(false) { m_value = read_pending; }

  // by_size
  from_iterator(InputIterator first, std::size_t sz)
    : m_first(first), m_last(first), m_default_end(false)
  {
    std::advance(m_last, sz);
    m_value = read_pending;
  }

    typename base_type::reference
      dereference() const
    {
      BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
        "Attempt to dereference end iterator");
      if (m_value == read_pending)
          extract_current();
      return m_value;
    }

    bool equal(const from_iterator& that) const 
    {
      if (m_default_end || m_first == m_last)
        return that.m_default_end || that.m_first == that.m_last;
      if (that.m_default_end || that.m_first == that.m_last)
        return false;
      return m_first == that.m_first;
    }

    void increment()
    {
      BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
        "Attempt to increment end iterator");
      // skip high surrogate first if there is one:
      if(detail::is_high_surrogate(*m_first))
        ++m_first;
      ++m_first;
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
      m_value = static_cast<char32_t>(static_cast< ::boost::uint16_t>(*m_first));
      // if the last value is a high surrogate then adjust m_first and m_value as needed:
      if(detail::is_high_surrogate(*m_first))
      {
          // precondition; next value must have be a low-surrogate:
          InputIterator next(m_first);
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

//--------------------------  UTF-32 (i.e. non-converting) from_iterator  -------------------------//

template <class FromCharT, class InputIterator>
  //  FromCharT may be char32_t or, if sizeof(wchar_t) is 4, wchar_t
class from_iterator<utf32, FromCharT, InputIterator>
  : public boost::iterator_facade<from_iterator<utf32, FromCharT, InputIterator>,
      char32_t, std::input_iterator_tag, const char32_t>
{
  InputIterator  m_first;
  InputIterator  m_last;
  bool           m_default_end;

public:
  typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

  BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, FromCharT>::value),
    "InputIterator value_type must be same as FromCharT for this from_iterator");

  // end iterator
  from_iterator() : m_default_end(true) {}

  // by_null
  from_iterator(InputIterator first) : m_first(first), m_last(first),
    m_default_end(false) 
  {
    for (;
          *m_last != typename std::iterator_traits<InputIterator>::value_type();
          ++m_last) {}
  }

  // by range
  template <class T>
  from_iterator(InputIterator first, T last,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
    : m_first(first), m_last(end), m_default_end(false) {}

  // by_size
  from_iterator(InputIterator first, std::size_t sz)
    : m_first(first), m_last(first), m_default_end(false)
  {
    std::advance(m_last, sz);
  }

  char32_t dereference() const
  {
    BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
      "Attempt to dereference end iterator");
    return *m_first;
  }

  bool equal(const from_iterator& that) const
  {
    if (m_default_end || m_first == m_last)
      return that.m_default_end || that.m_first == that.m_last;
    if (that.m_default_end || that.m_first == that.m_last)
      return false;
    return m_first == that.m_first;
  }

  void increment()
  {
    BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
      "Attempt to increment end iterator");
    ++m_first;
  }
};

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                         to_iterator implementation                                   //
//                                                                                      //
//   Adapts UTF-32 InputIterator to an iterator with value_type ToCharT & ToEncoding    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

template <class ToEncoding, class ToCharT, class InputIterator>       // primary template;
class to_iterator;                                         //  specializations do the work

//-------------------------------  UTF-8 to_iterator  ----------------------------------//

template <class ToCharT, class InputIterator>
class to_iterator<utf8, ToCharT, InputIterator>
  : public boost::iterator_facade<to_iterator<utf8, ToCharT, InputIterator>,
      ToCharT, std::input_iterator_tag, const ToCharT>
{
  typedef boost::iterator_facade<to_iterator<utf8, ToCharT, InputIterator>,
    ToCharT, std::input_iterator_tag, const ToCharT> base_type;
   
  typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
//       "InputIterator value_type must be char32_t for this iterator");

  InputIterator     m_first;
  mutable char32_t  m_values[5];
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
    if(m_first == that.m_first)
    {
        // either the m_first's must be equal, or one must be 0 and 
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
        ++m_first;
    }
  }

  // construct:
  to_iterator() : m_first(InputIterator()), m_current(0)
  {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
    m_values[4] = 0;
  }
  to_iterator(InputIterator first) : m_first(first), m_current(4)
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
    boost::uint32_t c = *m_first;
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

//-------------------------------  UTF-16 to_iterator  ---------------------------------//

template <class ToCharT, class InputIterator>
class to_iterator<utf16, ToCharT, InputIterator>
  : public boost::iterator_facade<to_iterator<utf16, ToCharT, InputIterator>,
  ToCharT, std::input_iterator_tag, const ToCharT>
{
  typedef boost::iterator_facade<to_iterator<utf16, ToCharT,InputIterator>,
  ToCharT, std::input_iterator_tag, const ToCharT> base_type;

  typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

//     BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
//       "InputIterator value_type must be char32_t for this iterator");

  InputIterator     m_first;
  mutable ToCharT   m_values[3];
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
    if(m_first == that.m_first)
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
        ++m_first;
    }
  }

  // construct:
  to_iterator() : m_first(InputIterator()), m_current(0)
  {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
  }
  to_iterator(InputIterator first) : m_first(first), m_current(2)
  {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
  }
private:

  void extract_current()const
  {
    // begin by checking for a code point out of range:
    ::boost::uint32_t v = *m_first;
    if(v >= 0x10000u)
    {
        if(v > 0x10FFFFu)
          detail::invalid_utf32_code_point(*m_first);
        // split into two surrogates:
        m_values[0] = static_cast<ToCharT>(v >> 10) + detail::high_surrogate_base;
        m_values[1] = static_cast<ToCharT>(v & detail::ten_bit_mask)
          + detail::low_surrogate_base;
        m_current = 0;
        BOOST_ASSERT(detail::is_high_surrogate(m_values[0]));
        BOOST_ASSERT(detail::is_low_surrogate(m_values[1]));
    }
    else
    {
        // 16-bit code point:
      m_values[0] = static_cast<ToCharT>(*m_first);
        m_values[1] = 0;
        m_current = 0;
        // value must not be a surrogate:
        if(detail::is_surrogate(m_values[0]))
          detail::invalid_utf32_code_point(*m_first);
    }
  }
};

//-------------------  UTF-32 (i.e. non-converting) to_iterator  -----------------------//
  
template <class ToCharT, class InputIterator>
class to_iterator<utf32, ToCharT, InputIterator>
  : public boost::iterator_facade<to_iterator<utf32, ToCharT, InputIterator>,
      ToCharT, std::input_iterator_tag, const ToCharT>
{
  InputIterator m_itr;
public:
  to_iterator() : m_itr(InputIterator()) {}
  to_iterator(InputIterator itr) : m_itr(itr) {}
  ToCharT dereference() const { return *m_itr; }
  bool equal(const to_iterator& that) const {return m_itr == that.m_itr;}
  void increment() { ++m_itr; }
};

////--------------------------------------------------------------------------------------//
////                                    narrow codec                                      //
////--------------------------------------------------------------------------------------//
//
//#ifdef BOOST_WINDOWS_API
//namespace detail
//{
//  //  for this proof-of-concept implementation, use codepage 437 tables at end of header
//  BOOST_STRING_INTEROP_DECL extern const char16_t  to_utf16[];  
//  BOOST_STRING_INTEROP_DECL extern const unsigned char to_char[];
//  BOOST_STRING_INTEROP_DECL extern const boost::uint8_t slice_index[];
//}
//
//class narrow
//{
//public:
//  typedef char value_type;
//
//  template <class charT>
//  struct codec { typedef narrow type; };
//
//  //  narrow::from_iterator  -----------------------------------------------------------//
//  //
//  //  meets the DefaultCtorEndIterator requirements
//
//  template <class InputIterator>  
//  class from_iterator
//   : public boost::iterator_facade<from_iterator<InputIterator>,
//       char32_t, std::input_iterator_tag, const char32_t>
//  {
//    typedef boost::iterator_facade<from_iterator<InputIterator>,
//      char32_t, std::input_iterator_tag, const char32_t> base_type;
//
//    typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;
//
//    BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char>::value),
//      "InputIterator value_type must be char for this from_iterator");
//    BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
//    BOOST_STATIC_ASSERT(sizeof(char32_t)*CHAR_BIT == 32);
//    
//    InputIterator  m_first;
//    InputIterator  m_last;
//    bool           m_default_end;
//
//  public:
//
//    // end iterator
//    from_iterator() : m_default_end(true) {}
//
//    // by_null
//    from_iterator(InputIterator first) : m_first(first), m_last(first),
//      m_default_end(false) 
//    {
//      for (;
//           *m_last != typename std::iterator_traits<InputIterator>::value_type();
//           ++m_last) {}
//    }
//
//    // by range
//    template <class T>
//    from_iterator(InputIterator first, T last,
//      // enable_if ensures 2nd argument of 0 is treated as size, not range end
//      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
//      : m_first(first), m_last(end), m_default_end(false) {}
//
//    // by_size
//    from_iterator(InputIterator first, std::size_t sz)
//      : m_first(first), m_last(first), m_default_end(false) {std::advance(m_last, sz);}
//
//    char32_t dereference() const
//    {
//      BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
//        "Attempt to dereference end iterator");
//      unsigned char c = static_cast<unsigned char>(*m_first);
//      return static_cast<char32_t>(string_interop::detail::to_utf16[c]);
//    }
//
//    bool equal(const from_iterator& that) const
//    {
//      if (m_default_end || m_first == m_last)
//        return that.m_default_end || that.m_first == that.m_last;
//      if (that.m_default_end || that.m_first == that.m_last)
//        return false;
//      return m_first == that.m_first;
//    }
//
//    void increment()
//    { 
//      BOOST_ASSERT_MSG(!m_default_end && m_first != m_last,
//        "Attempt to increment end iterator");
//      ++m_first;
//    }
//  };
//
//  //  narrow::to_iterator  -------------------------------------------------------------//
//  //
//  //  meets the DefaultCtorEndIterator requirements
//
//  template <class InputIterator>  
//  class to_iterator
//   : public boost::iterator_facade<to_iterator<InputIterator>,
//       char, std::input_iterator_tag, const char>
//  {
//     typedef boost::iterator_facade<to_iterator<InputIterator>,
//       char, std::input_iterator_tag, const char> base_type;
//   
//     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;
//
//     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
//       "InputIterator value_type must be char32_t for this iterator");
//     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);
//
//     InputIterator m_first;
//
//  public:
//    // construct:
//    to_iterator() : m_first(InputIterator()) {}
//    to_iterator(InputIterator first) : m_first(first) {}
//
//    char dereference() const
//    {
//      BOOST_ASSERT_MSG(m_first != InputIterator(),
//        "Attempt to dereference end iterator");
//      char32_t c = *m_first;
//      if (c & 0xFFFF0000U)
//        return '?';
//      //cout << "*** c is " << hex << c << '\n';
//      //cout << "    to_slice[c >> 7] << 7 is "
//      //  << unsigned int(string_interop::detail::slice_index[c >> 7] << 7) << '\n';
//      return static_cast<char>(string_interop::detail::to_char
//        [
//          (string_interop::detail::slice_index[c >> 7] << 7) | (c & 0x7f)
//        ]);
//    }
//
//    bool equal(const to_iterator& that) const
//    {
//      return m_first == that.m_first;
//    }
//
//    void increment()
//    { 
//      BOOST_ASSERT_MSG(m_first != InputIterator(),
//        "Attempt to increment end iterator");
//      ++m_first;  // may change m_first to end iterator
//    }
//
//  };  // to_iterator
//};  // narrow
//
//#endif

//--------------------------------------------------------------------------------------//
//                                 conversion_iterator                                  //
//--------------------------------------------------------------------------------------//

//  A conversion_iterator composes a to_iterator and a from_iterator into an iterator
//  that adapts an InputIterator with the FromEncoding to behave as an iterator to a
//  sequence having the ToEncoding.

template <class ToEncoding, class FromEncoding, class InputIterator>
class conversion_iterator
  : public to_iterator<typename ToEncoding::actual_encoding,
      typename ToEncoding::value_type,
        from_iterator<typename FromEncoding::actual_encoding,
          typename FromEncoding::value_type, InputIterator> >
{
public:
  typedef typename from_iterator<typename FromEncoding::actual_encoding,
    typename FromEncoding::value_type, InputIterator>     from_iterator_type;

  typedef typename to_iterator<typename ToEncoding::actual_encoding,
    typename ToEncoding::value_type, from_iterator_type>  to_iterator_type;

  conversion_iterator() BOOST_DEFAULTED

  conversion_iterator(InputIterator first)
    : to_iterator_type(from_iterator_type(first)) {}

  template <class U>
  conversion_iterator(InputIterator first, U end,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, U>, void* >::type = 0)
    : to_iterator_type(from_iterator_type(begin, end)) {}

  conversion_iterator(InputIterator first, std::size_t sz)
    : to_iterator_type(from_iterator_type(begin, sz)) {}
};

//--------------------------------------------------------------------------------------//

//  what might the to_*string family of functions look like if implemented with a
//  conversion_iterator?

template <class FromEncoding = defaulted, class T>          
std::u16string to_u16string(const T& source)
{
  typedef typename FromEncoding::template
    encoding<typename std::iterator_traits<T>::value_type>::type from_encoding;

  typedef conversion_iterator<utf16, from_encoding, T> iterator_type;

  return std::u16string(iterator_type(source), iterator_type());
}

//template <class T, class U, class FromEncoding>
//std::u16string to_u16string(InputIterator first, U arg2)
//{
//  typedef conversion_iterator<...> iterator_type;
//  return std::u16string(iterator_type(first, arg2), iterator_type());
//}

}  // namespace string_interop
}  // namespace boost

//----------------------------------------------------------------------------//

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif  // BOOST_STRING_INTEROP_HPP
