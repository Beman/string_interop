//  interop/make_test.cpp  -------------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <boost/string_interop/string_interop.hpp>
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
using char16_t;
using char32_t;
using namespace boost::interop;

namespace
{
  //  Test with cases that require UTF-16 surrogate pairs
  //  U+1F60A SMILING FACE WITH SMILING EYES
  //  U+1F60E SMILING FACE WITH SUNGLASSES

  // build test strings character by character so they work with C++03 compilers
  const char32_t u32c[] = {0x1F60A, 0x1F60E, 0};
  const char16_t u16c[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};

  const u32string u32s(u32c);
  const u16string u16s(u16c);
  const string u8s("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");
  const string chars("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");
  const wstring wchars(L"\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");

  void to_narrow_test()
  {
    cout << "to_narrow test..." << endl;

    string x, expected("Works!");
    wstring w(L"Works!");
    x = to_narrow<wide, string>(w);
    BOOST_TEST_EQ(x, expected);

    expected = "Works!";
    x = to_narrow<wide, string>(L"Works!");
    BOOST_TEST_EQ(x, expected);

    expected = "orks!";
    x = to_narrow<wide, string>(w.c_str()+1);
    BOOST_TEST_EQ(x, expected);

    expected = "Work";
    x = to_narrow<wide, string>(w.c_str(), 4);
    BOOST_TEST_EQ(x, expected);

    expected = "Works!";
    x = to_narrow<wide, string>(w.begin(), w.end());
    BOOST_TEST_EQ(x, expected);

    cout << "  to_narrow test complete" << endl;
  }

  void to_wide_test()
  {
    cout << "to_wide test..." << endl;

    wstring x, expected(L"Works!");
    string w("Works!");
    x = to_wide<narrow, wstring>(w);
    BOOST_TEST(x == expected);

    expected = L"Works!";
    x = to_wide<narrow, wstring>("Works!");
    BOOST_TEST(x == expected);

    expected = L"orks!";
    x = to_wide<narrow, wstring>(w.c_str()+1);
    BOOST_TEST(x == expected);

    expected = L"Work";
    x = to_wide<narrow, wstring>(w.c_str(), 4);
    BOOST_TEST(x == expected);

    expected = L"Works!";
    x = to_wide<narrow, wstring>(w.begin(), w.end());
    BOOST_TEST(x == expected);

    cout << "  to_wide test complete" << endl;
  }

  void to_utf8_test()
  {
    cout << "to_utf8 test..." << endl;

    string x, expected("Works!");
    wstring w(L"Works!");
    x = to_utf8<wide, string>(w);
    BOOST_TEST_EQ(x, expected);

    expected = "Works!";
    x = to_utf8<wide, string>(L"Works!");
    BOOST_TEST_EQ(x, expected);

    expected = "orks!";
    x = to_utf8<wide, string>(w.c_str()+1);
    BOOST_TEST_EQ(x, expected);

    expected = "Work";
    x = to_utf8<wide, string>(w.c_str(), 4);
    BOOST_TEST_EQ(x, expected);

    expected = "Works!";
    x = to_utf8<wide, string>(w.begin(), w.end());
    BOOST_TEST_EQ(x, expected);

    cout << "  to_utf8 test complete" << endl;
  }

  void to_utf16_test()
  {
    cout << "to_utf16 test..." << endl;

    u16string x, expected(make_string<utf16, narrow, u16string>("Works!"));
    wstring w(L"Works!");
    x = to_utf16<wide, u16string>(w);
    BOOST_TEST(x == expected);

    expected  = make_string<utf16, narrow, u16string>("Works!");
    x = to_utf16<wide, u16string>(L"Works!");
    BOOST_TEST(x == expected);

    expected  = make_string<utf16, narrow, u16string>("orks!");
    x = to_utf16<wide, u16string>(w.c_str()+1);
    BOOST_TEST(x == expected);

    expected  = make_string<utf16, narrow, u16string>("Work");
    x = to_utf16<wide, u16string>(w.c_str(), 4);
    BOOST_TEST(x == expected);

    expected  = make_string<utf16, narrow, u16string>("Works!");
    x = to_utf16<wide, u16string>(w.begin(), w.end());
    BOOST_TEST(x == expected);

    cout << "  to_utf16 test complete" << endl;
  }

  void to_utf32_test()
  {
    cout << "to_utf32 test..." << endl;

    u32string x, expected(make_string<utf32, narrow, u32string>("Works!"));
    wstring w(L"Works!");
    x = to_utf32<wide, u32string>(w);
    BOOST_TEST(x == expected);

    expected  = make_string<utf32, narrow, u32string>("Works!");
    x = to_utf32<wide, u32string>(L"Works!");
    BOOST_TEST(x == expected);

    expected  = make_string<utf32, narrow, u32string>("orks!");
    x = to_utf32<wide, u32string>(w.c_str()+1);
    BOOST_TEST(x == expected);

    expected  = make_string<utf32, narrow, u32string>("Work");
    x = to_utf32<wide, u32string>(w.c_str(), 4);
    BOOST_TEST(x == expected);

    expected  = make_string<utf32, narrow, u32string>("Works!");
    x = to_utf32<wide, u32string>(w.begin(), w.end());
    BOOST_TEST(x == expected);

    cout << "  to_utf32 test complete" << endl;
  }

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

  to_narrow_test();
  to_wide_test();
  to_utf8_test();
  to_utf16_test();
  to_utf32_test();


  return ::boost::report_errors();
}
