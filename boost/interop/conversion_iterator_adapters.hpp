//  boost/interop/conversion_iterator_adapters.hpp  ------------------------------------//

//  Copyright Beman Dawes 2011
//  Copyright (c) 2004 John Maddock

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//  John Maddock introduced me to the idea of performing conversion via iterator        //
//  Adapters. The code for the UTF-8 <--> UTF-32 and UTF-16 <--> UTF-32 adapers was     //
//  adapted from his boost/regex/pending/unicode_iterator.hpp header.                   //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP)
#define BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP

#include <boost/interop/detail/config.hpp>
#include <boost/interop/string_0x.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/static_assert.hpp>
#include <stdexcept>
#include <sstream>
#include <ios>
#include <limits.h> // CHAR_BIT

namespace boost
{

//--------------------------------------------------------------------------------------//
//  For n encodings, it is desirable to provide 2n adapters rather than n squared       //
//  adapters. This is achieved by conceptually converting to UTF-32 and from UTF-32 as  //
//  an intermediate encoding, and then composing a converting_iterator from one         //
//  to_utf32_iterator and one from_utf32_iterator. For efficiency, specializations of   //
//  converting_iterator can provide direct conversion without an intermedate UTF32 step.//
//--------------------------------------------------------------------------------------//

//-------------------------------  to_utf32_iterator  ---------------------------------//

  template <class BaseIterator, class T>  // primary template
  class to_utf32_iterator;           // partial specializations must be provided

  template <class BaseIterator>
  class to_utf32_iterator<BaseIterator, u8_t>;

  template <class BaseIterator>
  class to_utf32_iterator<BaseIterator, u16_t>;

  template <class BaseIterator>
  class to_utf32_iterator<BaseIterator, u32_t>
    : public BaseIterator
      { public: to_utf32_iterator(BaseIterator it) : BaseIterator(it) {} };

//------------------------------  from_utf32_iterator  --------------------------------//

  template <class BaseIterator, class T>  // primary template
  class from_utf32_iterator;         // partial specializations must be provided

  template <class BaseIterator>
  class from_utf32_iterator<BaseIterator, u8_t>;

  template <class BaseIterator>
  class from_utf32_iterator<BaseIterator, u16_t>;

  template <class BaseIterator>
  class from_utf32_iterator<BaseIterator, u32_t>
    : public BaseIterator
      { public: from_utf32_iterator(BaseIterator it) : BaseIterator(it) {} };

//-------------------------------  converting_iterator  --------------------------------//

  template <class BaseIterator, class From, class To>  // primary template
  class converting_iterator                   // partial specializations *may* be provided
    :  public from_utf32_iterator<to_utf32_iterator<BaseIterator, From>, To>
  {
  public:
    explicit converting_iterator(BaseIterator it)
      : from_utf32_iterator<to_utf32_iterator<BaseIterator, From>, To>(it) {}
  };

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
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

//--------------------------------  specializations  -----------------------------------//

//---------------------------  <u8_t> to_utf32_iterator  ------------------------------//

