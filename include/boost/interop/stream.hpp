//  boost/interop/stream.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STREAM_HPP)
# define BOOST_INTEROP_STREAM_HPP

#include <boost/interop/string.hpp>
#include <boost/interop/iterator_adapter.hpp>
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
namespace xop
{
namespace detail
{

template <class Ostream, class InputIterator>
Ostream& inserter(Ostream& os, InputIterator begin)
{
  cout << "InputIterator stream inserter()\n";
  boost::xop::converting_iterator<InputIterator,
    typename std::iterator_traits<InputIterator>::value_type, ::boost::xop::by_null,
      typename Ostream::char_type>
    itr(begin);
  for (; *itr; ++itr)
    os << *itr;
  return os;
}

} // namespace detail
} // namespace xop
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

template <class Ostream, class Ctr>
typename boost::enable_if_c<boost::xop::is_character_container<Ctr>::value
    && !boost::is_same<typename Ctr::value_type, typename Ostream::char_type>::value,
  Ostream&>::type
operator<<(Ostream& os, const Ctr& ctr)
{
  //cout << boost::xop::is_character_container<Ctr>::value << ' '
  //     << boost::is_same<typename Ctr::value_type, typename Ostream::char_type>::value << ' ';
  cout << "Ctr\n"; 
  boost::xop::converting_iterator<typename Ctr::const_iterator,
    typename Ctr::value_type, boost::xop::by_range,
    typename Ostream::char_type> itr(ctr.begin(), ctr.end());
  for (; *itr; ++itr)
    os << *itr;
  return os;
}

//  Standard basic_ostream supplies this overload:
//
//    basic_ostream<charT,traits>& operator<<(const void* p);
//
//  That means pointers to types other than charT will select this overload rather than
//  a template <class Ostream, class InputIterator> overload.
//
//  As a fix, supply individual overloads for the ostreams and pointers we care about

basic_ostream<char>& operator<<(basic_ostream<char>& os, const wchar_t* p)
{
  return boost::xop::detail::inserter(os, p);
}

#ifndef BOOST_NO_CHAR16_T
basic_ostream<char>& operator<<(basic_ostream<char>& os, const char16_t* p)
{
  return boost::xop::detail::inserter(os, p);
}
#endif

#ifndef BOOST_NO_CHAR32_T
basic_ostream<char>& operator<<(basic_ostream<char>& os, const char32_t* p)
{
  return boost::xop::detail::inserter(os, p);
}
#endif

}  // namespace std

#endif  // BOOST_INTEROP_STREAM_HPP
