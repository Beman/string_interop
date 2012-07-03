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

#if !defined(BOOST_STRING_INTEROP_HPP)
#define BOOST_STRING_INTEROP_HPP

#include <boost/interop/detail/config.hpp>
#include <boost/interop/string_types.hpp>
#include <boost/interop/detail/is_iterator.hpp>
//#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <stdexcept>
#include <sstream>
//#include <ios>
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

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

//  codecs
class narrow;   // native encoding for char
class wide;     // native encoding for wchar_t
class utf8;     // UTF-8 encoding for char
class utf16;    // UTF-16 encoding for char16_t
class utf32;    // UTF-32 encoding for char32_t
class auto_detect;

//  conversion_iterator
template <class ToCodec, class FromCodec, class InputIterator>
  class conversion_iterator;

//  see auto_codec trait below

//  see convert() functions below

//---------------------------------  Requirements  -------------------------------------//
//
//  DefaultCtorEndIterator:
//
//  For an iterator of type T, T() constructs the end iterator.
//
//  Codec:
//
//  from_iterator meets the DefaultCtorEndIterator requirements.
//  ForwardIterator must be default constructible, and yield a singular value.
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
#pragma warning(push)
#pragma warning(disable:4100)
#endif
inline void invalid_utf32_code_point(::boost::uint32_t val)
{
   std::stringstream ss;
   ss << "Invalid UTF-32 code point U+" << std::showbase << std::hex << val << " encountered while trying to encode UTF-16 sequence";
   std::out_of_range e(ss.str());
   BOOST_INTEROP_THROW(e);
}
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

} // namespace detail

//------------------------------  auto_codec type selector  ----------------------------//

  template <class CharT> struct auto_codec;
  template <> struct auto_codec<char> { typedef narrow type; };
  template <> struct auto_codec<wchar_t> { typedef wide type; };
  template <> struct auto_codec<u16_t> { typedef utf16 type; };
  template <> struct auto_codec<u32_t> { typedef utf32 type; };

//----------------------------  auto_detect pseudo codec  ------------------------------//

class auto_detect
{
public:
  template <class CharT>
  struct codec
  { 
    typedef typename auto_codec<CharT>::type type; 
  };

};

//----------------------------------  narrow codec  ------------------------------------//

namespace detail
{
  // for this proof-of-concept, simply linking in codec tables is sufficient
  extern const boost::u16_t  to_utf16[];  
  extern const unsigned char to_char[];
  extern const boost::uint8_t slice_index[];
}

class narrow
{
public:
  typedef char value_type;

  template <class CharT>
  struct codec { typedef narrow type; };

  //  narrow::from_iterator  -----------------------------------------------------------//

  template <class ForwardIterator>  
  class from_iterator
   : public boost::iterator_facade<from_iterator<ForwardIterator>,
       u32_t, std::input_iterator_tag, const u32_t>
  {
    typedef boost::iterator_facade<from_iterator<ForwardIterator>,
      u32_t, std::input_iterator_tag, const u32_t> base_type;

    typedef typename std::iterator_traits<ForwardIterator>::value_type base_value_type;

    BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
    BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);
    
    ForwardIterator  m_begin;
    ForwardIterator  m_end;

  public:
    from_iterator() : m_begin(ForwardIterator()), m_end(m_begin) {}

    // by_null
    from_iterator(ForwardIterator begin) : m_begin(begin), m_end(begin) 
    {
      for (;
           *m_end != typename std::iterator_traits<ForwardIterator>::value_type();
           ++m_end) {}
    }

    // by range
    template <class T>
    from_iterator(ForwardIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<ForwardIterator, T>, void >::type* x=0)
      : m_begin(begin), m_end(end) {}

    // by_size
    from_iterator(ForwardIterator begin, std::size_t sz)
      : m_begin(begin), m_end(begin) {std::advance(m_end, sz);}

