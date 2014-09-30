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
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <locale>
#include <algorithm>
#include <limits.h>  // for CHAR_BIT, WCHAR_MAX
#include <cstring>   // for strlen

#include <boost/config/abi_prefix.hpp> // must be the last #include

#ifndef BOOST_NO_DEFAULTED_FUNCTIONS
# define BOOST_STR_IOP_DEFAULTED = default;
#else
# define BOOST_STR_IOP_DEFAULTED {}
#endif

namespace boost
{
namespace string_interop
{

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  template <class charT, class ErrorHandler, class CodecvtMgr>
    class generic_narrow;
  template <class charT, class ErrorHandler>
    class generic_utf32;
  template <class charT, class ErrorHandler>
    class generic_utf16;

  template <class charT>
  class default_error_handler
  {
  public:
    void operator()(const std::string& msg) const
    {
      std::cout << "default error policy" << std::endl;
      throw std::runtime_error(msg);   // TODO need a codec_error
    }
  };

  typedef std::codecvt<char32_t, char, std::mbstate_t>  codecvt_type;

  //class default_codecvt_mgr
  //{
  //public:
  //  const codecvt_type* operator()() BOOST_NOEXCEPT
  //  {
  //    return detail::default_codecvt();  // kick the can down the road
  //  }
  //};

  template <class Codecvt>
  class shared_codecvt_mgr
  {
    boost::shared_ptr<Codecvt>  m_codecvt;
  public:
    shared_codecvt_mgr() : m_codecvt(new Codecvt) {}
    BOOST_DEFAULTED_FUNCTION(shared_codecvt_mgr(const shared_codecvt_mgr& rhs), 
      {m_codecvt = rhs.m_codecvt;})

    const codecvt_type* operator()() const BOOST_NOEXCEPT
    {
      BOOST_ASSERT(m_codecvt.get());  // internal implementation error
      return m_codecvt.get();
    }
  };

  class dynamic_codecvt_mgr
  {
    boost::shared_ptr<codecvt_type> m_codecvt;
  public:
    dynamic_codecvt_mgr(boost::shared_ptr<codecvt_type> shared) BOOST_NOEXCEPT
      : m_codecvt(shared) {}

    const codecvt_type* operator()() BOOST_NOEXCEPT
    {
      BOOST_ASSERT(m_codecvt.get());  // internal implementation error
      return m_codecvt.get();
    }
  };


  //  codecs
//  class utf8;                                        // UTF-8 encoding for char
//#ifdef BOOST_WINDOWS_API
//  typedef detail::generic_utf16<wchar_t>  wide;      // UTF-16 encoding for wchar_t
//#else
//  // hack: assume POSIX wide encoding is UTF-16 or UTF-32
//# if WCHAR_MAX == 0xffff
//  typedef detail::generic_utf16<wchar_t>  wide;      // UTF-16 encoding for wchar_t
//# else
//  typedef detail::generic_utf32<wchar_t>  wide;      // UTF-32 encoding for wchar_t
//# endif
//#endif
//  typedef detail::generic_utf16<char16_t>    utf16;

  //typedef generic_narrow<char, default_error_handler<char>, default_codecvt_mgr> narrow;
  typedef generic_utf32<char32_t, default_error_handler<char32_t> >                 utf32;

  class default_codec;

//  select_codec type selector

  template <class charT> struct select_codec;
  //template <> struct select_codec<char>    { typedef narrow type; };
  //template <> struct select_codec<wchar_t> { typedef wide type; };
  //template <> struct select_codec<char16_t>   { typedef utf16 type; };
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
//                                   generic_utf32                                      //
//--------------------------------------------------------------------------------------//

template <class charT, class ErrorHandler>
class generic_utf32
{
  ErrorHandler m_error_handler;

public:
  typedef generic_utf32<charT, ErrorHandler>  type;
  typedef charT                               value_type;
  typedef ErrorHandler                        error_handler_type;

  template <class InputIterator>
  class from_iterator;
  template <class InputIterator>
  class to_iterator;

  explicit generic_utf32(ErrorHandler ep = ErrorHandler()) BOOST_NOEXCEPT
    : m_error_handler(ep) {}

