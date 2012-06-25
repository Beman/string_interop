//  boost/interop/make_string.hpp  -----------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MAKE_STRING_HPP
#define BOOST_MAKE_STRING_HPP

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                Experimental string type and encoding conversion                      //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <boost/interop/codex_iterator.hpp>
#include <algorithm>

namespace boost
{
namespace interop
{

  //  container
  template <class ToContainer, class FromContainer>
  ToContainer convert(const FromContainer& x)
  {
    typedef boost::interop::codex_iterator<
      FromContainer::const_iterator,
      FromContainer::value_type, by_range, ToContainer::value_type>
       iter_type;
    ToContainer tmp;
    iter_type itr(x.cbegin(), x.cend());
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

  //  iterator, size
  template <class ToContainer, class InputIterator>
  ToContainer convert(InputIterator begin, std::size_t sz)
  {
    typedef boost::interop::codex_iterator<
      InputIterator,
      std::iterator_traits<InputIterator>::value_type, by_size, ToContainer::value_type>
       iter_type;
    ToContainer tmp;
    iter_type itr(begin, sz);
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

  //  TODO:
  //  * Two iterators
  //  * One iterator
  //  * Iterator, size

}  // namespace interop
}  // namespace boost

#endif  // BOOST_MAKE_STRING_HPP