    u32_t dereference() const
    {
      BOOST_ASSERT_MSG(m_begin != m_end, "Attempt to dereference end iterator");
      unsigned char c = static_cast<unsigned char>(*m_begin);
      return static_cast<u32_t>(interop::detail::to_utf16[c]);
    }

    bool equal(const from_iterator& that) const
    {
      if (m_begin == m_end)
        return that.m_begin == that.m_end;
      if (that.m_begin != that.m_end)
        return m_begin == that.m_begin;
      return false;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_begin != m_end, "Attempt to increment end iterator");
      ++m_begin;
    }
  };

  //  narrow::to_iterator  -------------------------------------------------------------//

  template <class InputIterator>  
  class to_iterator
   : public boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     InputIterator m_iterator;

  public:
    // construct:
    to_iterator() BOOST_DEFAULTED;
    to_iterator(InputIterator begin) : m_iterator(begin) {}

    char dereference() const
    {
      BOOST_ASSERT_MSG(m_iterator != InputIterator(),
        "Attempt to dereference end iterator");
      u32_t c = *m_iterator;
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
      return m_iterator == that.m_iterator;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_iterator != InputIterator(),
        "Attempt to increment end iterator");
      ++m_iterator;  // may change m_iterator to end iterator
    }

    InputIterator& base() {return m_iterator;}

  };  // to_iterator
};  // narrow

