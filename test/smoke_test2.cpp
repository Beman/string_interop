//  interop/smoke_test2.cpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <boost/interop/convert.hpp>
#include <cstring>  // for memcmp
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <boost/type_traits.hpp>
#include <iostream>

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

}  // unnamed namespace


//--------------------------------------------------------------------------------------//
//                                      cpp_main                                        //
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

  //  look for smoke

  //  narrow
  {
    typedef narrow::from_iterator test_from_iterator;
    test_from_iterator begin1(chars.c_str());

    typedef narrow::to_iterator<const u32_t*> test_to_iterator;
    test_to_iterator begin2(u32c);

    static_assert(boost::is_same<auto_detect::codec<char>::type, narrow>::value,
      "auto detected the wrong type");

    typedef conversion_iterator<narrow, narrow>
      conversion_iterator_example;
    conversion_iterator_example cvn_iterator;

    string source("foo");
    string result = convert<narrow, narrow, string>(source);
    BOOST_TEST_EQ(source, result);
  }

  ////  utf8
  //{
  //  typedef utf8::from_iterator test_from_iterator;
  //  test_from_iterator begin1(chars.c_str());

  //  typedef utf8::to_iterator<const u32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  //  utf8 is not auto-deteced from char, so don't do the usual static_assert
  //  //static_assert(boost::is_same<auto_detect::codec<char>::type, narrow>::value,
  //  //  "auto detected the wrong type");

  //  typedef conversion_iterator<utf8, utf8>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  string source("foo");
  //  string result = convert<utf8, utf8, string>(source);
  //  BOOST_TEST_EQ(source, result);
  //}

  ////  utf16
  //{
  //  typedef utf16::from_iterator<const u16_t*> test_from_iterator;
  //  test_from_iterator begin1(u16s.c_str());

  //  typedef utf16::to_iterator<const u32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  static_assert(boost::is_same<auto_detect::codec<u16_t>::type, utf16>::value,
  //    "auto detected the wrong type");

  //  typedef conversion_iterator<utf16, utf16>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  u16string source(u16s);
  //  u16string result = convert<utf16, utf16, u16string>(source);
  //  BOOST_TEST(source == result);
  //}

  ////  utf32
  //{
  //  typedef utf32::from_iterator<const u32_t*> test_from_iterator;
  //  test_from_iterator begin1(u32s.c_str());

  //  typedef utf32::to_iterator<const u32_t*> test_to_iterator;
  //  test_to_iterator begin2(u32c);

  //  static_assert(boost::is_same<auto_detect::codec<u32_t>::type, utf32>::value,
  //    "auto detected the wrong type");

  //  typedef conversion_iterator<utf32, utf32>
  //    conversion_iterator_example;
  //  conversion_iterator_example cvn_iterator;

  //  u32string source(u32s);
  //  u32string result = convert<utf32, utf32, u32string>(source);
  //  BOOST_TEST(source == result);
  //}

  return ::boost::report_errors();
}
