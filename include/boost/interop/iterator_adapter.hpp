//  boost/interop/conversion_iterator_adapters.hpp  ------------------------------------//

//  Copyright Beman Dawes 2011
//  Copyright (c) 2004 John Maddock

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//  John Maddock's boost/regex/pending/unicode_iterator.hpp introduced me to the idea   //
//  of performing conversion via iterator adapters. The code below for the UTF-8        //
//  to/from UTF-32 and UTF-16 <--> UTF-32 adapers was based on that header.             //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_INTEROP_ITERATOR_ADAPTER_HPP)
#define BOOST_INTEROP_ITERATOR_ADAPTER_HPP

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
namespace xop
{
namespace detail
{
  extern const boost::u16_t  to_utf16[];  
  extern const unsigned char to_char[];
  extern const boost::uint8_t slice_index[];
}

/*  TODO:

  *  by_null should do the character_traits eof dance

  *  John's code in <u8_t> from_iterator has been modified enough that it needs a
     complete test of its own.
     See Markus Kuhn's http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt as a
     possible test file.

*/

//--------------------------------------------------------------------------------------//

//  For n encodings, it is desirable to provide 2n rather than n squared adapters.
//  This is achieved conceptually by converting to and from UTF-32 as
//  an intermediate encoding, and then composing a converting_iterator from one
//  from_iterator and one to_iterator. For efficiency, specializations of
//  converting_iterator can provide direct conversion without an intermedate UTF32 step.

//--------------------------------------------------------------------------------------//

//---------------------------  end-detection policy classes  ---------------------------//

//  from_iterators need to know when the end of the sequence is reached. There are
//  several approaches to determine this; by half-open range, by size, and by
//  null-termination. End-detection policies allow the end to be efficiently determined,
//  and to do so without adding data members needed only for specific approaches.

template <class InputIterator>
class by_null
{
public:
  bool is_end(InputIterator itr) const
  {
    return *itr ==
      static_cast<typename std::iterator_traits<InputIterator>::value_type>(0);
  }
  void advance() const {}
};

template <class InputIterator>
class by_range
{
  InputIterator m_end;
public:
  by_range() {}
  by_range(InputIterator itr) : m_end(itr) {}

  void end(InputIterator e) { m_end = e; }

  bool is_end(InputIterator itr) const { return itr == m_end; }
  void advance() const {}
};

template <class InputIterator>
class by_size
{
  mutable std::size_t m_size;
public:
  by_size() {}
  by_size(std::size_t n) : m_size(n) {}

  void size(std::size_t sz) { m_size = sz; }

  bool is_end(InputIterator itr) const { return m_size == 0U; }
  void advance() const { --m_size; }
};

//---------------------------------  from_iterator  ------------------------------------//

  // primary template; partial specializations *must* be provided
  template <class InputIterator, class T, template<class> class EndPolicy>  
  class from_iterator; 

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u8_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u16_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u32_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, char, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, wchar_t, EndPolicy>;

//----------------------------------  to_iterator  -------------------------------------//
//
//  These iterators always identify the end by termination value

  // primary template; partial specializations *must* be provided
  template <class InputIterator, class T>  
  class to_iterator; 

  template <class InputIterator>
  class to_iterator<InputIterator, u8_t>;

  template <class InputIterator>
  class to_iterator<InputIterator, u16_t>;

  template <class InputIterator>
  class to_iterator<InputIterator, u32_t>;
  
  template <class InputIterator>
  class to_iterator<InputIterator, char>;

  template <class InputIterator>
  class to_iterator<InputIterator, wchar_t>;

//---------------------------------  policy_iterator  ----------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class policy_iterator;

//-------------------------------  converting_iterator  --------------------------------//