////-----------------------------------  wide codec  -------------------------------------//
//
//class wide
//{
//public:
//  typedef wchar_t value_type;
//  template <class CharT> struct codec { typedef wide type; };
//
//  //  wide::from_iterator  -------------------------------------------------------------//
//
//
//  //  wide::to_iterator  ---------------------------------------------------------------//
//
//};
//
////-----------------------------------  utf8 codec  -------------------------------------//
//
//class utf8
//{
//public:
//  typedef char value_type;
//  template <class CharT> struct codec { typedef utf8 type; };
//
//  //  utf8::from_iterator  -------------------------------------------------------------//
//
//  template <class InputIterator>
//  class from_iterator
//   : public boost::iterator_facade<from_iterator<InputIterator>,
//       u32_t, std::input_iterator_tag, const u32_t>,
//     public EndPolicy<InputIterator>
//  {
//     typedef boost::iterator_facade<from_iterator<InputIterator>,
//       u32_t, std::input_iterator_tag, const u32_t> base_type;
//     // special values for pending iterator reads:
//     BOOST_STATIC_CONSTANT(u32_t, read_pending = 0xffffffffu);
//
//     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;
//
//     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
//     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);
//
//     // for the end itorator (i.e. m_is_end == true), other values are unspecified
//     InputIterator  m_iterator;  // current position
//     mutable u32_t  m_value;     // current value or read_pending
//     mutable bool   m_is_end;    // true iff this is the end iterator
//    // note: InputIterator is not require to be default construtable [iterator.iterators],
//    // so m_iterator == InputIterator() cannot be used to denote the end iterator.
//
//   public:
//
//    // end iterator
//    from_iterator() : m_is_end(true) {}  // construct end iterator
//
//    // by_null
//    from_iterator(InputIterator begin) : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_null_policy>::value,
//          "Constructor not valid unless EndPolicy is by_null");
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//    // by range
//    template <class T>
//    from_iterator(InputIterator begin, T end,
//      // enable_if ensures 2nd argument of 0 is treated as size, not range end
//      typename boost::enable_if<boost::is_same<InputIterator, T>, void >::type* x=0)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_range_policy>::value, "Constructor not valid unless EndPolicy is by_range");
//      this->end(end);
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//    // by_size
//    from_iterator(InputIterator begin, std::size_t sz)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_size_policy>::value, "Constructor not valid unless EndPolicy is by_size");
//      this->size(sz);
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//     typename base_type::reference
//        dereference() const
//     {
//        BOOST_ASSERT_MSG(!m_is_end,
//          "Attempt to dereference end iterator");
//        if (m_value == read_pending)
//           extract_current();
//        return m_value;
//     }
//
//     bool equal(const from_iterator& that) const
//     {
//       if (m_is_end)
//         return that.m_is_end;
//       if (that.m_is_end)
//         return false;
//       return m_iterator == that.m_iterator;
//     }
//
//     void increment()
//     {
//        BOOST_ASSERT_MSG(!m_is_end,
//          "Attempt to increment end iterator");
//        unsigned count = detail::utf8_byte_count(*m_iterator);
//        std::advance(m_iterator, count);
//        this->advance(count);
//        if (this->is_end(m_iterator))
//          m_is_end = true;
//        m_value = read_pending;
//     }
//  private:
//     static void invalid_sequence()
//     {
//        std::out_of_range e(
//          "Invalid UTF-8 sequence encountered while trying to encode UTF-32 character");
//        BOOST_INTEROP_THROW(e);
//     }
//     void extract_current()const
//     {
//        BOOST_ASSERT_MSG(!m_is_end,
//          "Internal logic error: extracting from end iterator");
//        m_value = static_cast<u32_t>(static_cast< ::boost::uint8_t>(*m_iterator));
//        // we must not have a continuation character:
//        if((m_value & 0xC0u) == 0x80u)
//           invalid_sequence();
//        // see how many extra byts we have:
//        unsigned extra = detail::utf8_trailing_byte_count(*m_iterator);
//        // extract the extra bits, 6 from each extra byte:
//        InputIterator next(m_iterator);
//        for(unsigned c = 0; c < extra; ++c)
//        {
//           ++next;
//           m_value <<= 6;
//           m_value += static_cast<boost::uint8_t>(*next) & 0x3Fu;
//        }
//        // we now need to remove a few of the leftmost bits, but how many depends
//        // upon how many extra bytes we've extracted:
//        static const boost::uint32_t masks[4] = 
//        {
//           0x7Fu,
//           0x7FFu,
//           0xFFFFu,
//           0x1FFFFFu,
//        };
//        m_value &= masks[extra];
//        // check the result:
//        if(m_value > static_cast<u32_t>(0x10FFFFu))
//           invalid_sequence();
//     }
//  };
//
//  //  utf8::to_iterator  ---------------------------------------------------------------//
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
//     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);
//
//  public:
//
//     typename base_type::reference
//     dereference()const
//     {
//        if(m_current == 4)
//           extract_current();
//        return m_values[m_current];
//     }
//     bool equal(const to_iterator& that)const
//     {
//        if(m_iterator == that.m_iterator)
//        {
//           // either the m_current's must be equal, or one must be 0 and 
//           // the other 4: which means neither must have bits 1 or 2 set:
//           return (m_current == that.m_current)
//              || (((m_current | that.m_current) & 3) == 0);
//        }
//        return false;
//     }
//     void increment()
//     {
//        // if we have a pending read then read now, so that we know whether
//        // to skip a position, or move to a low-surrogate:
//        if(m_current == 4)
//        {
//           // pending read:
//           extract_current();
//        }
//        // move to the next surrogate position:
//        ++m_current;
//        // if we've reached the end skip a position:
//        if(m_values[m_current] == 0)
//        {
//           m_current = 4;
//           ++m_iterator;
//        }
//     }
//
//     InputIterator& base() { return m_iterator; }
//
//     // construct:
//     to_iterator() : m_iterator(), m_current(0)
//     {
//        m_values[0] = 0;
//        m_values[1] = 0;
//        m_values[2] = 0;
//        m_values[3] = 0;
//        m_values[4] = 0;
//     }
//     to_iterator(InputIterator b) : m_iterator(b), m_current(4)
//     {
//        m_values[0] = 0;
//        m_values[1] = 0;
//        m_values[2] = 0;
//        m_values[3] = 0;
//        m_values[4] = 0;
//        BOOST_XOP_LOG("utf-8 from utf-32");
//    }
//  private:
//
//     void extract_current()const
//     {
//        boost::uint32_t c = *m_iterator;
//        if(c > 0x10FFFFu)
//           detail::invalid_utf32_code_point(c);
//        if(c < 0x80u)
//        {
//           m_values[0] = static_cast<unsigned char>(c);
//           m_values[1] = static_cast<unsigned char>(0u);
//           m_values[2] = static_cast<unsigned char>(0u);
//           m_values[3] = static_cast<unsigned char>(0u);
//        }
//        else if(c < 0x800u)
//        {
//           m_values[0] = static_cast<unsigned char>(0xC0u + (c >> 6));
//           m_values[1] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//           m_values[2] = static_cast<unsigned char>(0u);
//           m_values[3] = static_cast<unsigned char>(0u);
//        }
//        else if(c < 0x10000u)
//        {
//           m_values[0] = static_cast<unsigned char>(0xE0u + (c >> 12));
//           m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
//           m_values[2] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//           m_values[3] = static_cast<unsigned char>(0u);
//        }
//        else
//        {
//           m_values[0] = static_cast<unsigned char>(0xF0u + (c >> 18));
//           m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 12) & 0x3Fu));
//           m_values[2] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
//           m_values[3] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//        }
//        m_current= 0;
//     }
//     InputIterator m_iterator;
//     mutable u8_t m_values[5];
//     mutable unsigned m_current;
//  };
//
//};
//
////----------------------------------  utf16 codec  -------------------------------------//
//
//class utf16
//{
//public:
//  typedef u16_t value_type;
//  template <class CharT> struct codec { typedef utf16 type; };
//
//  //  utf16::from_iterator  ------------------------------------------------------------//
//
//  template <class InputIterator>
//  class from_iterator
//   : public boost::iterator_facade<from_iterator<InputIterator>,
//       u32_t, std::input_iterator_tag, const u32_t>,
//     public EndPolicy<InputIterator>
//  {
//     typedef boost::iterator_facade<from_iterator<InputIterator>,
//       u32_t, std::input_iterator_tag, const u32_t> base_type;
//     // special values for pending iterator reads:
//     BOOST_STATIC_CONSTANT(u32_t, read_pending = 0xffffffffu);
//
//     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;
//
//     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
//     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);
//
//     // for the end itorator (i.e. m_is_end == true), other values are unspecified
//     InputIterator  m_iterator;  // current position
//     mutable u32_t  m_value;     // current value or read_pending
//     mutable bool   m_is_end;    // true iff this is the end iterator
//    // note: InputIterator is not require to be default construtable [iterator.iterators],
//    // so m_iterator == InputIterator() cannot be used to denote the end iterator.
//
//   public:
//
//    // end iterator
//    from_iterator() : m_is_end(true) {} // construct end iterator
//
//    // by_null
//    from_iterator(InputIterator begin) : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_null_policy>::value, "Constructor not valid unless EndPolicy is by_null");
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//    // by range
//    template <class T>
//    from_iterator(InputIterator begin, T end,
//      // enable_if ensures 2nd argument of 0 is treated as size, not range end
//      typename boost::enable_if<boost::is_same<InputIterator, T>, void >::type* x=0)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_range_policy>::value, "Constructor not valid unless EndPolicy is by_range");
//      this->end(end);
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//    // by_size
//    from_iterator(InputIterator begin, std::size_t sz)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_size_policy>::value, "Constructor not valid unless EndPolicy is by_size");
//      this->size(sz);
//      m_is_end = this->is_end(m_iterator);
//      m_value = read_pending;
//    }
//
//     typename base_type::reference
//        dereference() const
//     {
//        BOOST_ASSERT_MSG(!m_is_end, 
//          "Attempt to dereference end iterator");
//        if (m_value == read_pending)
//           extract_current();
//        return m_value;
//     }
//
//     bool equal(const from_iterator& that) const 
//     {
//       if (m_is_end)
//         return that.m_is_end;
//       if (that.m_is_end)
//         return false;
//       return m_iterator == that.m_iterator;
//     }
//
//     void increment()
//     {
//       BOOST_ASSERT_MSG(!m_is_end,
//         "Attempt to increment end iterator");
//       // skip high surrogate first if there is one:
//       if(detail::is_high_surrogate(*m_iterator))
//       {
//         ++m_iterator;
//         this->advance();
//       }
//       ++m_iterator;
//       this->advance();
//       if (this->is_end(m_iterator))
//          m_is_end = true;
//        m_value = read_pending;
//     }
//
//  private:
//     static void invalid_code_point(::boost::uint16_t val)
//     {
//        std::stringstream ss;
//        ss << "Misplaced UTF-16 surrogate U+" << std::showbase << std::hex << val
//           << " encountered while trying to encode UTF-32 sequence";
//        std::out_of_range e(ss.str());
//        BOOST_INTEROP_THROW(e);
//     }
//     static void invalid_sequence()
//     {
//        std::out_of_range e(
//          "Invalid UTF-16 sequence encountered while trying to encode UTF-32 character");
//        BOOST_INTEROP_THROW(e);
//     }
//     void extract_current() const
//     {
//        m_value = static_cast<u32_t>(static_cast< ::boost::uint16_t>(*m_iterator));
//        // if the last value is a high surrogate then adjust m_iterator and m_value as needed:
//        if(detail::is_high_surrogate(*m_iterator))
//        {
//           // precondition; next value must have be a low-surrogate:
//           InputIterator next(m_iterator);
//           u16_t t = *++next;
//           if((t & 0xFC00u) != 0xDC00u)
//              invalid_code_point(t);
//           m_value = (m_value - detail::high_surrogate_base) << 10;
//           m_value |= (static_cast<u32_t>(
//             static_cast<u16_t>(t)) & detail::ten_bit_mask);
//        }
//        // postcondition; result must not be a surrogate:
//        if(detail::is_surrogate(m_value))
//           invalid_code_point(static_cast<u16_t>(m_value));
//     }
//  };
//
//  //  utf16::to_iterator  --------------------------------------------------------------//
//
//  template <class InputIterator>
//  class to_iterator
//   : public boost::iterator_facade<to_iterator<InputIterator>,
//      u16_t, std::input_iterator_tag, const u16_t>
//  {
//     typedef boost::iterator_facade<to_iterator<InputIterator>,
//       u16_t, std::input_iterator_tag, const u16_t> base_type;
//
//     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;
//
//     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//     BOOST_STATIC_ASSERT(sizeof(u16_t)*CHAR_BIT == 16);
//
//  public:
//
//     typename base_type::reference
//     dereference()const
//     {
//        if(m_current == 2)
//           extract_current();
//        return m_values[m_current];
//     }
//     bool equal(const to_iterator& that)const
//     {
//        if(m_iterator == that.m_iterator)
//        {
//           // Both m_currents must be equal, or both even
//           // this is the same as saying their sum must be even:
//           return (m_current + that.m_current) & 1u ? false : true;
//        }
//        return false;
//     }
//     void increment()
//     {
//        // if we have a pending read then read now, so that we know whether
//        // to skip a position, or move to a low-surrogate:
//        if(m_current == 2)
//        {
//           // pending read:
//           extract_current();
//        }
//        // move to the next surrogate position:
//        ++m_current;
//        // if we've reached the end skip a position:
//        if(m_values[m_current] == 0)
//        {
//           m_current = 2;
//           ++m_iterator;
//        }
//     }
//
//     InputIterator& base() {return m_iterator;}
//
//     // construct:
//     to_iterator() : m_iterator(), m_current(0)
//     {
//        m_values[0] = 0;
//        m_values[1] = 0;
//        m_values[2] = 0;
//     }
//     to_iterator(InputIterator b) : m_iterator(b), m_current(2)
//     {
//        m_values[0] = 0;
//        m_values[1] = 0;
//        m_values[2] = 0;
//        BOOST_XOP_LOG("utf-16 from utf-32");
//    }
//  private:
//
//     void extract_current()const
//     {
//        // begin by checking for a code point out of range:
//        ::boost::uint32_t v = *m_iterator;
//        if(v >= 0x10000u)
//        {
//           if(v > 0x10FFFFu)
//              detail::invalid_utf32_code_point(*m_iterator);
//           // split into two surrogates:
//           m_values[0] = static_cast<u16_t>(v >> 10) + detail::high_surrogate_base;
//           m_values[1] = static_cast<u16_t>(v & detail::ten_bit_mask)
//             + detail::low_surrogate_base;
//           m_current = 0;
//           BOOST_ASSERT(detail::is_high_surrogate(m_values[0]));
//           BOOST_ASSERT(detail::is_low_surrogate(m_values[1]));
//        }
//        else
//        {
//           // 16-bit code point:
//           m_values[0] = static_cast<u16_t>(*m_iterator);
//           m_values[1] = 0;
//           m_current = 0;
//           // value must not be a surrogate:
//           if(detail::is_surrogate(m_values[0]))
//              detail::invalid_utf32_code_point(*m_iterator);
//        }
//     }
//     InputIterator m_iterator;
//     mutable u16_t m_values[3];
//     mutable unsigned m_current;
//  };
//
//};
//
////----------------------------------  utf32 codec  -------------------------------------//
//
//class utf32
//{
//public:
//  typedef u32_t value_type;
//  template <class CharT> struct codec { typedef utf32 type; };
//
//  //  utf32::from_iterator  ------------------------------------------------------------//
//
//  template <class InputIterator>
//  class from_iterator
//    : public boost::iterator_facade<from_iterator<InputIterator>,
//        u32_t, std::input_iterator_tag, const u32_t>,
//      public EndPolicy<InputIterator>
//  {
//    // for the end itorator (i.e. m_is_end == true), value of m_iterator unspecified
//    InputIterator  m_iterator;
//    mutable bool   m_is_end;    // true iff this is the end iterator
//    // note: InputIterator is not require to be default construtable [iterator.iterators],
//    // so m_iterator == InputIterator() cannot be used to denote the end iterator.
//
//  public:
//    from_iterator() : m_is_end(true) {}  // construct end iterator
//
//    // by_null
//    from_iterator(InputIterator begin) : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_null_policy>::value, "Constructor not valid unless EndPolicy is by_null");
//      m_is_end = this->is_end(m_iterator);
//    }
//
//    // by range
//    template <class T>
//    from_iterator(InputIterator begin, T end,
//      // enable_if ensures 2nd argument of 0 is treated as size, not range end
//      typename boost::enable_if<boost::is_same<InputIterator, T>, void >::type* x=0)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_range_policy>::value, "Constructor not valid unless EndPolicy is by_range");
//      this->end(end);
//      m_is_end = this->is_end(m_iterator);
//   }
//
//    // by_size
//    from_iterator(InputIterator begin, std::size_t sz)
//      : m_iterator(begin) 
//    {
//      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
//        by_size_policy>::value, "Constructor not valid unless EndPolicy is by_size");
//      this->size(sz);
//      m_is_end = this->is_end(m_iterator);
//    }
//
//    u32_t dereference() const
//    {
//      BOOST_ASSERT_MSG(!m_is_end,
//        "Attempt to dereference end iterator");
//      return *m_iterator;
//    }
//
//    bool equal(const from_iterator& that) const
//    {
//      if (m_is_end)
//        return that.m_is_end;
//      if (that.m_is_end)
//        return false;
//      return m_iterator == that.m_iterator;
//    }
//
//    void increment()
//    {
//      BOOST_ASSERT_MSG(!m_is_end,
//        "Attempt to increment end iterator");
//      ++m_iterator;
//      this->advance();
//      if (this->is_end(m_iterator))
//        m_is_end = true;
//    }
//  };
//
//  //  utf32::to_iterator  --------------------------------------------------------------//
//
//  template <class InputIterator>
//  class to_iterator
//   : public boost::iterator_facade<to_iterator<InputIterator>,
//      u32_t, std::input_iterator_tag, const u32_t>
//  {
//    InputIterator m_itr;
//  public:
//    to_iterator() {}
//    to_iterator(InputIterator itr) : m_itr(itr) {}
//    u32_t dereference() const { return *m_itr; }
//    bool equal(const to_iterator& that) const {return m_itr == that.m_itr;}
//    void increment() { ++m_itr; }
//    InputIterator& base() {return m_itr;}
//  };
//
//};

