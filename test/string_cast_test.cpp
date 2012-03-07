//  interop/test/string_cast_test.cpp  -------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/interop/string_conversion.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using namespace std;
using namespace std::tbd;

//--------------------------------------------------------------------------------------//
//                                         main                                         //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{
  //string s1(string_cast<string>(u16string(u"test 1")));
  //BOOST_TEST_EQ(s1, string("test 1"));

  //u16string s2(string_cast<u16string>(string("test 2")));
  //BOOST_TEST_EQ(s2, u16string(u"test 2"));

  wstring   sw();
  u16string s16();
  u32string s32();

  string_cast<string>(sw);
  string_cast<string>(sw, user_error_handler);
  string_cast<string>(s16);
  string_cast<string>(s16, user_error_handler);
  string_cast<string>(s32);
  string_cast<string>(s32, user_error_handler);

  //string_cast<string>(s16, user_from_codec16);
  //string_cast<string>(s16, user_from_codec16, user_error_handler);
  //string_cast<string>(s16, user_to_codec16);
  //string_cast<string>(s16, user_to_codec16, user_error_handler);
  //string_cast<string>(s16, user_from_codec16, user_to_codec16);
  //string_cast<string>(s16, user_from_codec16, user_to_codec16, user_error_handler);


  return ::boost::report_errors();
}