  // primary template; partial specializations *may* be provided
  template <class InputIterator, class From, template<class> class EndPolicy, class To>  
  class converting_iterator
    : public to_iterator<from_iterator<InputIterator, From, EndPolicy>, To>
  {
  public:
    explicit converting_iterator(InputIterator begin)
      : to_iterator<from_iterator<InputIterator, From, EndPolicy>, To>(begin)
    // Requires: An EndPolicy that requires no initialization
    {
      BOOST_XOP_LOG("converting_iterator primary template, by_null");
    }
    converting_iterator(InputIterator begin, InputIterator end)
      : to_iterator<from_iterator<InputIterator, From, EndPolicy>, To>(begin)
    // Requires: An EndPolicy that supplies end iterator initialization
    {
      BOOST_XOP_LOG("converting_iterator primary template, by range");
      this->base().end(end);
    }
    converting_iterator(InputIterator begin, std::size_t sz)
      : to_iterator<from_iterator<InputIterator, From, EndPolicy>, To>(begin)
    // Requires: An EndPolicy that supplies size initialization
    {
      BOOST_XOP_LOG("converting_iterator primary template, by size");
      this->base().size(sz);
    }
  };

  ////  case of From is u32_t
  //template <class InputIterator, class To, template<class> class EndPolicy>
  //class converting_iterator<InputIterator, u32_t, EndPolicy, To>                
  //  : public to_iterator<policy_iterator<InputIterator, EndPolicy>, To>
  //{
  //public:
  //  explicit converting_iterator(InputIterator begin)
  //    : to_iterator<policy_iterator<InputIterator, EndPolicy>, To>(begin)
  //  // Requires: An EndPolicy that requires no initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; From is u32_t, by_null");
  //  }
  //  converting_iterator(InputIterator begin, InputIterator end)
  //    : to_iterator<policy_iterator<InputIterator, EndPolicy>, To>(begin)
  //  // Requires: An EndPolicy that supplies end iterator initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; From is u32_t, by range");
  //    base().end(end);
  //  }
  //  converting_iterator(InputIterator begin, std::size_t sz)
  //    : to_iterator<policy_iterator<InputIterator, EndPolicy>, To>(begin)
  //  // Requires: An EndPolicy that supplies size initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterato; From is u32_t, by size");
  //    base().size(sz);
  //  }
  //};

  ////  case of To is u32_t
  //template <class InputIterator, class From, template<class> class EndPolicy>
  //class converting_iterator<InputIterator, From, EndPolicy, u32_t>                
  //  : public from_iterator<InputIterator, From, EndPolicy>
  //{
  //public:
  //  explicit converting_iterator(InputIterator begin)
  //    : from_iterator<InputIterator,From,EndPolicy>(begin)
  //  // Requires: An EndPolicy that requires no initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; To is u32_t, by null");
  //  }
  //  converting_iterator(InputIterator begin, InputIterator end)
  //    : from_iterator<InputIterator,From,EndPolicy>(begin)
  //  // Requires: An EndPolicy that supplies end iterator initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; To is u32_t, by range");
  //    this->end(end);
  //  }
  //  converting_iterator(InputIterator begin, std::size_t sz)
  //    : from_iterator<InputIterator,From,EndPolicy>(begin)
  //  // Requires: An EndPolicy that supplies size initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; To is u32_t, by size");
  //    this->size(sz);
  //  }
  //};

