//  interop/smoke_test2.cpp  ----------------------------------------------------------//

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

  //  look for smoke

  //  narrow
  {
    cout << "narrow..." << endl;
    typedef narrow::from_iterator<const char*> test_from_iterator;
    test_from_iterator begin1(chars.c_str());

    test_from_iterator begin1end;
    BOOST_TEST(begin1 != begin1end);

    typedef narrow::to_iterator<const char32*> test_to_iterator;
    test_to_iterator begin2(u32c);

    typedef narrow::from_iterator<string::const_iterator> test_string_from_iterator;
    test_string_from_iterator begin4(chars.begin(), chars.begin());
    test_string_from_iterator begin4end;
    BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
                                     // if invalid assumptions made about default
                                     // constructed iterator

    BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char>::type,
      narrow>::value), "auto detected the wrong type");

    typedef conversion_iterator<narrow, narrow, const char*>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    string source("foo");
    string result = make_string<narrow, narrow, string>(source);
    BOOST_TEST_EQ(source, result);
  }

  //  wide
  {
    cout << "wide..." << endl;
    typedef wide::from_iterator<const wchar_t*> test_from_iterator;
    test_from_iterator begin1(wchars.c_str());

    test_from_iterator begin1end;
    BOOST_TEST(begin1 != begin1end);

    typedef wide::to_iterator<const char32*> test_to_iterator;
    test_to_iterator begin2(u32c);

    typedef wide::from_iterator<wstring::const_iterator> test_string_from_iterator;
    test_string_from_iterator begin4(wchars.begin(), wchars.begin());
    test_string_from_iterator begin4end;
    BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
                                     // if invalid assumptions made about default
                                     // constructed iterator

    BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<wchar_t>::type,
      wide>::value), "auto detected the wrong type");

    typedef conversion_iterator<wide, wide, const wchar_t*>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    wstring source(L"foo");
    wstring result = make_string<wide, wide, wstring>(source);
    BOOST_TEST(source == result);
  }

  //  utf8
  {
    cout << "utf8..." << endl;
    typedef utf8::from_iterator<const char*> test_from_iterator;
    test_from_iterator begin1(chars.c_str());

    test_from_iterator begin1end;
    BOOST_TEST(begin1 != begin1end);

    typedef utf8::to_iterator<const char32*> test_to_iterator;
    test_to_iterator begin2(u32c);

    test_from_iterator begin3(chars.c_str(), chars.c_str());

    test_from_iterator begin3end;
    BOOST_TEST(begin3 == begin3end);

    typedef utf8::from_iterator<string::const_iterator> test_string_from_iterator;
    test_string_from_iterator begin4(chars.begin(), chars.begin());
    test_string_from_iterator begin4end;
    BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
                                     // if invalid assumptions made about default
                                     // constructed iterator

    //  utf8 is not auto-deteced from char, so don't do the usual static_assert
    //BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char>::type, narrow>::value),
    //  "auto detected the wrong type");

    typedef conversion_iterator<utf8, utf8, const char*>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    string source("foo");
    string result = make_string<utf8, utf8, string>(source);
    BOOST_TEST_EQ(source, result);
  }

  //  utf16
  {
    cout << "utf16..." << endl;
    typedef utf16::from_iterator<const char16*> test_from_iterator;
    test_from_iterator begin1(u16s.c_str());

    typedef utf16::to_iterator<const char32*> test_to_iterator;
    test_to_iterator begin2(u32c);

    typedef utf16::from_iterator<u16string::const_iterator> test_string_from_iterator;
    test_string_from_iterator begin4(u16s.begin(), u16s.begin());
    test_string_from_iterator begin4end;
    BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
                                     // if invalid assumptions made about default
                                     // constructed iterator

    BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char16>::type,
      utf16>::value), "auto detected the wrong type");

    typedef conversion_iterator<utf16, utf16, const char16*>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    u16string source(u16s);
    u16string result = make_string<utf16, utf16, u16string>(source);
    BOOST_TEST(source == result);
  }

  //  utf32
  {
    cout << "utf32..." << endl;
    typedef utf32::from_iterator<const char32*> test_from_iterator;
    test_from_iterator begin1(u32s.c_str());

    typedef utf32::to_iterator<const char32*> test_to_iterator;
    test_to_iterator begin2(u32c);

    typedef utf32::from_iterator<u32string::const_iterator> test_string_from_iterator;
    test_string_from_iterator begin4(u32s.begin(), u32s.begin());
    test_string_from_iterator begin4end;
    BOOST_TEST(begin4 == begin4end); // will assert in VC++ <xstring> debug build
                                     // if invalid assumptions made about default
                                     // constructed iterator

    BOOST_STATIC_ASSERT_MSG((boost::is_same<default_codec::codec<char32>::type,
      utf32>::value), "auto detected the wrong type");

    typedef conversion_iterator<utf32, utf32, const char32*>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    u32string source(u32s);
    u32string result = make_string<utf32, utf32, u32string>(source);
    BOOST_TEST(source == result);
  }

  //  error character tests
  cout << "(Windows only) error characters: \""
       << make_string<narrow, utf32, string>(u32s) << "\"" << endl;

  //// make_string alias experiment

  //string x, expected("OK");
  //wstring w(L"OK");
  //x = make_narrow<wide, string, wstring>(w);
  //BOOST_TEST_EQ(x, expected);

  return ::boost::report_errors();
}
