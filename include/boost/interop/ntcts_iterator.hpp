//  boost/interop/ntcts_iterator.hpp  --------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//----------  null terminated character type string (NTCTS) iterator adapter  ----------//

#if !defined(BOOST_NTCTS_ITERATOR_HPP)
#define BOOST_NTCTS_ITERATOR_HPP

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

namespace boost
{
namespace interop
{

  //  TODO:
  //
  //  * test non-const (char*) source

  //  Q: Why isn't ntcts_iterator a random access iterator?
  //  A: Because it isn't possible to compute the distance between two ntcts_iterators
  //     if one of them is the end iterator, and that violates [random.access.iterators]
  //     requirement for b - a in table 111.

  template <class charT>
  class ntcts_iterator
    : public boost::iterator_facade<ntcts_iterator<charT>,
        charT, std::bidirectional_iterator_tag, const charT> 
  {
    const charT* m_p;  // 0 for the end iterator
  public:

    ntcts_iterator() BOOST_NOEXCEPT : m_p(0) {}  // construct end iterator

    ntcts_iterator(const char* begin) : m_p(begin)
    {
      BOOST_ASSERT_MSG(begin, "attempt to construct ntcts_iterator with null pointer"); 
      if (*m_p == charT())
        m_p = 0;
    }

    charT dereference() const BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_p, "attempt to dereference end ntcts_iterator"); 
      return *m_p;
    }

    bool equal(ntcts_iterator rhs) const BOOST_NOEXCEPT
    {
      return m_p == rhs.m_p || (m_p && rhs.m_p && *m_p == *rhs.m_p);
    }

    void increment() BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_p, "attempt to increment end ntcts_iterator"); 
      if (*++m_p == charT())
        m_p = 0;
    }

    void decrement() BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_p, "attempt to decrement end ntcts_iterator"); 
        --m_p;
    }

  };

  typedef ntcts_iterator<char> char_iterator;
  typedef ntcts_iterator<wchar_t> wchar_iterator;

}  // namespace interop
}  // namespace boost

//--------------------------------------------------------------------------------------//

#endif  // BOOST_NTCTS_ITERATOR_HPP