  ////  case of value_type/To/From are all the same type 
  //template <class InputIterator, template<class> class EndPolicy>
  //class converting_iterator<InputIterator,
  //  typename std::iterator_traits<InputIterator>::value_type, EndPolicy,
  //  typename std::iterator_traits<InputIterator>::value_type>                
  //  : public policy_iterator<InputIterator, EndPolicy>
  //{
  //public:
  //  explicit converting_iterator(InputIterator begin)
  //    : policy_iterator<InputIterator,EndPolicy>(begin)
  //  // Requires: An EndPolicy that requires no initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; value_type/To/From the same, by null");
  //  }
  //  converting_iterator(InputIterator begin, InputIterator end)
  //    : policy_iterator<InputIterator,EndPolicy>(begin)
  //  // Requires: An EndPolicy that supplies end iterator initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; value_type/To/From the same, by range");
  //    this->end(end);
  //  }
  //  converting_iterator(InputIterator begin, std::size_t sz)
  //    : policy_iterator<InputIterator,EndPolicy>(begin)
  //  // Requires: An EndPolicy that supplies size initialization
  //  {
  //    BOOST_XOP_LOG("converting_iterator; value_type/To/From the same, by size");
  //    this->size(sz);
  //  }
  //};

//--------------------------------------------------------------------------------------//
//                                  implementation                                      
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

//------------------------------  <u8_t> from_iterator  --------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u8_t, EndPolicy>
   : public boost::iterator_facade<from_iterator<InputIterator, u8_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t>,
     public EndPolicy<InputIterator>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator, u8_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(u32_t, pending_read = 0xffffffffu);

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

     // UTF-8 is a multi-character encoding, so invariants are a bit different than for
     // single character encodings:
     mutable InputIterator  m_position;    // points to the next input character
     mutable u32_t          m_value;       // the current value, which may be read_pending

  public:
     typename base_type::reference
        dereference() const
     {
        if(m_value == pending_read)
           extract_current();
        return m_value;
     }
     bool equal(const from_iterator& that)const
     {
        return m_position == that.m_position;
     }
     void increment()
     {
        BOOST_ASSERT_MSG(m_value, "Attempt to increment past the end");
        m_value = pending_read;
     }

     // construct:
     from_iterator() : m_position()
     {
        m_value = pending_read;
     }
     from_iterator(InputIterator b) : m_position(b)
     {
        m_value = pending_read;
        BOOST_XOP_LOG("utf-8 to utf-32");
     }
  private:
     static void invalid_sequence()
     {
        std::out_of_range e("Invalid UTF-8 sequence encountered while trying to encode UTF-32 character");
        BOOST_INTEROP_THROW(e);
     }
     void extract_current() const
     {
        if (this->is_end(m_position))
        {
          m_value = 0U;
          return;
        }
        m_value = static_cast<u32_t>(static_cast< ::boost::uint8_t>(*m_position));
        // we must not have a continuation character:
        if((m_value & 0xC0u) == 0x80u)
           invalid_sequence();
        // see how many extra byts we have:
        unsigned extra = detail::utf8_trailing_byte_count(*m_position);
        // extract the extra bits, 6 from each extra byte:
        for(unsigned c = 0; c < extra; ++c)
        {
           ++m_position;
           this->advance();
           if (this->is_end(m_position))
             invalid_sequence();
           m_value <<= 6;
           m_value += static_cast<boost::uint8_t>(*m_position) & 0x3Fu;
        }
        ++m_position;  // satisfy the invariant that m_position
        this->advance();     //   points to next input character
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
  };

//------------------------------  <u16_t> from_iterator  -------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u16_t, EndPolicy>
   : public boost::iterator_facade<from_iterator<InputIterator, u16_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t>,
     public EndPolicy<InputIterator>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator, u16_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t> base_type;
     // special values for pending iterator reads:
     BOOST_STATIC_CONSTANT(u32_t, pending_read = 0xffffffffu);

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

     // UTF-16 is a multi-character encoding, so invariants are a bit different than for
     // single character encodings:
     mutable InputIterator  m_position;  // points to first byte of next input character
     mutable u32_t          m_value;     // the current value, which may be read_pending

  public:
     typename base_type::reference
     dereference()const
     {
        if(m_value == pending_read)
           extract_current();
        return m_value;
     }
     bool equal(const from_iterator& that)const
     {
        return m_position == that.m_position;
     }
     void increment()
     {
        BOOST_ASSERT_MSG(m_value, "Attempt to increment past the end");
        m_value = pending_read;
     }