//--------------------------------------------------------------------------------------//
//                                 conversion_iterator                                  //
//--------------------------------------------------------------------------------------//

//  A conversion_iterator composes a ToCodec's to_iterator and a FromCodec's from_iterator
//  into a single iterator adapts an InputIterator to FromCodec's value_type to act as an
//  iterator to the ToCodec's value_type.

template <class ToCodec, class FromCodec, class InputIterator>
class conversion_iterator
  : public ToCodec::template to_iterator<
      typename FromCodec::template from_iterator<InputIterator> >
{
public:
  typedef typename FromCodec::template from_iterator<InputIterator>  from_iterator_type;
  typedef typename ToCodec::template to_iterator<from_iterator_type> to_iterator_type;

  conversion_iterator() BOOST_DEFAULTED

  conversion_iterator(InputIterator begin)
    : to_iterator_type(from_iterator_type(begin)) {}

  template <class U>
  conversion_iterator(InputIterator begin, U end,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, U>, void >::type* x=0)
    : to_iterator_type(from_iterator_type(begin, end)) {}

  conversion_iterator(InputIterator begin, std::size_t sz)
    : to_iterator_type(from_iterator_type(begin, sz)) {}
};

//--------------------------------------------------------------------------------------//
//                                 convert function                                     //
//--------------------------------------------------------------------------------------//

