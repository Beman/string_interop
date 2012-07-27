//  interop/make_test.cpp  -------------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <boost/interop/string_interop.hpp>
#include <cstring>  // for memcmp
#include <boost/static_assert.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <boost/type_traits.hpp>
#include <iostream>

using std::string;
using std::wstring;
using boost::u16string;
using boost::u32string;
using boost::char16;
using boost::char32;
using namespace boost::interop;

namespace
{
  //  Test with cases that require UTF-16 surrogate pairs
  //  U+1F60A SMILING FACE WITH SMILING EYES
  //  U+1F60E SMILING FACE WITH SUNGLASSES

  // build test strings character by character so they work with C++03 compilers
  const char32 u32c[] = {0x1F60A, 0x1F60E, 0};
  const char16 u16c[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};

  const u32string u32s(u32c);
  const u16string u16s(u16c);
  const string u8s("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");
  const string chars("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");
  const wstring wchars(L"\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");

}  // unnamed namespace


//--------------------------------------------------------------------------------------//
//                                      cpp_main                                        //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{
  // verify test constants
  BOOST_TEST_EQ(u32s.size(), 2U);
  BOOST_TEST_EQ(u16s.size(), 4U);
  BOOST_TEST_EQ(u8s.size(), 8U);
  BOOST_TEST(std::memcmp(u32s.c_str(), u32c, u32s.size())==0);
  BOOST_TEST(std::memcmp(u16s.c_str(), u16c, u16s.size())==0);

  // make_string alias tests

  {
    string x, expected("Works!");
    wstring w(L"Works!");
    x = make_narrow<wide, string>(w);
    BOOST_TEST_EQ(x, expected);
  }
  {
    string x, expected("Works!");
    x = make_narrow<wide, string>(L"Works!");
    BOOST_TEST_EQ(x, expected);
  }
  {
    string x, expected("Works!");
    wstring w(L"Works!");
    x = make_narrow<wide, string>(w.c_str());
    BOOST_TEST_EQ(x, expected);
  }
  {
    string x, expected("Work");
    wstring w(L"Works!");
    x = make_narrow<wide, string>(w.c_str(), 4);
    BOOST_TEST_EQ(x, expected);
  }
  {
    string x, expected("Works!");
    wstring w(L"Works!");
    x = make_narrow<wide, string>(w.begin(), w.end());
    BOOST_TEST_EQ(x, expected);
  }

  return ::boost::report_errors();
}