  //  make iterator functions

  template <class InputIterator>
  from_iterator<InputIterator> from(InputIterator begin)
  {
    return from_iterator(begin, m_error_handler);
  }

  template <class InputIterator>
  to_iterator<InputIterator> to(InputIterator begin)
  {
    return to_iterator<InputIterator>(begin, m_error_handler);
  }

  //  generic_utf32::from_iterator  ---------------------------------------------------//

  template <class InputIterator>
  class from_iterator
    : public boost::iterator_facade<from_iterator<InputIterator>,
      value_type, std::input_iterator_tag, const value_type>
  {
    BOOST_STATIC_ASSERT_MSG((boost::is_same<typename std::iterator_traits<InputIterator>::value_type,
      value_type>::value),
      "InputIterator value_type must be same as codec value_type");

    InputIterator       m_begin;
    InputIterator       m_end;
    error_handler_type  m_error;
    bool                m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // ntcts
    from_iterator(InputIterator begin, error_handler_type eh)
      : m_begin(begin), m_end(begin), m_error(eh), m_default_end(false)
    {
      for (;
           *m_end != typename std::iterator_traits<InputIterator>::value_type();
           ++m_end) {}
    }

    // range
    template <class T>
    from_iterator(InputIterator begin, T end, error_handler_type eh,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type =0)
      : m_begin(begin), m_end(end), m_error(eh), m_default_end(false)
    {}

    // sized
    from_iterator(InputIterator begin, std::size_t sz, error_handler_type eh)
      : m_begin(begin), m_end(begin), m_error(eh), m_default_end(false)
    {
      std::advance(m_end, sz);
    }

    value_type dereference() const
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
  //
  //  Remarks: to_iterator meets the DefaultCtorEndIterator requirements
  //  Requires: InputIterator meets the DefaultCtorEndIterator requirements

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
     value_type, std::input_iterator_tag, const value_type>
  {
    InputIterator m_itr;
  public:
    to_iterator() : m_itr(InputIterator()) {}

    to_iterator(InputIterator itr, error_handler_type) : m_itr(itr) {}

    value_type dereference() const { return *m_itr; }

    bool equal(const to_iterator& that) const {return m_itr == that.m_itr;}

    void increment() {++m_itr;}
  };

};

//--------------------------------------------------------------------------------------//
//                                   generic_utf16                                      //
//--------------------------------------------------------------------------------------//

template <class charT, class ErrorHandler>
class generic_utf16
{
  ErrorHandler m_error_handler;

public:
  typedef generic_utf16<charT, ErrorHandler>  type;
  typedef charT                               value_type;
  typedef ErrorHandler                        error_handler_type;

  template <class InputIterator>
  class from_iterator;
  template <class InputIterator>
  class to_iterator;

  explicit generic_utf16(ErrorHandler ep = ErrorHandler()) BOOST_NOEXCEPT
    : m_error_handler(ep) {}

  //  make iterator functions

  template <class InputIterator>
  from_iterator<InputIterator> from(InputIterator begin)
  {
    return from_iterator(begin, m_error_handler);
  }

  template <class InputIterator>
  to_iterator<InputIterator> to(InputIterator begin)
  {
    return to_iterator<InputIterator>(begin, m_error_handler);
  }

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

     BOOST_STATIC_ASSERT_MSG((boost::is_same<base_value_type, value_type>::value),
       "InputIterator value_type must be charT for this from_iterator");

     InputIterator       m_begin;        // current position
     InputIterator       m_end;
     error_handler_type  m_error_handler;
     mutable char32_t    m_value;        // current value or read_pending
     bool                m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // ntcts
    from_iterator(InputIterator begin, error_handler_type eh)
      : m_begin(begin), m_end(begin), m_error_handler(eh), m_value(read_pending), m_default_end(false) 
    {
      for (; *m_end != typename std::iterator_traits<InputIterator>::value_type(); ++m_end) {}
    }

    // range
    template <class T>
    from_iterator(InputIterator begin, T end, error_handler_type eh,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type = 0)
      : m_begin(begin), m_end(end), m_error_handler(eh), m_value(read_pending),
        m_default_end(false) {}

