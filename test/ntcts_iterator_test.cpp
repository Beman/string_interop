//  ntcts_iterator_test.cpp  -----------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <string>
#include <iostream>
#include <boost/interop/ntcts_iterator.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::cout;
using std::endl;
using namespace boost::interop;

//--------------------------------------------------------------------------------------//
//                                      cpp_main                                        //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{
  char_iterator i1 = char_iterator();
  char_iterator i2("");
  BOOST_TEST(i1 == i2);
  char_iterator i3("z");
  BOOST_TEST(i1 != i3);
  ++i3;
  BOOST_TEST(i1 == i3);

  //std::string s(char_iterator("foo bar"), char_iterator());
  //BOOST_TEST_EQ(s.size(), 7U);
  //BOOST_TEST_EQ(s, std::string("foo bar"));
  return ::boost::report_errors();
}