     // construct:
     from_iterator() : m_position()
     {
        m_value = pending_read;
     }
     from_iterator(InputIterator b) : m_position(b)
     {
        m_value = pending_read;
        BOOST_XOP_LOG("utf-16 to utf-32");
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
        std::out_of_range e("Invalid UTF-16 sequence encountered while trying to encode UTF-32 character");
        BOOST_INTEROP_THROW(e);
     }
     void extract_current()const
     {
        if (this->is_end(m_position))
        {
          m_value = 0U;
          return;
        }
        m_value = static_cast<u32_t>(static_cast< ::boost::uint16_t>(*m_position));
        // if the last value is a high surrogate then adjust m_position and m_value as needed:
        if(detail::is_high_surrogate(*m_position))
        {
           // precondition; next value must have be a low-surrogate:
           ++m_position;
           this->advance();
           if (this->is_end(m_position))
             invalid_sequence();
           ::boost::uint16_t t = *m_position;
           if((t & 0xFC00u) != 0xDC00u)
              invalid_code_point(t);
           m_value = (m_value - detail::high_surrogate_base) << 10;
           m_value |= (static_cast<u32_t>(static_cast< ::boost::uint16_t>(t))
             & detail::ten_bit_mask);
        }
        ++m_position;  // satisfy the invariant that m_position
        this->advance();     //   points to next input character

        // postcondition; result must not be a surrogate:
        if(detail::is_surrogate(m_value))
           invalid_code_point(static_cast< ::boost::uint16_t>(m_value));
     }
  };

//-------------------------------  <u32_t> from_iterator  ------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u32_t, EndPolicy>
    : public boost::iterator_facade<from_iterator<InputIterator, u32_t, EndPolicy>,
        u32_t, std::input_iterator_tag, const u32_t>,
      public EndPolicy<InputIterator>

  {
    InputIterator m_itr;
  public:
    from_iterator() {}
    from_iterator(InputIterator itr) : m_itr(itr)
    {
        BOOST_XOP_LOG("utf-32 to utf-32");
    }
    u32_t dereference() const
    {
      if (this->is_end(m_itr))
        return 0;
      return *m_itr;
    }
    bool equal(const from_iterator& that) const {return m_itr == that.m_itr;}
    void increment()
    {
      BOOST_ASSERT_MSG(!this->is_end(m_itr), "Attempt to increment past end");
      ++m_itr;
      this->advance();
    }
  };

//-------------------------------  <u8_t> to_iterator  ---------------------------------//

  template <class InputIterator>
  class to_iterator<InputIterator, u8_t>
   : public boost::iterator_facade<to_iterator<InputIterator, u8_t>,
       u8_t, std::input_iterator_tag, const u8_t>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator, u8_t>,
       u8_t, std::input_iterator_tag, const u8_t> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(u8_t)*CHAR_BIT == 8);

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
        if(m_position == that.m_position)
        {
           // either the m_current's must be equal, or one must be 0 and 
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
           ++m_position;
        }
     }

     InputIterator& base() { return m_position; }

     // construct:
     to_iterator() : m_position(), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
     }
     to_iterator(InputIterator b) : m_position(b), m_current(4)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
        m_values[4] = 0;
        BOOST_XOP_LOG("utf-8 from utf-32");
    }
  private:

     void extract_current()const
     {
        boost::uint32_t c = *m_position;
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
     InputIterator m_position;
     mutable u8_t m_values[5];
     mutable unsigned m_current;
  };

