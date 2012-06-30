#include <boost/interop/detail/config.hpp>
#include <boost/interop/string_types.hpp>
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

namespace boost
{
namespace interop
{
namespace detail
{
  // for this proof-of-concept, simply linking in codec tables is sufficient
  extern const boost::u16_t  to_utf16[];  
  extern const unsigned char to_char[];
  extern const boost::uint8_t slice_index[];
}


//---------------------------  end-detection policy classes  ---------------------------//

//  from_iterator needs to know when the end of the sequence is reached. There are
//  several approaches to determine this; by half-open range, by size, and by
//  null-termination. End-detection policies allow the end to be efficiently determined,
//  and to do so without adding data members needed only for specific approaches.

struct by_null_policy {};
struct by_range_policy {};
struct by_size_policy {};

template <class InputIterator>
class by_null
{
public:
  typedef by_null_policy policy_type;
  bool is_end(InputIterator itr) const
  {
    return *itr ==
      static_cast<typename std::iterator_traits<InputIterator>::value_type>(0);
  }
  void advance(std::size_t sz=1) const {}
};

template <class InputIterator>
class by_range
{
  InputIterator m_end;
public:
  typedef by_range_policy policy_type;
  by_range() {}
  by_range(InputIterator itr) : m_end(itr) {}

  void end(InputIterator e) { m_end = e; }

  bool is_end(InputIterator itr) const { return itr == m_end; }
  void advance(std::size_t sz=1) const {}
};

template <class InputIterator>
class by_size
{
  mutable std::size_t m_size;
public:
  typedef by_size_policy policy_type;
  by_size() {}
  by_size(std::ptrdiff_t n) : m_size(n) {}

  void size(std::size_t sz) { m_size = sz; }

  bool is_end(InputIterator itr) const { return m_size <= 0; }
  void advance(std::size_t sz=1) const { m_size -= sz; }
};

//------------------------------------  codecs  ----------------------------------------//

struct native
{
  template <class CharT>
  struct codec { typedef native type; };

  template <class InputIterator, template<class> class EndPolicy>  
  class from_iterator
   : public boost::iterator_facade<from_iterator<InputIterator, EndPolicy>,
       u32_t, std::input_iterator_tag, const u32_t>,
     public EndPolicy<InputIterator>
  {
    typedef boost::iterator_facade<from_iterator<InputIterator, EndPolicy>,
      u32_t, std::input_iterator_tag, const u32_t> base_type;

    typedef typename std::iterator_traits<InputIterator>::value_type base_value_type;

    BOOST_STATIC_ASSERT(sizeof(base_value_type)*CHAR_BIT == 8);
    BOOST_STATIC_ASSERT(sizeof(u32_t)*CHAR_BIT == 32);

    // for the end itorator (i.e. m_is_end == true), value of m_iterator unspecified
    InputIterator  m_iterator;
    mutable bool   m_is_end;    // true iff this is the end iterator
    // note: InputIterator is not require to be default construtable [iterator.iterators],
    // so m_iterator == InputIterator() cannot be used to denote the end iterator.

  public:
    // construct:
    from_iterator() : m_is_end(true) {}  // construct end iterator

    // by_null
    from_iterator(InputIterator begin) : m_iterator(begin) 
    {
      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
        by_null_policy>::value, "Constructor not valid unless EndPolicy is by_null");
      m_is_end = this->is_end(m_iterator);
    }

    // by range
    template <class T>
    from_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void >::type* x=0)
      : m_iterator(begin) 
    {
      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
        by_range_policy>::value, "Constructor not valid unless EndPolicy is by_range");
      this->end(end);
      m_is_end = this->is_end(m_iterator);
    }