//  container
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = auto_detect,
          class ToContainer = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToContainer,
# endif
          class FromString>
  // enable_if resolves ambiguity with single iterator overload
typename boost::disable_if<boost::is_iterator<FromString>,
ToContainer>::type convert(const FromString& x)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template codec<typename FromString::value_type>::type,
    const typename FromString::value_type*>
      iter_type;

  ToContainer tmp;

  //  use x.c_str(), x.c_str()+x.size() rather than x.begin(), x.end() to avoid over
  //  aggressive asserts in the VC++ standard library
  iter_type itr(x.c_str(), x.c_str()+x.size());
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//  null terminated iterator
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = auto_detect,
          class ToContainer = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToContainer,
# endif
          class InputIterator>
  // enable_if resolves ambiguity with FromContainer overload
typename boost::enable_if<boost::is_iterator<InputIterator>,
ToContainer>::type convert(InputIterator begin)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  ToContainer tmp;
  iter_type itr(begin);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//  iterator, size
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = auto_detect,
          class ToContainer = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToContainer,
# endif
          class InputIterator>
ToContainer convert(InputIterator begin, std::size_t sz)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  ToContainer tmp;
  iter_type itr(begin, sz);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

//  iterator range
template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = auto_detect,
          class ToContainer = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToContainer,
# endif
          class InputIterator, class InputIterator2>
  // enable_if ensures 2nd argument of 0 is treated as size, not range end
typename boost::enable_if<boost::is_iterator<InputIterator2>,
ToContainer>::type convert(InputIterator begin, InputIterator2 end)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  ToContainer tmp;
  iter_type itr(begin, end);
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

}  // namespace interop
}  // namespace boost

#endif  // BOOST_STRING_INTEROP_HPP