    // sized
    from_iterator(InputIterator begin, std::size_t sz, error_handler_type eh)
      : m_begin(begin), m_end(begin), m_error_handler(eh), m_value(read_pending), m_default_end(false)
    {
      std::advance(m_end, sz);
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
  //
  //  Remarks: to_iterator meets the DefaultCtorEndIterator requirements
  //  Requires: InputIterator meets the DefaultCtorEndIterator requirements

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
     value_type, std::input_iterator_tag, const value_type>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       value_type, std::input_iterator_tag, const value_type> base_type;

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     InputIterator       m_begin;
     error_handler_type  m_error_handler;
     mutable value_type  m_values[3];
     mutable unsigned    m_current;

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
     to_iterator(InputIterator b, error_handler_type eh)
       : m_begin(b), m_error_handler(eh), m_current(2)
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

//--------------------------------------------------------------------------------------//
//                                  generic_narrow                                      //
//--------------------------------------------------------------------------------------//

template <class charT, class ErrorHandler, class CodecvtMgr>
class generic_narrow
{
  ErrorHandler  m_error_handler;
  CodecvtMgr    m_codecvt_mgr;

  static const int max_char_buf = 4;  // enough for UTF-8; just a guess for other encodings.
                               // codecvt will report failure if it encounters a UTF-32
                               // code-point that needs more buffer space.
public:
  typedef generic_narrow<charT, ErrorHandler, CodecvtMgr>   type;
  typedef charT                                             value_type;
  typedef ErrorHandler                                      error_handler_type;
  typedef CodecvtMgr                                        codecvt_mgr_type;

  class from_iterator;
  template <class InputIterator>
  class to_iterator;

  //  constructors

  explicit generic_narrow(ErrorHandler ep = ErrorHandler(), 
    CodecvtMgr ccvt = CodecvtMgr()) : m_error_handler(ep), m_codecvt_mgr(ccvt)  {}

  explicit generic_narrow(CodecvtMgr ccvt)  // m_error_handler is default constructed 
    :  m_codecvt_mgr(ccvt) {}

  //  make iterators

  from_iterator from(const char* begin)
  {
    return from_iterator(begin, m_error_handler, m_codecvt_mgr);
  }

  template <class InputIterator>
  to_iterator<InputIterator> to(InputIterator begin)
  {
    return to_iterator<InputIterator>(begin, m_error_handler, m_codecvt_mgr);
  }

  //  generic_narrow::from_iterator  ---------------------------------------------------//
  //
  //  meets the DefaultCtorEndIterator requirements

  class from_iterator
   : public boost::iterator_facade<from_iterator,
       char32_t, std::input_iterator_tag, const char32_t>
  {
    typedef boost::iterator_facade<from_iterator,
      char32_t, std::input_iterator_tag, const char32_t> base_type;

    // special values for pending iterator reads:
    BOOST_STATIC_CONSTANT(char32_t, read_pending = 0xffffffffu);

    const char*             m_begin;
    const char*             m_end;
    mutable const char*     m_next;
    error_handler_type      m_error;
    codecvt_mgr_type        m_codecvt;
    mutable char32_t        m_value;     // current value or read_pending
    mutable std::mbstate_t  m_state;
    bool                    m_default_end;

  public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // ntcts
    from_iterator(const char* begin, error_handler_type ep, codecvt_mgr_type cp)
      : m_begin(begin), m_end(begin), m_error(ep), m_codecvt(cp), m_value(read_pending),
        m_state(std::mbstate_t()), m_default_end(false)
    {
      for (; *m_end != '\0'; ++m_end) {}
    }

    // range
    template <class T>
    from_iterator(const char* begin, T end, error_handler_type ep, codecvt_mgr_type cp,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<const char*, T>, void* >::type = 0)
      : m_begin(begin), m_end(end), m_error(ep), m_codecvt(cp), m_value(read_pending),
      m_state(std::mbstate_t()), m_default_end(false) {}

    // sized
    from_iterator(const char* begin, std::size_t sz, error_handler_type ep, codecvt_mgr_type cp
      )
      : m_begin(begin), m_end(begin), m_error(ep), m_codecvt(cp), m_value(read_pending),
        m_state(std::mbstate_t()), m_default_end(false)
    {
      std::advance(m_end, sz);
    }

    char32_t dereference() const
    {
      BOOST_ASSERT_MSG(!m_default_end && m_begin != m_end,
        "Attempt to dereference end iterator");

      if (m_value == read_pending)
      {
#     ifndef NDEBUG
        m_next = 0;
#      endif
        char32_t*  to_next;
        std::codecvt_base::result result = 
          m_codecvt()->
            in(m_state, m_begin, m_end, m_next, &m_value, &m_value+1, to_next);
        if (result != std::codecvt_base::ok) 
        {
          m_error("barf");  // TODO
        }
        BOOST_ASSERT(m_next && m_begin != m_next);  // result was ok, so verify have made progress
      }
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

      //  call dereference() if a read is pending
      if (m_value == read_pending)
        dereference();

      //  since dereference() has been called, m_next points to next code point
      m_begin = m_next;
      m_value = read_pending;
    }
  };  // from_iterator

  //  generic_narrow::to_iterator  -----------------------------------------------------//
  //
  //  Remarks: to_iterator meets the DefaultCtorEndIterator requirements
  //  Requires: InputIterator meets the DefaultCtorEndIterator requirements

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

     error_handler_type      m_error;
     codecvt_mgr_type        m_codecvt;
     mutable std::mbstate_t  m_state;
     mutable InputIterator   m_from;      // value_type is char32_t
     mutable uint8_t         m_to;        // index into m_values; always 0 if read pending  
     mutable uint8_t         m_to_count;  // 0 if and only if read pending
     mutable char            m_values[max_char_buf];

  public:
    // construct:
    to_iterator() : m_from(InputIterator()), m_to(0), m_to_count(0) {}  // end iterator

    to_iterator(InputIterator begin, error_handler_type ep, codecvt_mgr_type cp)
      : m_error(ep), m_codecvt(cp), m_state(std::mbstate_t()), m_from(begin),
        m_to(0), m_to_count(0)
    {}

    bool equal(const to_iterator& that) const
    {
      return m_from == that.m_from && m_to == that.m_to;
    }

    char dereference() const
    {
      BOOST_ASSERT_MSG(m_from != InputIterator(),
        "Attempt to dereference end iterator");
      
      if (!m_to_count)  // read pending
      {
        const char32_t* from_next;
        char*           to_next;
        char32_t from = *m_from;  // std::codecvt::out needs char32_t*, not InputIterator
        std::codecvt_base::result result =
          m_codecvt()->
            out(m_state, &from, &from+1, from_next,
                &m_values[0], &m_values[max_char_buf], to_next);

        if (result != std::codecvt_base::ok)
        {
          m_error("barf");  // TODO
        }
        m_to = 0;
        m_to_count = to_next - &m_values[0];
      }
      BOOST_ASSERT_MSG(m_to_count,
        "generic_narrow::to_iterator internal implementation error");
      return m_values[m_to];
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_from != InputIterator(),
        "Attempt to increment end iterator");

      if (m_to == m_to_count)  // if read pending
        dereference();
      ++m_to;
      if (m_to == m_to_count)  // reached end
      {
        m_to = m_to_count = 0;  // mark as read pending
        ++m_from;               // move to next UTF-32 code point
      }
    }

  };  // to_iterator
};  // narrow

//--------------------------------------------------------------------------------------//
//                                     utf8 codec                                       //
//--------------------------------------------------------------------------------------//

template <class ErrorHandler>
class generic_utf8
{
  ErrorHandler  m_error_handler;
public:
  typedef generic_utf8<ErrorHandler>  type;
  typedef char                        value_type;
  typedef ErrorHandler                error_handler_type;

