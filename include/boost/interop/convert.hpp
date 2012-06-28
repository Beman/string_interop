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

#include <boost/interop/conversion_iterator.hpp>
#include <boost/interop/detail/is_iterator.hpp>

namespace boost
{
namespace interop
{

  //  container
  template <class ToContainer,
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
    template <class, class> class ToIterator = to_iterator,
    template <class, class,
      template<class> class> class FromIterator = from_iterator,
# endif
    class FromContainer>
    // enable_if resolves ambiguity with single iterator overload
  typename boost::disable_if<boost::is_iterator<FromContainer>,
  ToContainer>::type convert(const FromContainer& x)
  {
    typedef boost::interop::conversion_iterator<
      typename FromContainer::const_iterator, typename FromContainer::value_type, by_range,
      typename ToContainer::value_type
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
      , ToIterator, FromIterator 
# endif
      >
        iter_type;
    ToContainer tmp;
    iter_type itr(x.cbegin(), x.cend());
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

  //  null terminated iterator
  template <class ToContainer, class InputIterator>
    // enable_if resolves ambiguity with FromContainer overload
  typename boost::enable_if<boost::is_iterator<InputIterator>,
  ToContainer>::type convert(InputIterator begin)
  {
    typedef boost::interop::conversion_iterator<
      InputIterator,
      typename std::iterator_traits<InputIterator>::value_type, by_null,
      typename ToContainer::value_type>
        iter_type;
    ToContainer tmp;
    iter_type itr(begin);
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

  //  iterator, size
  template <class ToContainer, class InputIterator>
  ToContainer convert(InputIterator begin, std::size_t sz)
  {
    typedef boost::interop::conversion_iterator<
      InputIterator,
      typename std::iterator_traits<InputIterator>::value_type, by_size,
      typename ToContainer::value_type>
        iter_type;
    ToContainer tmp;
    iter_type itr(begin, sz);
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

  //  iterator range
  template <class ToContainer, class InputIterator, class InputIterator2>
    // enable_if ensures 2nd argument of 0 is treated as size, not range end
  typename boost::enable_if<boost::is_iterator<InputIterator2>,
  ToContainer>::type convert(InputIterator begin, InputIterator2 end)
  {
    typedef boost::interop::conversion_iterator<
      InputIterator,
      typename std::iterator_traits<InputIterator>::value_type, by_range,
      typename ToContainer::value_type>
        iter_type;
    ToContainer tmp;
    iter_type itr(begin, end);
    for (; itr != iter_type(); ++itr)
      tmp.push_back(*itr);
    return tmp;
  }

}  // namespace interop
}  // namespace boost

#endif  // BOOST_MAKE_STRING_HPP