    // by_size
    from_iterator(InputIterator begin, std::size_t sz)
      : m_iterator(begin) 
    {
      static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
        by_size_policy>::value, "Constructor not valid unless EndPolicy is by_size");
      this->size(sz);
      m_is_end = this->is_end(m_iterator);
    }

    u32_t dereference() const
    {
      BOOST_ASSERT_MSG(!m_is_end,
        "Attempt to dereference end iterator");
      unsigned char c = static_cast<unsigned char>(*m_iterator);
      return static_cast<u32_t>(interop::detail::to_utf16[c]);
    }

    bool equal(const from_iterator& that) const
    {
      if (m_is_end)
        return that.m_is_end;
      if (that.m_is_end)
        return false;
      return m_iterator == that.m_iterator;
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(!m_is_end,
        "Attempt to increment end iterator");
      ++m_iterator;
      this->advance();
      if (this->is_end(m_iterator))
        m_is_end = true;
    }
  };

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
    to_iterator() : m_iterator() {}
    to_iterator(InputIterator begin) : m_iterator(begin) {}
    to_iterator(InputIterator begin, std::size_t sz) : m_iterator(begin, sz) {}

    template <class T>
     to_iterator(InputIterator begin, T end,
      // enable_if ensures 2nd argument of 0 is treated as size, not range end
      typename boost::enable_if<boost::is_same<InputIterator, T>, void >::type* x=0)
       : m_iterator(begin, end) {}

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
};  // native

struct utf8
{
  template <class CharT>
  struct codec { typedef utf8 type; };
};

struct utf16
{
  template <class CharT>
  struct codec { typedef utf16 type; };
};

struct utf32
{
  template <class CharT>
  struct codec { typedef utf32 type; };
};

namespace detail
{
  template <class CharT> struct auto_codec;
  template <> struct auto_codec<char> { typedef native type; };
}

struct auto_detect
{
  template <class CharT>
  struct codec
  { 
    typedef typename detail::auto_codec<CharT>::type type; 
  };

};

//------------------------------  conversion_iterator  ---------------------------------//

template <class ToCodec, class FromCodec,
  class InputIterator, template<class> class EndPolicy>
class conversion_iterator
  : public 
      ToCodec::template to_iterator<typename FromCodec::template from_iterator<InputIterator,
        EndPolicy> >
{
public:
  typedef typename FromCodec::template from_iterator<InputIterator, EndPolicy> from_iterator_type;
  typedef typename ToCodec::template to_iterator<from_iterator_type>           to_iterator_type;

  conversion_iterator() : to_iterator_type() {}

  conversion_iterator(InputIterator begin)
    : to_iterator_type(from_iterator_type(begin))
  {
    static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
      by_null_policy>::value, "Constructor not valid unless EndPolicy is by_null");
  }

  template <class U>
  conversion_iterator(InputIterator begin, U end,
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
    typename boost::enable_if<boost::is_same<InputIterator, U>, void >::type* x=0)
    : to_iterator_type(from_iterator_type(begin, end))
  {
    static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
      by_range_policy>::value,"Constructor not valid unless EndPolicy is by_range");
  }

  conversion_iterator(InputIterator begin, std::size_t sz)
    : to_iterator_type(from_iterator_type(begin, sz))
  {
    static_assert(is_same<typename EndPolicy<InputIterator>::policy_type,
      by_size_policy>::value,"Constructor not valid unless EndPolicy is by_size");
  }
};

//----------------------------------  convert  -----------------------------------------//

template <class ToCodec,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
          class FromCodec = auto_detect,
          class ToContainer = std::basic_string<typename ToCodec::value_type>,
# else
          class FromCodec,
          class ToContainer,
# endif
          class FromContainer>
ToContainer convert(const FromContainer& x)
{
  typedef conversion_iterator<
    ToCodec,
    typename FromCodec::template codec<typename FromContainer::value_type>::type,
    typename FromContainer::const_iterator, by_range>  iter_type;
  ToContainer tmp;
  iter_type itr(x.cbegin(), x.cend());
  for (; itr != iter_type(); ++itr)
    tmp.push_back(*itr);
  return tmp;
}

}  // namespace interop
}  // namespace boost
