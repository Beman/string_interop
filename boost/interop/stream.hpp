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
#include <ostream>

namespace boost
{
namespace xop  // short for interoperability
{

//--------------------------------------------------------------------------------------//
//                               stream inserters
//--------------------------------------------------------------------------------------//

template <class Ostream, class Ctr>
typename boost::enable_if<is_character_container<Ctr>,
  Ostream&>::type
operator<<(Ostream& os, const Ctr& ctr)
{
  converting_iterator<typename Ctr::const_iterator,
    typename Ctr::value_type, by_range,
    typename Ostream::char_type> itr(ctr.begin(), ctr.end());
  for (; *itr; ++itr)
    os << *itr;
  return os;
}

//
//template <class Ostream, class InputIterator>
//typename boost::enable_if<::boost::is_iterator<InputIterator>,
//  Ostream&>::type
//operator<<(Ostream& os, InputIterator begin)
//{
//  cout << "InputIterator stream inserter\n";
//  converting_iterator<InputIterator,
//    typename std::iterator_traits<InputIterator>::value_type, by_null,
//      typename Ostream::char_type>
//    itr(begin);
//  for (; *itr; ++itr)
//    os << *itr;
//  return os;
//}

}  // namespace xop
}  // namespace boost

#endif  // BOOST_INTEROP_STREAM_HPP
