//  boost/interop/string_operators.hpp  ------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  Additional operators and algorithms for interoperation between character strings    //
//                      of different types and encodings.                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_STRING_OPERATORS_HPP)
#define BOOST_STRING_OPERATORS_HPP

#include <boost/interop/string_operators.hpp>
#include <string>
#include <iterator>
#include <algorithm>

namespace boost
{
namespace interop
{

//--------------------------------------------------------------------------------------//
//                              copy_string algorithm                                   //
//--------------------------------------------------------------------------------------//

template<class InputIterator, class FromCodec,
         class OutputIterator, class ToCodec>
inline
OutputIterator
copy_string(InputIterator first, InputIterator last, OutputIterator result)
{
  typedef conversion_iterator<ToCodec,
    typename FromCodec::template
      codec<typename std::iterator_traits<InputIterator>::value_type>::type,
    InputIterator>
      iter_type;
  return std::copy(iter_type(begin, end), iter_type(), result);
}

//--------------------------------------------------------------------------------------//
//                                   assignment                                         //
//--------------------------------------------------------------------------------------//

template <class charT1, class traits1, class Allocator1
          class charT2, class traits2, class Allocator2>
inline
std::basic_string<charT1, traits1, Allocator1>&
operator=(std::basic_string<charT1, traits1, Allocator1>& lhs,
          std::basic_string<charT2, traits2, Allocator2>& rhs)
{
std::cout << "operator=" << std::endl;
  if (&lhs != &rhs)
  {
    lhs.clear();
    copy_string(rhs.cbegin(), rhs.cend(),std::back_inserter(lhs));
  }
  return lhs;
}

//--------------------------------------------------------------------------------------//
//                                 concatenation                                        //
//--------------------------------------------------------------------------------------//

template <class charT1, class traits1, class Allocator1
          class charT2, class traits2, class Allocator2>
inline
std::basic_string<charT1, traits1, Allocator1>&
operator+=(std::basic_string<charT1, traits1, Allocator1>& lhs,
          std::basic_string<charT2, traits2, Allocator2>& rhs)
{
std::cout << "operator+=" << std::endl;
  copy_string(rhs.cbegin(), rhs.cend(),std::back_inserter(lhs));
  return lhs;
}

}  // namespace interop
}  // namespace boost

#endif  // BOOST_STRING_OPERATORS_HPP