  template <class InputIterator>
  class from_iterator;
  template <class InputIterator>
  class to_iterator;

  explicit generic_utf8(ErrorHandler ep = ErrorHandler()) BOOST_NOEXCEPT
    : m_error_handler(ep) {}

  //  make iterator functions

  template <class InputIterator>
  from_iterator<InputIterator> from(InputIterator begin)
  {
    return from_iterator<InputIterator>(begin, m_error_handler);
  }

  template <class InputIterator>
  to_iterator<InputIterator> to(InputIterator begin)
  {
    return to_iterator<InputIterator>(begin, m_error_handler);
  }

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

     error_handler_type  m_error;
     InputIterator       m_begin;  // current position
     InputIterator       m_end;
     mutable char32_t    m_value;    // current value or read_pending
     bool                m_default_end;

   public:

    // end iterator
    from_iterator() : m_default_end(true) {}

    // ntcts
    from_iterator(InputIterator begin, error_handler_type eh)
      : m_error(eh), m_begin(begin), m_end(begin),
        m_value(read_pending), m_default_end(false) 
    {
      for (; *m_end != typename std::iterator_traits<InputIterator>::value_type();
        ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end, error_handler_type eh,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void* >::type = 0)
      : m_error(eh), m_begin(begin), m_end(end),
        m_value(read_pending), m_default_end(false) {}

    // by_size
    from_iterator(InputIterator begin, std::size_t sz, error_handler_type eh)
      : m_error(eh), m_begin(begin), m_end(begin),
        m_value(read_pending), m_default_end(false)
    {
      std::advance(m_end, sz);
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
  //  Remarks: to_iterator meets the DefaultCtorEndIterator requirements
  //  Requires: InputIterator meets the DefaultCtorEndIterator requirements

  template <class InputIterator>
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     // TODO: why does this fail to compile?
     //BOOST_ASSERT_MSG((boost::is_same<base_value_type, char32_t>::value),
     //  "InputIterator value_type must be char32_t for this iterator");

     InputIterator       m_from;
     error_handler_type  m_error;

     // TODO: shouldn't this be unsigned char? Was it char32_t in John's original code:
     mutable char32_t   m_values[5];  // zero terminated; thus room for any value UTF-8
                                      // sequence + zero termination 
     mutable unsigned   m_current;    // index into m_values; 4 implies read pending.

  public:

     typename base_type::reference
     dereference()const
     {
        if(m_current == 4)  // read pending?
           extract_current();
        return m_values[m_current];
     }

     bool equal(const to_iterator& that)const
     {
        if(m_from == that.m_from)
        {
           // either the m_current's must be equal, or one must be 0 and 
           // the other 4 (i.e. read pending): thus neither must have bits 1 or 2 set:
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
           m_current = 4;  // mark as read pending
           ++m_from;
        }
     }

     // construct:
     to_iterator() : m_from(InputIterator()), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
     }
     to_iterator(InputIterator b, error_handler_type eh)
       : m_from(b), m_error(eh), m_current(4)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
    }
  private:

