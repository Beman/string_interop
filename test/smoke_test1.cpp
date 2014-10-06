//  string_interop/smoke_test1.cpp  ----------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <iostream>
#include "../include/boost/string_interop/string_interop.hpp"
#include <cstring>  // for memcmp
#include <boost/static_assert.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <boost/type_traits.hpp>
#include <iomanip>
#include <locale>
#include <string>
#include <vector>
#include <cstdint>
#include <cvt/utf8>

using std::cout;
using std::endl;
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

  //  look for smoke

  //  narrow
  {
    std::cout << "narrow..." << std::endl;

    {
      // narrow from_iterator::from() tests
      typedef narrow::from_iterator test_from_iterator;
      narrow my_narrow;

      test_from_iterator itr1(my_narrow.from(chars));
      test_from_iterator itr2(my_narrow.from(chars.c_str()));
      test_from_iterator itr3(my_narrow.from(chars.c_str(), chars.c_str()+chars.size()));
      test_from_iterator itr4(my_narrow.from(chars.c_str(), 1));
      test_from_iterator itr5(my_narrow.from(chars.c_str(), 0));  // ambiguous without enable_if

      test_from_iterator itr1end;
      BOOST_TEST(itr1 != itr1end);
      BOOST_TEST(*itr1 == char32_t(0x1F60A));
      ++itr1;
      BOOST_TEST(itr1 != itr1end);
      BOOST_TEST(*itr1 == char32_t(0x1F60E));
      ++itr1;
      BOOST_TEST(itr1 == itr1end);
    }

    typedef stdext::cvt::codecvt_utf8<char32_t> cvt_utf8_type;
    typedef shared_codecvt_mgr<cvt_utf8_type> cvt_utf8_policy;
    typedef basic_narrow<char, default_error_handler, cvt_utf8_policy> narrow_utf8;

    narrow_utf8 char_utf8_dummy;  // not used; tests default construction compiles.
    // Note that codecvt is constructed only once as long as iterators are constructed
    // using char_utf8_dummy. 

    cvt_utf8_policy cvt_policy;
    narrow_utf8 char_utf8(cvt_policy);

    //  from_iterator tests

    typedef narrow_utf8::from_iterator test_from_iterator;

    {
      // narrow from_iterator::from() tests
      cout << "    narrow from_iterator::from() constructor tests" << endl;
      test_from_iterator itr1(char_utf8.from(chars));
      test_from_iterator itr2(char_utf8.from(chars.c_str()));
      test_from_iterator itr3(char_utf8.from(chars.c_str(), chars.c_str()+chars.size()));
      test_from_iterator itr4(char_utf8.from(chars.c_str(), 1));
      test_from_iterator itr5(char_utf8.from(chars.c_str(), 0));  // ambiguous without enable_if
    }

    {
      // utf32 from_iterator::from() tests
      cout << "    utf32 from_iterator::from() constructor tests" << endl;
      utf32::from_iterator itr1(utf32().from(u32s));
      utf32::from_iterator itr2(utf32().from(u32s.c_str()));
      utf32::from_iterator itr3(utf32().from(u32s.c_str(), u32s.c_str()+u32s.size()));
      utf32::from_iterator itr4(utf32().from(u32s.c_str(), 1));
      utf32::from_iterator itr5(utf32().from(u32s.c_str(), 0));  // ambiguous without enable_if
    }

    cout << "  narrow from_iterator::from() use tests" << endl;
    test_from_iterator begin1(char_utf8.from(chars));

    test_from_iterator begin1end;
    BOOST_TEST(begin1 != begin1end);
    BOOST_TEST(*begin1 == char32_t(0x1F60A));
    ++begin1;
    BOOST_TEST(begin1 != begin1end);
    BOOST_TEST(*begin1 == char32_t(0x1F60E));
    ++begin1;
    BOOST_TEST(begin1 == begin1end);

    begin1 = char_utf8.from(chars.c_str());
    BOOST_TEST(begin1 != begin1end);
    BOOST_TEST(*begin1 == char32_t(0x1F60A));
    ++begin1;
    BOOST_TEST(begin1 != begin1end);
    BOOST_TEST(*begin1 == char32_t(0x1F60E));
    ++begin1;
    BOOST_TEST(begin1 == begin1end);

    begin1 = char_utf8.from(chars.c_str());
    BOOST_TEST(begin1 != begin1end);
    ++begin1;
    BOOST_TEST(begin1 != begin1end);
    BOOST_TEST(*begin1 == char32_t(0x1F60E));
    ++begin1;
    BOOST_TEST(begin1 == begin1end);

    //  to_iterator tests

    typedef narrow_utf8::to_iterator<const char32_t*> test_to_iterator;

    test_to_iterator begin2(char_utf8.to(u32c));
    test_to_iterator begin2end;
    BOOST_TEST(begin2 != begin2end);

    BOOST_TEST(*begin2 == '\xF0');
    ++begin2;
    BOOST_TEST(*begin2 == '\x9F');
    ++begin2;
    BOOST_TEST(*begin2 == '\x98');
    ++begin2;
    BOOST_TEST(*begin2 == '\x8A');
    ++begin2;
    BOOST_TEST(*begin2 == '\xF0');
    ++begin2;
    BOOST_TEST(*begin2 == '\x9F');
    ++begin2;
    BOOST_TEST(*begin2 == '\x98');
    ++begin2;
    BOOST_TEST(*begin2 == '\x8E');
    // Note: test_to_iterator does not meet DefaultCtorEndIterator requirements, so
    // we cannot test to see that the end iterator is reached.

    //  conversion_iterator tests

    typedef conversion_iterator<narrow_utf8, narrow_utf8> cvt_type;

    cvt_type iter3(chars.c_str());

    BOOST_TEST(*iter3 == '\xF0');
    ++iter3;
    BOOST_TEST(*iter3 == '\x9F');
    ++iter3;
    BOOST_TEST(*iter3 == '\x98');
    ++iter3;
    BOOST_TEST(*iter3 == '\x8A');
    ++iter3;
    BOOST_TEST(*iter3 == '\xF0');
    ++iter3;
    BOOST_TEST(*iter3 == '\x9F');
    ++iter3;
    BOOST_TEST(*iter3 == '\x98');
    ++iter3;
    BOOST_TEST(*iter3 == '\x8E');
    ++iter3;
    BOOST_TEST(iter3 == cvt_type());

   }


  //  to_*string() family
  {
    cout << "  testing to_*string() family..." << endl;
    string s;
    s = to_string(chars);
    BOOST_TEST(s == u8s);
    s.clear();
    s = to_string(chars.c_str());
    BOOST_TEST(s == u8s);
    s.clear();
    s = to_string(chars.c_str(), 1);
    BOOST_TEST(s == u8s);
    s.clear();
    s = to_string(chars.c_str(), 0);
    BOOST_TEST(s == u8s);
    s.clear();
    s = to_string(chars.c_str(), chars.c_str()+chars.size());
    BOOST_TEST(s == u8s);
    //string s8 = to_u8string(chars);
    //BOOST_TEST(s8 == u8s);
    //u16string s16 = to_u16string(chars);
    //BOOST_TEST(s16 == u16s);
    //u32string s32 = to_u32string(chars);
    //BOOST_TEST(s32 == u32s);
    //wstring ws = to_wstring(chars);
    //BOOST_TEST(ws == wchars);
  }

  ////  wide
  //{
  //  std::cout << "wide..." << std::endl;
  //  typedef wide::from_iterator<const wchar_t*> test_from_iterator;
  //  test_from_iterator begin1(wchars.c_str());

  //  test_from_iterator begin1end;
  //  BOOST_TEST(begin1 != begin1end);

  //  typedef wide::to_iterator<const char32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  typedef wide::from_iterator<wstring::const_iterator> test_string_from_iterator;
  //  test_string_from_iterator begin4(wchars.begin(), wchars.begin());
  //  test_string_from_iterator begin4end;
  //  BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
  //                                   // if invalid assumptions made about default
  //                                   // constructed iterator

  //  BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<wchar_t>::type,
  //    wide>::value), "auto detected the wrong type");

  //  typedef conversion_iterator<wide, wide, const wchar_t*>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  wstring source(L"foo");
  //  wstring result = make_string<wide, wide, wstring>(source);
  //  BOOST_TEST(source == result);
  //}

  ////  utf8
  //{
  //  std::cout << "utf8..." << std::endl;
  //  typedef utf8::from_iterator<const char*> test_from_iterator;
  //  test_from_iterator begin1(chars.c_str());

  //  test_from_iterator begin1end;
  //  BOOST_TEST(begin1 != begin1end);

  //  typedef utf8::to_iterator<const char32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  test_from_iterator begin3(chars.c_str(), chars.c_str());

  //  test_from_iterator begin3end;
  //  BOOST_TEST(begin3 == begin3end);

  //  typedef utf8::from_iterator<string::const_iterator> test_string_from_iterator;
  //  test_string_from_iterator begin4(chars.begin(), chars.begin());
  //  test_string_from_iterator begin4end;
  //  BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
  //                                   // if invalid assumptions made about default
  //                                   // constructed iterator

  //  //  utf8 is not auto-deteced from char, so don't do the usual static_assert
  //  //BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char>::type, narrow>::value),
  //  //  "auto detected the wrong type");

  //  typedef conversion_iterator<utf8, utf8, const char*>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  string source("foo");
  //  string result = make_string<utf8, utf8, string>(source);
  //  BOOST_TEST_EQ(source, result);
  //}

  ////  utf16
  //{
  //  std::cout << "utf16..." << std::endl;
  //  typedef utf16::from_iterator<const char16_t*> test_from_iterator;
  //  test_from_iterator begin1(u16s.c_str());

  //  typedef utf16::to_iterator<const char32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  typedef utf16::from_iterator<u16string::const_iterator> test_string_from_iterator;
  //  test_string_from_iterator begin4(u16s.begin(), u16s.begin());
  //  test_string_from_iterator begin4end;
  //  BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
  //                                   // if invalid assumptions made about default
  //                                   // constructed iterator

  //  BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char16_t>::type,
  //    utf16>::value), "auto detected the wrong type");

  //  typedef conversion_iterator<utf16, utf16, const char16_t*>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  u16string source(u16s);
  //  u16string result = make_string<utf16, utf16, u16string>(source);
  //  BOOST_TEST(source == result);
  //}

  ////  utf32
  //{
  //  std::cout << "utf32..." << std::endl;
  //  typedef utf32::from_iterator<const char32_t*> test_from_iterator;
  //  test_from_iterator begin1(u32s.c_str());

  //  typedef utf32::to_iterator<const char32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  typedef utf32::from_iterator<u32string::const_iterator> test_string_from_iterator;
  //  test_string_from_iterator begin4(u32s.begin(), u32s.begin());
  //  test_string_from_iterator begin4end;
  //  BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
  //                                   // if invalid assumptions made about default
  //                                   // constructed iterator

  //  BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char32_t>::type,
  //    utf32>::value), "auto detected the wrong type");

  //  typedef conversion_iterator<utf32, utf32, const char32_t*>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  u32string source(u32s);
  //  u32string result = make_string<utf32, utf32, u32string>(source);
  //  BOOST_TEST(source == result);
  //}

  //// make_string alias experiment

  //string x, expected("OK");
  //wstring w(L"OK");
  //x = make_narrow<wide, string, wstring>(w);
  //BOOST_TEST_EQ(x, expected);

  return ::boost::report_errors();
}