//-------------------------------  <u16_t> to_iterator  --------------------------------//

  template <class InputIterator>
  class to_iterator<InputIterator, u16_t>
   : public boost::iterator_facade<to_iterator<InputIterator, u16_t>,
      u16_t, std::input_iterator_tag, const u16_t>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator, u16_t>,
       u16_t, std::input_iterator_tag, const u16_t> base_type;

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(u16_t)*CHAR_BIT == 16);

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
        if(m_position == that.m_position)
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
           ++m_position;
        }
     }

     InputIterator& base() {return m_position;}

     // construct:
     to_iterator() : m_position(), m_current(0)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
     }
     to_iterator(InputIterator b) : m_position(b), m_current(2)
     {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        BOOST_XOP_LOG("utf-16 from utf-32");
    }
  private:

     void extract_current()const
     {
        // begin by checking for a code point out of range:
        ::boost::uint32_t v = *m_position;
        if(v >= 0x10000u)
        {
           if(v > 0x10FFFFu)
              detail::invalid_utf32_code_point(*m_position);
           // split into two surrogates:
           m_values[0] = static_cast<u16_t>(v >> 10) + detail::high_surrogate_base;
           m_values[1] = static_cast<u16_t>(v & detail::ten_bit_mask) + detail::low_surrogate_base;
           m_current = 0;
           BOOST_ASSERT(detail::is_high_surrogate(m_values[0]));
           BOOST_ASSERT(detail::is_low_surrogate(m_values[1]));
        }
        else
        {
           // 16-bit code point:
           m_values[0] = static_cast<u16_t>(*m_position);
           m_values[1] = 0;
           m_current = 0;
           // value must not be a surrogate:
           if(detail::is_surrogate(m_values[0]))
              detail::invalid_utf32_code_point(*m_position);
        }
     }
     InputIterator m_position;
     mutable u16_t m_values[3];
     mutable unsigned m_current;
  };

//-------------------------------  <u32_t> to_iterator  --------------------------------//

  template <class InputIterator>
  class to_iterator<InputIterator, u32_t>
   : public boost::iterator_facade<to_iterator<InputIterator, u32_t>,
      u32_t, std::input_iterator_tag, const u32_t>
  {
    InputIterator m_itr;
  public:
    to_iterator() {}
    to_iterator(InputIterator itr) : m_itr(itr)
    {
      BOOST_XOP_LOG("utf-32 from utf-32");
    }
    u32_t dereference() const { return *m_itr; }
    bool equal(const to_iterator& that) const {return m_itr == that.m_itr;}
    void increment() { ++m_itr; }
    InputIterator& base() {return m_itr;}
  };

//  /***************************************************************************************
// 
//  char and wchar_t codecs need to know the encoding of char and wchar_t strings.
//
//  They need to know how errors are to be reported, such as by exception or by
//  replacement character.
//
//  Since it is totally ineffcient to write separate codecs for generic cases, they need to
//  know a codec sub-type, such as single character, NTBS, etc, and what table to pass to 
//  that subtype.
//
//  Presumably that information comes from global state information. Details to be worked out.
//
//  For this proof-of-concept, the code just uses the tables linked in. The error
//  action for UTF-32 is replacement character '�' (U+FFFD), and for char it is the '?'
//  character U+003F
//
//  **********
//    
//  What happens when a character not in the current T character set is
//  encountered? One possibility to to translate to a replacement character.
//  One of the ASCII characters not normally encountered might be a good choice.
//  The codes below 0x20 or 0x7f (DEL) might be considered. The Wikipedia UTF-8
//  article suggests these are popular:
//   * The replacement character '�' (U+FFFD)
//   * The symbol for substitute '␦' (U+2426) (ISO 2047)
//   * The '?' or '¿' character (U+003F or U+00BF)
//   * The invalid Unicode code points U+DC80..U+DCFF where the low 8 bits are the
//     byte's value.
//   * Interpret the bytes according to another encoding (often ISO-8859-1 or CP1252).
//
//  *************************************************************************************/

