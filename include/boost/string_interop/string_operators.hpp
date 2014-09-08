//  boost/string_interop/string_operators.hpp  ------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  Additional operators and algorithms for interoperation between character strings    //
//                      of different types and encodings.                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#ifndef BOOST_STRING_OPERATORS_HPP
#define BOOST_STRING_OPERATORS_HPP

#include <boost/string_interop/string_interop.hpp>
#include <string>
#include <iterator>
#include <algorithm>

namespace boost
{
namespace string_interop
{

//--------------------------------------------------------------------------------------//
//                              copy_string algorithm                                   //
//--------------------------------------------------------------------------------------//

template<class FromCodec, class ToCodec, class InputIterator,
         class OutputIterator>
inline
OutputIterator
copy_string(InputIterator first, InputIterator last, OutputIterator result)
{
std::cout << "copy_string" << std::endl;
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;
  return std::copy(iter_type(first, last), iter_type(), result);
}

//--------------------------------------------------------------------------------------//
//                                     assign                                           //
//--------------------------------------------------------------------------------------//

template <class charT1, class traits1, class Allocator1,
          class charT2, class traits2, class Allocator2>
inline
std::basic_string<charT1, traits1, Allocator1>&
assign(std::basic_string<charT1, traits1, Allocator1>& to,
       const std::basic_string<charT2, traits2, Allocator2>& from)
{
std::cout << "assign" << std::endl;
  std::basic_string<charT1, traits1, Allocator1> tmp;
  copy_string<select_codec<charT2>::type, select_codec<charT1>::type>
    (from.cbegin(), from.cend(), std::back_inserter(tmp));
  std::swap(tmp, to);
  return to;
}

////--------------------------------------------------------------------------------------//
////                                    append                                            //
////--------------------------------------------------------------------------------------//
//
//template <class charT1, class traits1, class Allocator1,
//          class charT2, class traits2, class Allocator2>
//inline
//std::basic_string<charT1, traits1, Allocator1>&
//operator+=(std::basic_string<charT1, traits1, Allocator1>& to,
//          std::basic_string<charT2, traits2, Allocator2>& from)
//{
//std::cout << "operator+=" << std::endl;
//  copy_string(from.cbegin(), from.cend(),std::back_inserter(to));
//  return to;
//}

}  // namespace string_interop
}  // namespace boost

#endif  // BOOST_STRING_OPERATORS_HPP
