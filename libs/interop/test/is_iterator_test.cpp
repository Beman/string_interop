//  libs/interop/test/is_iterator_test.cpp  --------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/interop/detail/is_iterator.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <iostream>
#include <vector>

using boost::is_iterator;

int cpp_main(int, char*[])
{
  std::cout << "is_iterator test...\n";

  BOOST_TEST(!is_iterator<int>::value);
  BOOST_TEST(is_iterator<int*>::value);
  BOOST_TEST(is_iterator<const int*>::value);
  BOOST_TEST(is_iterator<volatile int*>::value);
  BOOST_TEST(is_iterator<const volatile int*>::value);
  BOOST_TEST(!is_iterator<std::vector<int> >::value);
  BOOST_TEST(is_iterator<std::vector<int>::iterator>::value);
  BOOST_TEST(is_iterator<const std::vector<int>::iterator>::value);
  BOOST_TEST(is_iterator<std::vector<int>::const_iterator>::value);

  return ::boost::report_errors();
}
