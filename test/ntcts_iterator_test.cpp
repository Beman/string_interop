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

  const char* p3 = "z";
  char_iterator i3(p3);
  BOOST_TEST(i3 != i1);
  BOOST_TEST_EQ(*i3, 'z');
  ++i3;
  BOOST_TEST(i1 == i3);

  const char* p4 = "ab";
  char_iterator i4(p4);
  BOOST_TEST(i4 != i1);
  BOOST_TEST_EQ(*i4, 'a');
  //*i4 = 'x';  // error: left operand must be l-value
  ++i4;
  BOOST_TEST(i4 != i1);
  BOOST_TEST_EQ(*i4, 'b');
  ++i4;
  BOOST_TEST(i1 == i4);

  const char* p5 = "foo bar";
  std::string s5;
  s5.assign(char_iterator(p5), char_iterator());
  BOOST_TEST_EQ(s5.size(), 7U);
  BOOST_TEST_EQ(s5, std::string("foo bar"));

  char a[] = {'a', 'b', '\0'};
  ntcts_iterator<char> i6(a);
  BOOST_TEST_EQ(*i6, 'a');
  *i6 = 'z';
  BOOST_TEST_EQ(*i6, 'z');

  return ::boost::report_errors();
}
