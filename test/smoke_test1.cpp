//  string_interop/smoke_test1.cpp  ----------------------------------------------------//

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

using std::string;
using std::wstring;
using std::u16string;
using std::u32string;
using namespace boost::string_interop;

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
  const string s("\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");
  const wstring ws(L"\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E");

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

  //  look for smoke

#ifdef BOOST_WINDOWS_API
  BOOST_STATIC_ASSERT_MSG(boost::is_same<wide::actual_encoding, utf16>::value,
    "wide::actual_encoding wrong. Is detail::wide_encoding helper trait working?");
#endif

  {
    from_iterator<utf8, char, const char*> end_iter8;
    from_iterator<utf8, char, const char*> iter8(u8s.c_str());

    from_iterator<utf16, char16_t, const char16_t*> end_iter16;
    from_iterator<utf16, char16_t, const char16_t*> iter16(u16s.c_str());

    from_iterator<utf32, char32_t, const char32_t*> end_iter32;
    from_iterator<utf32, char32_t, const char32_t*> iter32(u32s.c_str());

    from_iterator<wide::actual_encoding, wchar_t, const wchar_t*> end_iterw;
    from_iterator<wide::actual_encoding, wchar_t, const wchar_t*> iterw(ws.c_str());
  }
  {
    to_iterator<utf8, char, const char32_t*> end_iter8;
    to_iterator<utf8, char, const char32_t*> iter8(u32s.c_str());

    to_iterator<utf16, char16_t, const char32_t*> end_iter16;
    to_iterator<utf16, char16_t, const char32_t*> iter16(u32s.c_str());

    to_iterator<utf32, char32_t, const char32_t*> end_iter32;
    to_iterator<utf32, char32_t, const char32_t*> iter32(u32s.c_str());

    to_iterator<wide::actual_encoding, wchar_t, const char32_t*> end_iterw;
    to_iterator<wide::actual_encoding, wchar_t, const char32_t*> iterw(u32s.c_str());
  }
  {
    conversion_iterator<utf8, utf8, const char*> end_iter_8_8;
    conversion_iterator<utf8, utf8, const char*> iter_8_8(u8s.c_str());

    conversion_iterator<utf16, utf8, const char*> end_iter_16_8;
    conversion_iterator<utf16, utf8, const char*> iter_16_8(u8s.c_str());

    conversion_iterator<utf32, utf8, const char*> end_iter_32_8;
    conversion_iterator<utf32, utf8, const char*> iter_32_8(u8s.c_str());

    conversion_iterator<wide, utf8, const char*> end_iter_wide_8;
    conversion_iterator<wide, utf8, const char*> iter_wide_8(u8s.c_str());
  }

  {
    u16string result1 = to_u16string<utf8>(u8s.c_str());
    // alternate formulation: to_u16string<utf8>(u8s.c_str())
    BOOST_TEST(result1 == u16s);

    u16string result2 = to_u16string(u32s.c_str());
    BOOST_TEST(result1 == u16s);
  }

  return ::boost::report_errors();
}
