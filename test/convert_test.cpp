//  interop/convert_test.cpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <iostream>
#include <boost/interop/string_interop.hpp>
#include <cstring>  // for memcmp
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::string;
using std::wstring;
using boost::u8string;
using boost::u16string;
using boost::u32string;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;
using namespace boost::interop;

namespace
{
  //  Test with cases that require UTF-16 surrogate pairs
  //  U+1F60A SMILING FACE WITH SMILING EYES
  //  U+1F60E SMILING FACE WITH SUNGLASSES

  // build test strings character by character so they work with C++03 compilers
  const u32_t u32c[] = {0x1F60A, 0x1F60E, 0};
  const u16_t u16c[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
  const u8_t  u8c[] = {0xF0, 0x9F, 0x98, 0x8A, 0xF0, 0x9F, 0x98, 0x8E, 0};

  const u32string u32s(u32c);
  const u16string u16s(u16c);
  const u8string u8s(u8c);
  const string chars("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");

//----------------------------- no_default_arguments_test ------------------------------//

  void no_default_arguments_test()
  {
    std::cout << "no_default_arguments_test..." << std::endl;

    // container
    string s1 = convert<utf8, utf16, string>(u16s);
    BOOST_TEST_EQ(s1.size(), 8);
    BOOST_TEST(s1 == chars);

    // null terminated iterator
    string s2 = convert<utf8, utf16, string>(u16s.c_str());
    BOOST_TEST_EQ(s2.size(), 8);
    BOOST_TEST(s2 == chars);

    // iterator, size
    string s3 = convert<utf8, utf16, string>(u16s.c_str(), u16s.size());
    BOOST_TEST_EQ(s3.size(), 8);
    BOOST_TEST(s3 == chars);

    // iterator range
    string s4 = convert<utf8, utf16, string>(u16s.begin(), u16s.end());
    BOOST_TEST_EQ(s4.size(), 8);
    BOOST_TEST(s4 == chars);
  }

//-------------------------------- default_arguments_test ------------------------------//

  void default_arguments_test()
  {
# ifndef BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
    std::cout << "default_arguments_test..." << std::endl;

    // container
    string s1 = convert<utf8>(u16s);
    BOOST_TEST_EQ(s1.size(), 8);
    BOOST_TEST(s1 == chars);

    // null terminated iterator
    string s2 = convert<utf8>(u16s.c_str());
    BOOST_TEST_EQ(s2.size(), 8);
    BOOST_TEST(s2 == chars);

    // iterator, size
    string s3 = convert<utf8>(u16s.c_str(), u16s.size());
    BOOST_TEST_EQ(s3.size(), 8);
    BOOST_TEST(s3 == chars);

    // iterator range
    string s4 = convert<utf8>(u16s.begin(), u16s.end());
    BOOST_TEST_EQ(s4.size(), 8);
    BOOST_TEST(s4 == chars);
# endif
  }

}

//--------------------------------------------------------------------------------------//
//                                         main                                         //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{
  // verify test constants
  BOOST_TEST_EQ(u32s.size(), 2);
  BOOST_TEST_EQ(u16s.size(), 4);
  BOOST_TEST_EQ(u8s.size(), 8);
  BOOST_TEST(std::memcmp(u32s.c_str(), u32c, u32s.size())==0);
  BOOST_TEST(std::memcmp(u16s.c_str(), u16c, u16s.size())==0);
  BOOST_TEST(std::memcmp(u8s.c_str(), u8c, u8s.size())==0);

  no_default_arguments_test();
  default_arguments_test();

  return ::boost::report_errors();
}