//-------------------------------  <char> from_iterator  -------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, char, EndPolicy>
   : public boost::iterator_facade<from_iterator<InputIterator, char, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t>,
     public EndPolicy<InputIterator>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator, char, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t> base_type;

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

     InputIterator m_iterator;

  public:
     u32_t dereference() const
     {
       if (this->is_end(m_iterator))
         return 0;
       unsigned char c = static_cast<unsigned char>(*m_iterator);
       return static_cast<u32_t>(xop::detail::to_utf16[c]);
     }
     bool equal(const from_iterator& that) const
     {
       return m_iterator == that.m_iterator;
     }
     void increment()
     { 
       BOOST_ASSERT_MSG(!this->is_end(m_iterator), "Attempt to increment past end");
       ++m_iterator;
       this->advance();
     }

     // construct:
     from_iterator() : m_iterator() {}
     from_iterator(InputIterator b) : m_iterator(b)
     {
       BOOST_XOP_LOG("char to utf-32");
     }
  };

//-------------------------------  <char> to_iterator  ---------------------------------//

#if defined(BOOST_WINDOWS_API)

  template <class InputIterator>
  class to_iterator<InputIterator, char>
   : public boost::iterator_facade<to_iterator<InputIterator, char>,
       char, std::input_iterator_tag, const char>
  {
     typedef boost::iterator_facade<to_iterator<InputIterator, char>,
       char, std::input_iterator_tag, const char> base_type;
   
     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 32);
     BOOST_STATIC_ASSERT(sizeof(char)*CHAR_BIT == 8);

     InputIterator m_iterator;

  public:
     char dereference() const
     {
       u32_t c = *m_iterator;
       //cout << "*** c is " << hex << c << '\n';
       //cout << "    to_slice[c >> 7] << 7 is "
       //  << unsigned int(xop::detail::slice_index[c >> 7] << 7) << '\n';
       return static_cast<char>(xop::detail::to_char
         [
           (xop::detail::slice_index[c >> 7] << 7) | (c & 0x7f)
         ]);
     }

     bool equal(const to_iterator& that) const
     {
       return m_iterator == that.m_iterator;
     }

     void increment()  { ++m_iterator; }

     InputIterator& base() {return m_iterator;}

     // construct:
     to_iterator() : m_iterator() {}
     to_iterator(InputIterator b) : m_iterator(b)
     {
       BOOST_XOP_LOG("char from utf-32");
     }
  };

 # elif defined(BOOST_POSIX_API)  // POSIX; assumes char is UTF-8

  template <class InputIterator>
  class to_iterator<InputIterator, char>
   : public boost::iterator_facade<to_iterator<InputIterator, char>,
       char, std::input_iterator_tag, const char>
  {
     to_iterator<InputIterator, u8_t>  m_iterator;

  public:
     char dereference() const
     {
       return static_cast<char>(*m_iterator);
     }

     bool equal(const to_iterator& that) const
     {
       return m_iterator == that.m_iterator;
     }

     void increment()  { ++m_iterator; }

     InputIterator& base() {return m_iterator;}

     // construct:
     to_iterator() : m_iterator() {}
     to_iterator(InputIterator b) : m_iterator(b)
     {
        BOOST_XOP_LOG("char from utf-32");
     }
  };

# else
#   error Sorry, not implemented for other than 16 or 32 bit wchar_t
# endif

//-----------------------------  <wchar_t> from_iterator  ------------------------------//

#if defined(BOOST_WINDOWS_API)  // assume wchar_t is UTF-16

