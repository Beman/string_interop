//  boost/string_interop/ntcts_iterator.hpp  --------------------------------------------------//

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
  namespace string_interop
  {

    //  FAQ
    //
    //  Q: Why isn't ntcts_iterator a bidirectional iterator?
    //  A: Because it isn't possible to meet the [bidrectional.iterators]
    //     requirement for --r in table 110.
    //  Q: Why isn't ntcts_iterator a random access iterator?
    //  A: Because it isn't possible to meet the bidirectional requirement for
    //     random access iterators and it isn't possible to compute the distance between
    //     two ntcts_iterators if one of them is the end iterator,  and that violates
    //     [random.access.iterators] requirement for b - a in table 111.

    template <class charT>
    class ntcts_iterator
      : public boost::iterator_facade<ntcts_iterator<charT>,
      charT, std::forward_iterator_tag>
    {
    public:

      ntcts_iterator() BOOST_NOEXCEPT : m_p(0) {}  // construct end iterator

      ntcts_iterator(charT* begin) : m_p(begin)
      {
        BOOST_ASSERT_MSG(begin, "attempt to construct ntcts_iterator with null pointer");
        if (*m_p == charT())
          m_p = 0;
      }

    private:
      friend class boost::iterator_core_access;

      charT* m_p;  // 0 for the end iterator

      typename boost::iterator_facade<boost::string_interop::ntcts_iterator<charT>,
        charT, std::forward_iterator_tag>::
        reference dereference() const BOOST_NOEXCEPT
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

    };

    typedef ntcts_iterator<const char> char_iterator;
    typedef ntcts_iterator<const wchar_t> wchar_iterator;

    template <class charT>
    ntcts_iterator<charT> begin(ntcts_iterator<charT> it)
    {
      return it;
    }
    template <class charT>
    ntcts_iterator<charT> end(ntcts_iterator<charT> it)
    {
      return ntcts_iterator<charT>();
    }

  }  // namespace string_interop
}  // namespace boost

//--------------------------------------------------------------------------------------//

#endif  // BOOST_NTCTS_ITERATOR_HPP
