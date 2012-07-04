//  boost/interop/stream.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STREAM_HPP)
# define BOOST_INTEROP_STREAM_HPP

#include <boost/interop/string_types.hpp>
#include <boost/interop/convert.hpp>
#include <boost/interop/detail/is_iterator.hpp>
#include <boost/interop/detail/iterator_value.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <ostream>
#include <iterator>

namespace boost
{
namespace interop
{
namespace detail
{

template <class Ostream, class InputIterator>
Ostream& inserter(Ostream& os, InputIterator begin)
{
  typedef boost::interop::conversion_iterator<
    typename boost::interop::auto_codec<typename Ostream::char_type>::type,
    typename boost::interop::auto_codec<
      typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;

  for (iter_type itr(begin); itr != iter_type(); ++itr)
    os << *itr;
  return os;
}

} // namespace detail
} // namespace interop
} // namespace boost


//--------------------------------------------------------------------------------------//
//                                                                                      //
//                         stream inserters and extractors                              //
//                                                                                      //
//     These need to be found by argument dependent lookup so go in namespace std       //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace std
{

//  basic_string overload

template <class Ostream, class charT, class Traits, class Allocator>
typename boost::enable_if_c<!boost::is_same<charT, typename Ostream::char_type>::value,
  Ostream&>::type
operator<<(Ostream& os, const basic_string<charT, Traits, Allocator>& str)
{
  typedef const basic_string<charT, Traits, Allocator> string_type;

  typedef boost::interop::conversion_iterator<
    typename boost::interop::auto_codec<typename Ostream::char_type>::type,
    typename boost::interop::auto_codec<charT>::type,
    typename string_type::const_iterator>
      iter_type;

  iter_type itr(str.cbegin(), str.cend());
  for (; itr != iter_type(); ++itr)
    os << *itr;
  return os;
}

//  Character pointer overloads
//
//  Standard basic_ostream supplies this overload:
//
//    basic_ostream<charT,traits>& operator<<(const void* p);
//
//  That means pointers to types other than charT will select the void* overload rather
//  than a template <class Ostream, class InputIterator> overload.
//
//  As a fix, supply individual overloads for the ostreams and pointers we care about

//basic_ostream<char>& operator<<(basic_ostream<char>& os, const wchar_t* p)
//{
//  return boost::interop::detail::inserter(os, p);
//}

#ifndef BOOST_NO_CHAR16_T
basic_ostream<char>& operator<<(basic_ostream<char>& os, const char16_t* p)
{
  return boost::interop::detail::inserter(os, p);
}
#endif

#ifndef BOOST_NO_CHAR32_T
basic_ostream<char>& operator<<(basic_ostream<char>& os, const char32_t* p)
{
  return boost::interop::detail::inserter(os, p);
}
#endif

}  // namespace std

#endif  // BOOST_INTEROP_STREAM_HPP
