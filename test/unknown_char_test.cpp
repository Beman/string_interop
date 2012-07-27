//  interop/unknown_char_test.cpp  -----------------------------------------------------//

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

using std::cout;
using std::endl;
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

  // test known characters
  BOOST_TEST_EQ((make_string<narrow, wide, string>(L"foo")), string("foo"));

  // U+2144 is TURNED SANS-SERIF CAPITAL Y
  BOOST_TEST_EQ((make_string<narrow, wide, string>(L"f\x2144o")), string("f?o"));

  // u32s is two characters above the BMP
  BOOST_TEST_EQ((make_string<narrow, utf32, string>(u32s)), string("??"));

  //cout << "(Windows only) error characters: \""
  //     << make_string<narrow, utf32, string>(u32s) << "\"" << endl;

  return ::boost::report_errors();
}