     void extract_current() const
     {
        boost::uint32_t c = *m_from;
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

template <class InputIterator, class FromCodec, class ToCodec>
class conversion_iterator
  : public ToCodec::template to_iterator<typename FromCodec::from_iterator>
{
public:
  typedef typename FromCodec::from_iterator                           from_iterator_type;
  typedef typename ToCodec::template to_iterator<from_iterator_type>  to_iterator_type;

  conversion_iterator() BOOST_STR_IOP_DEFAULTED

  conversion_iterator(InputIterator begin, FromCodec fc = FromCodec(),
    ToCodec tc = ToCodec())
    : to_iterator_type(tc.to(fc.from(begin))) {}

  template <class U>
  conversion_iterator(InputIterator begin, U end, FromCodec fc = FromCodec(),
    ToCodec tc = ToCodec(),
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, U>, void* >::type = 0)
    : to_iterator_type(tc.to(fc.from(begin, end))) {}

  conversion_iterator(InputIterator begin, std::size_t sz, FromCodec fc = FromCodec(),
    ToCodec tc = ToCodec())
    : to_iterator_type(tc.to(fc.from(begin, sz))) {}
};


}  // namespace string_interop
}  // namespace boost

//----------------------------------------------------------------------------//

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif  // BOOST_STRING_INTEROP_HPP