  template <class BaseIterator>
  class to_utf32_iterator<BaseIterator, u8_t>
   : public boost::iterator_facade<to_utf32_iterator<BaseIterator, u8_t>, u32_t, std::bidirectional_iterator_tag, const u32_t>
{
   typedef boost::iterator_facade<to_utf32_iterator<BaseIterator, u8_t>, u32_t, std::bidirectional_iterator_tag, const u32_t> base_type;
   // special values for pending iterator reads:
   BOOST_STATIC_CONSTANT(u32_t, pending_read = 0xffffffffu);

   typedef typename std::iterator_traits<BaseIterator>::value_type base_value_type;

   BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
   BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

public:
   typename base_type::reference
      dereference()const
   {
      if(m_value == pending_read)
         extract_current();
      return m_value;
   }
   bool equal(const to_utf32_iterator& that)const
   {
      return m_position == that.m_position;
   }
   void increment()
   {
      // skip high surrogate first if there is one:
      unsigned c = detail::utf8_byte_count(*m_position);
      std::advance(m_position, c);
      m_value = pending_read;
   }
   void decrement()
   {
      // Keep backtracking until we don't have a trailing character:
      unsigned count = 0;
      while((*--m_position & 0xC0u) == 0x80u) ++count;
      // now check that the sequence was valid:
      if(count != detail::utf8_trailing_byte_count(*m_position))
         invalid_sequence();
      m_value = pending_read;
   }
   BaseIterator base()const
   {
      return m_position;
   }
   // construct:
   to_utf32_iterator() : m_position()
   {
      m_value = pending_read;
   }
   to_utf32_iterator(BaseIterator b) : m_position(b)
   {
      m_value = pending_read;
   }
   //
   // Checked constructor:
   //
   to_utf32_iterator(BaseIterator b, BaseIterator start, BaseIterator end) : m_position(b)
   {
      m_value = pending_read;
      //
      // We must not start with a continuation character, or end with a 
      // truncated UTF-8 sequence otherwise we run the risk of going past
      // the start/end of the underlying sequence:
      //
      if(start != end)
      {
         unsigned char v = *start;
         if((v & 0xC0u) == 0x80u)
            invalid_sequence();
         if((b != start) && (b != end) && ((*b & 0xC0u) == 0x80u))
            invalid_sequence();
         BaseIterator pos = end;
         do
         {
            v = *--pos;
         }
         while((start != pos) && ((v & 0xC0u) == 0x80u));
         std::ptrdiff_t extra = detail::utf8_byte_count(v);
         if(std::distance(pos, end) < extra)
            invalid_sequence();
      }
   }
private:
   static void invalid_sequence()
   {
      std::out_of_range e("Invalid UTF-8 sequence encountered while trying to encode UTF-32 character");
      BOOST_INTEROP_THROW(e);
   }
   void extract_current()const
   {
      m_value = static_cast<u32_t>(static_cast< ::boost::uint8_t>(*m_position));
      // we must not have a continuation character:
      if((m_value & 0xC0u) == 0x80u)
         invalid_sequence();
      // see how many extra byts we have:
      unsigned extra = detail::utf8_trailing_byte_count(*m_position);
      // extract the extra bits, 6 from each extra byte:
      BaseIterator next(m_position);
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
      if(m_value > static_cast<u32_t>(0x10FFFFu))
         invalid_sequence();
   }
   BaseIterator m_position;
   mutable u32_t m_value;
};

//---------------------------  <u16_t> to_utf32_iterator  -----------------------------//

//  template <class BaseIterator>
//  class to_utf32_iterator<BaseIterator, u16_t>
//   : public boost::iterator_facade<u16_to_u32_iterator<BaseIterator, u32_t>, u32_t, std::bidirectional_iterator_tag, const u32_t>
//{
//   typedef boost::iterator_facade<u16_to_u32_iterator<BaseIterator, u32_t>, u32_t, std::bidirectional_iterator_tag, const u32_t> base_type;
//   // special values for pending iterator reads:
//   BOOST_STATIC_CONSTANT(u32_t, pending_read = 0xffffffffu);
//
//   typedef typename std::iterator_traits<BaseIterator>::value_type base_value_type;
//
//   BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
//   BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);
//
//public:
//   typename base_type::reference
//      dereference()const
//   {
//      if(m_value == pending_read)
//         extract_current();
//      return m_value;
//   }
//   bool equal(const u16_to_u32_iterator& that)const
//   {
//      return m_position == that.m_position;
//   }
//   void increment()
//   {
//      // skip high surrogate first if there is one:
//      if(detail::is_high_surrogate(*m_position)) ++m_position;
//      ++m_position;
//      m_value = pending_read;
//   }
//   void decrement()
//   {
//      --m_position;
//      // if we have a low surrogate then go back one more:
//      if(detail::is_low_surrogate(*m_position)) 
//         --m_position;
//      m_value = pending_read;
//   }
//   BaseIterator base()const
//   {
//      return m_position;
//   }
//   // construct:
//   u16_to_u32_iterator() : m_position()
//   {
//      m_value = pending_read;
//   }
//   u16_to_u32_iterator(BaseIterator b) : m_position(b)
//   {
//      m_value = pending_read;
//   }
//   //
//   // Range checked version:
//   //
//   u16_to_u32_iterator(BaseIterator b, BaseIterator start, BaseIterator end) : m_position(b)
//   {
//      m_value = pending_read;
//      //
//      // The range must not start with a low surrogate, or end in a high surrogate,
//      // otherwise we run the risk of running outside the underlying input range.
//      // Likewise b must not be located at a low surrogate.
//      //
//      boost::uint16_t val;
//      if(start != end)
//      {
//         if((b != start) && (b != end))
//         {
//            val = *b;
//            if(detail::is_surrogate(val) && ((val & 0xFC00u) == 0xDC00u))
//               invalid_code_point(val);
//         }
//         val = *start;
//         if(detail::is_surrogate(val) && ((val & 0xFC00u) == 0xDC00u))
//            invalid_code_point(val);
//         val = *--end;
//         if(detail::is_high_surrogate(val))
//            invalid_code_point(val);
//      }
//   }
//private:
//   static void invalid_code_point(::boost::uint16_t val)
//   {
//      std::stringstream ss;
//      ss << "Misplaced UTF-16 surrogate U+" << std::showbase << std::hex << val << " encountered while trying to encode UTF-32 sequence";
//      std::out_of_range e(ss.str());
//      BOOST_INTEROP_THROW(e);
//   }
//   void extract_current()const
//   {
//      m_value = static_cast<u32_t>(static_cast< ::boost::uint16_t>(*m_position));
//      // if the last value is a high surrogate then adjust m_position and m_value as needed:
//      if(detail::is_high_surrogate(*m_position))
//      {
//         // precondition; next value must have be a low-surrogate:
//         BaseIterator next(m_position);
//         ::boost::uint16_t t = *++next;
//         if((t & 0xFC00u) != 0xDC00u)
//            invalid_code_point(t);
//         m_value = (m_value - detail::high_surrogate_base) << 10;
//         m_value |= (static_cast<u32_t>(static_cast< ::boost::uint16_t>(t)) & detail::ten_bit_mask);
//      }
//      // postcondition; result must not be a surrogate:
//      if(detail::is_surrogate(m_value))
//         invalid_code_point(static_cast< ::boost::uint16_t>(m_value));
//   }
//   BaseIterator m_position;
//   mutable u32_t m_value;
//};

//--------------------------  <u8_t> from_utf32_iterator  -----------------------------//

//  template <class BaseIterator>
//  class from_utf32_iterator<BaseIterator, u8_t>
//   : public boost::iterator_facade<u32_to_u8_iterator<BaseIterator, u8_t>, u8_t, std::bidirectional_iterator_tag, const u8_t>
//{
//   typedef boost::iterator_facade<u32_to_u8_iterator<BaseIterator, u8_t>, u8_t, std::bidirectional_iterator_tag, const u8_t> base_type;
//   
//   typedef typename std::iterator_traits<BaseIterator>::value_type base_value_type;
//
//   BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//   BOOST_STATIC_ASSERT(sizeof(u8_t)*CHAR_BIT == 8);
//
//public:
//   typename base_type::reference
//      dereference()const
//   {
//      if(m_current == 4)
//         extract_current();
//      return m_values[m_current];
//   }
//   bool equal(const u32_to_u8_iterator& that)const
//   {
//      if(m_position == that.m_position)
//      {
//         // either the m_current's must be equal, or one must be 0 and 
//         // the other 4: which means neither must have bits 1 or 2 set:
//         return (m_current == that.m_current)
//            || (((m_current | that.m_current) & 3) == 0);
//      }
//      return false;
//   }
//   void increment()
//   {
//      // if we have a pending read then read now, so that we know whether
//      // to skip a position, or move to a low-surrogate:
//      if(m_current == 4)
//      {
//         // pending read:
//         extract_current();
//      }
//      // move to the next surrogate position:
//      ++m_current;
//      // if we've reached the end skip a position:
//      if(m_values[m_current] == 0)
//      {
//         m_current = 4;
//         ++m_position;
//      }
//   }
//   void decrement()
//   {
//      if((m_current & 3) == 0)
//      {
//         --m_position;
//         extract_current();
//         m_current = 3;
//         while(m_current && (m_values[m_current] == 0))
//            --m_current;
//      }
//      else
//         --m_current;
//   }
//   BaseIterator base()const
//   {
//      return m_position;
//   }
//   // construct:
//   u32_to_u8_iterator() : m_position(), m_current(0)
//   {
//      m_values[0] = 0;
//      m_values[1] = 0;
//      m_values[2] = 0;
//      m_values[3] = 0;
//      m_values[4] = 0;
//   }
//   u32_to_u8_iterator(BaseIterator b) : m_position(b), m_current(4)
//   {
//      m_values[0] = 0;
//      m_values[1] = 0;
//      m_values[2] = 0;
//      m_values[3] = 0;
//      m_values[4] = 0;
//   }
//private:
//
//   void extract_current()const
//   {
//      boost::uint32_t c = *m_position;
//      if(c > 0x10FFFFu)
//         detail::invalid_utf32_code_point(c);
//      if(c < 0x80u)
//      {
//         m_values[0] = static_cast<unsigned char>(c);
//         m_values[1] = static_cast<unsigned char>(0u);
//         m_values[2] = static_cast<unsigned char>(0u);
//         m_values[3] = static_cast<unsigned char>(0u);
//      }
//      else if(c < 0x800u)
//      {
//         m_values[0] = static_cast<unsigned char>(0xC0u + (c >> 6));
//         m_values[1] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//         m_values[2] = static_cast<unsigned char>(0u);
//         m_values[3] = static_cast<unsigned char>(0u);
//      }
//      else if(c < 0x10000u)
//      {
//         m_values[0] = static_cast<unsigned char>(0xE0u + (c >> 12));
//         m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
//         m_values[2] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//         m_values[3] = static_cast<unsigned char>(0u);
//      }
//      else
//      {
//         m_values[0] = static_cast<unsigned char>(0xF0u + (c >> 18));
//         m_values[1] = static_cast<unsigned char>(0x80u + ((c >> 12) & 0x3Fu));
//         m_values[2] = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
//         m_values[3] = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
//      }
//      m_current= 0;
//   }
//   BaseIterator m_position;
//   mutable u8_t m_values[5];
//   mutable unsigned m_current;
//};

//--------------------------  <u16_t> from_utf32_iterator  ----------------------------//

//  template <class BaseIterator>
//  class from_utf32_iterator<BaseIterator, u16_t>
//   : public boost::iterator_facade<u32_to_u16_iterator<BaseIterator, u16_t>, u16_t, std::bidirectional_iterator_tag, const u16_t>
//{
//   typedef boost::iterator_facade<u32_to_u16_iterator<BaseIterator, u16_t>, u16_t, std::bidirectional_iterator_tag, const u16_t> base_type;
//
//   typedef typename std::iterator_traits<BaseIterator>::value_type base_value_type;
//
//   BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
//   BOOST_STATIC_ASSERT(sizeof(u16_t)*CHAR_BIT == 16);
//
//public:
//   typename base_type::reference
//      dereference()const
//   {
//      if(m_current == 2)
//         extract_current();
//      return m_values[m_current];
//   }
//   bool equal(const u32_to_u16_iterator& that)const
//   {
//      if(m_position == that.m_position)
//      {
//         // Both m_currents must be equal, or both even
//         // this is the same as saying their sum must be even:
//         return (m_current + that.m_current) & 1u ? false : true;
//      }
//      return false;
//   }
//   void increment()
//   {
//      // if we have a pending read then read now, so that we know whether
//      // to skip a position, or move to a low-surrogate:
//      if(m_current == 2)
//      {
//         // pending read:
//         extract_current();
//      }
//      // move to the next surrogate position:
//      ++m_current;
//      // if we've reached the end skip a position:
//      if(m_values[m_current] == 0)
//      {
//         m_current = 2;
//         ++m_position;
//      }
//   }
//   void decrement()
//   {
//      if(m_current != 1)
//      {
//         // decrementing an iterator always leads to a valid position:
//         --m_position;
//         extract_current();
//         m_current = m_values[1] ? 1 : 0;
//      }
//      else
//      {
//         m_current = 0;
//      }
//   }
//   BaseIterator base()const
//   {
//      return m_position;
//   }
//   // construct:
//   u32_to_u16_iterator() : m_position(), m_current(0)
//   {
//      m_values[0] = 0;
//      m_values[1] = 0;
//      m_values[2] = 0;
//   }
//   u32_to_u16_iterator(BaseIterator b) : m_position(b), m_current(2)
//   {
//      m_values[0] = 0;
//      m_values[1] = 0;
//      m_values[2] = 0;
//   }
//private:
//
//   void extract_current()const
//   {
//      // begin by checking for a code point out of range:
//      ::boost::uint32_t v = *m_position;
//      if(v >= 0x10000u)
//      {
//         if(v > 0x10FFFFu)
//            detail::invalid_utf32_code_point(*m_position);
//         // split into two surrogates:
//         m_values[0] = static_cast<u16_t>(v >> 10) + detail::high_surrogate_base;
//         m_values[1] = static_cast<u16_t>(v & detail::ten_bit_mask) + detail::low_surrogate_base;
//         m_current = 0;
//         BOOST_ASSERT(detail::is_high_surrogate(m_values[0]));
//         BOOST_ASSERT(detail::is_low_surrogate(m_values[1]));
//      }
//      else
//      {
//         // 16-bit code point:
//         m_values[0] = static_cast<u16_t>(*m_position);
//         m_values[1] = 0;
//         m_current = 0;
//         // value must not be a surrogate:
//         if(detail::is_surrogate(m_values[0]))
//            detail::invalid_utf32_code_point(*m_position);
//      }
//   }
//   BaseIterator m_position;
//   mutable u16_t m_values[3];
//   mutable unsigned m_current;
//};

}

#endif  // BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP
