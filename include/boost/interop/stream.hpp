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
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <ostream>
#include <iterator>



namespace boost
{
  namespace detail
  {
    template <class T, bool Pred> struct iterator_value_helper;
    template <class T> struct iterator_value_helper<T, true>
    { 
      typedef typename std::iterator_traits<T>::value_type type;
    };
    struct not_an_iterator;
    template <class T> struct iterator_value_helper<T, false>
    { 
      typedef not_an_iterator type;
    };

    template <class T>
    struct iterator_value
    {
      typedef
        typename iterator_value_helper<T, is_iterator<T>::value >::type type;
    };

  } // namespace detail
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
  cout << boost::xop::is_character_container<Ctr>::value << ' '
       << boost::is_same<typename Ctr::value_type, typename Ostream::char_type>::value << ' ';
  cout << "Ctr\n"; 
  boost::xop::converting_iterator<typename Ctr::const_iterator,
    typename Ctr::value_type, boost::xop::by_range,
    typename Ostream::char_type> itr(ctr.begin(), ctr.end());
  for (; *itr; ++itr)
    os << *itr;
  return os;
}

//template <class Ostream, class InputIterator>
//
////typename boost::enable_if<typename boost::is_iterator<InputIterator>,
//
//// worked, but forcing char
//typename boost::enable_if<
//  boost::mpl::and_<
//    typename boost::is_iterator<InputIterator>,
//    boost::mpl::not_<boost::is_same< typename Ostream::value_type,
//                                     char
//                                   > >
//  >,
//
////typename boost::enable_if<
////  boost::mpl::and_<
////    typename boost::is_iterator<InputIterator>,
////    boost::mpl::not_<boost::is_same<
////      typename Ostream::value_type,
////      typename boost::detail::iterator_value<InputIterator>::type
////          > // is_same
////        > // not
////      > // and
////    ,
//Ostream& >::type // enable_if
//operator<<(Ostream& os, InputIterator begin)
//{
//  cout << "InputIterator stream inserter\n";
//  boost::xop::converting_iterator<InputIterator,
//    typename std::iterator_traits<InputIterator>::value_type, ::boost::xop::by_null,
//      typename Ostream::char_type>
//    itr(begin);
//  for (; *itr; ++itr)
//    os << *itr;
//  return os;
//}

}  // namespace std

#endif  // BOOST_INTEROP_STREAM_HPP
