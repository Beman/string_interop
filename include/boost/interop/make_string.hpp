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

#include <boost/interop/iterator_adapter.hpp>
#include <algorithm>

namespace boost
{
namespace interop
{

  template <class ToString, class FromString>
  ToString make_string(const FromString& x)
  {
    ToString tmp;
    for converting_iterator<FromString::const_iterator, FromString::value_type,
                            by_range, ToString::value_type>
       itr(x.cbegin(), x.cend());
       *itr != 0; ++itr)
         std::back_insert_iterator(tmp);

       return tmp;
  }

  //  TODO:
  //  * Two iterators
  //  * One iterator
  //  * Iterator, size

}  // namespace interop
}  // namespace boost

#endif  // BOOST_MAKE_STRING_HPP
