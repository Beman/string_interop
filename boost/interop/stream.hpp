//  boost/interop/stream.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STREAM_HPP)
# define BOOST_INTEROP_STREAM_HPP

#include <boost/interop/string.hpp>
#include <boost/interop/iterator_adapter.hpp>
#include <boost/interop/detail/is_iterator.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <ostream>
#include <iterator>

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
//typename boost::enable_if_c<::boost::xop::is_character_container<Ctr>::value
//    && !::boost::is_same<typename Ctr::value_type, typename Ostream::value_type>::value,
//  Ostream&>::type
//typename boost::enable_if<
//  boost::mpl::and_<boost::xop::is_character_container<Ctr>,
//                   boost::mpl::not_<
//    boost::is_same<typename Ctr::value_type, typename Ostream::value_type> > >,
//  Ostream&>::type

// This did work, except for the second and_ argument being a dummy
//typename boost::enable_if<
//  boost::mpl::and_<typename boost::xop::is_character_container<Ctr>,
//    boost::mpl::true_>,
//  Ostream&>::type

typename boost::enable_if<typename boost::xop::is_character_container<Ctr>,
  Ostream&>::type
operator<<(Ostream& os, const Ctr& ctr)
{
  boost::xop::converting_iterator<typename Ctr::const_iterator,
    typename Ctr::value_type, ::boost::xop::by_range,
    typename Ostream::char_type> itr(ctr.begin(), ctr.end());
  for (; *itr; ++itr)
    os << *itr;
  return os;
}

template <class Ostream, class InputIterator>
typename boost::enable_if<typename boost::is_iterator<InputIterator>,
  Ostream&>::type
operator<<(Ostream& os, InputIterator begin)
{
  cout << "InputIterator stream inserter\n";
  //converting_iterator<InputIterator,
  //  typename std::iterator_traits<InputIterator>::value_type, ::boost::xop::by_null,
  //    typename Ostream::char_type>
  //  itr(begin);
  //for (; *itr; ++itr)
  //  os << *itr;
  return os;
}

}  // namespace std

#endif  // BOOST_INTEROP_STREAM_HPP