namespace detail
{
  template <class InputIterator>
  class wide_to_u16
    : public boost::iterator_facade<wide_to_u16<InputIterator>,
       u16_t, std::input_iterator_tag, const u16_t>
  {
    InputIterator m_itr;
  public:
    wide_to_u16() {}
    explicit wide_to_u16(InputIterator itr) : m_itr(itr) {}
    u16_t dereference() const {return u16_t(*m_itr);}
    bool equal(const wide_to_u16& that) const {return m_itr == that.m_itr;}
    void increment() {++m_itr;}
  };
}
  BOOST_STATIC_ASSERT(sizeof(wchar_t)*CHAR_BIT == 16);

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, wchar_t, EndPolicy>
   : public boost::iterator_facade<from_iterator<InputIterator, wchar_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t>
  {
     typedef boost::iterator_facade<from_iterator<InputIterator, wchar_t, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t> base_type;

     typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

     BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 16);
     BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

     from_iterator<detail::wide_to_u16<InputIterator>, u16_t, EndPolicy> m_iterator;

  public:
     from_iterator() : m_iterator() {}
     from_iterator(InputIterator b) : m_iterator(detail::wide_to_u16<InputIterator>(b))
     {
       BOOST_XOP_LOG("wchar_t to utf-32");
     }
     u32_t dereference() const { return *m_iterator; }
     bool equal(const from_iterator& that) const {return m_iterator == that.m_iterator;}
     void increment() { ++m_iterator; }
     void size(std::size_t sz) { m_iterator.size(sz); }
     void end(InputIterator e) { m_iterator.end(detail::wide_to_u16<InputIterator>(e)); }
  };

# elif defined(BOOST_POSIX_API)  // POSIX; assumes wchar_t is UTF-32

  BOOST_STATIC_ASSERT(sizeof(wchar_t)*CHAR_BIT == 32);
  //...
# else
#   error Sorry, not implemented for other than 16 or 32 bit wchar_t
# endif

//------------------------------  <wchar_t> to_iterator  -------------------------------//

#if defined(BOOST_WINDOWS_API)  // assume wchar_t is UTF-16

  BOOST_STATIC_ASSERT(sizeof(wchar_t)*CHAR_BIT == 16);

  template <class InputIterator>
  class to_iterator<InputIterator, wchar_t>
   : public boost::iterator_facade<to_iterator<InputIterator, wchar_t>,
       wchar_t, std::input_iterator_tag, const wchar_t>
  {
     to_iterator<InputIterator, u16_t>  m_iterator;

  public:
     wchar_t dereference() const
     {
       return static_cast<wchar_t>(*m_iterator);
     }

     bool equal(const to_iterator& that) const
     {
       return m_iterator == that.m_iterator;
     }

     void increment()  { ++m_iterator; }

     InputIterator& base() {return m_iterator.base();}

     // construct:
     to_iterator() : m_iterator() {}
     to_iterator(InputIterator b) : m_iterator(b)
     {
       BOOST_XOP_LOG("wchar_t from utf-32");
     }
  };

# elif defined(BOOST_POSIX_API)  // POSIX; assumes wchar_t is UTF-32

  BOOST_STATIC_ASSERT(sizeof(wchar_t)*CHAR_BIT == 32);
  //...
# else
#   error Sorry, not implemented for other than 16 or 32 bit wchar_t
# endif

//---------------------------------  policy_iterator  ----------------------------------//

  template <class InputIterator, template<class> class EndPolicy>
  class policy_iterator
    : public boost::iterator_facade<policy_iterator<InputIterator, EndPolicy>,
        typename std::iterator_traits<InputIterator>::value_type,
        std::input_iterator_tag,
        const typename std::iterator_traits<InputIterator>::value_type>,
      public EndPolicy<InputIterator>

  {
    InputIterator m_itr;
  public:
    policy_iterator() {}
    explicit policy_iterator(InputIterator itr) : m_itr(itr) {}
    typename std::iterator_traits<InputIterator>::value_type dereference() const
    {
      if (this->is_end(m_itr))
        return 0;
      return *m_itr;
    }
    bool equal(const policy_iterator& that) const {return m_itr == that.m_itr;}
    void increment()
    {
      BOOST_ASSERT_MSG(!this->is_end(m_itr), "Attempt to increment past end");
      ++m_itr;
      this->advance();
    }
  };

}  // namespace xop

}  // namespace boost

#endif  // BOOST_INTEROP_ITERATOR_ADAPTER_HPP
