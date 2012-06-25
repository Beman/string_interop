//  interop/convert_test.cpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <boost/interop/convert.hpp>
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

//------------------------------------ simple_test -------------------------------------//

  void simple_test()
  {
    std::cout << "simple_test..." << std::endl;

    // container
    u8string s1 = convert<u8string>(u32s);
    BOOST_TEST_EQ(s1.size(), 8);
    BOOST_TEST(s1 == u8s);

    // iterator, null terminated
    u8string s2 = convert<u8string>(u32s.c_str());
    BOOST_TEST_EQ(s2.size(), 8);
    BOOST_TEST(s2 == u8s);

    // iterator, size
    u8string s3 = convert<u8string>(u32s.c_str(), u32s.size());
    BOOST_TEST_EQ(s3.size(), 8);
    BOOST_TEST(s3 == u8s);

    // iterator range
    u8string s4 = convert<u8string>(u32s.begin(), u32s.end());
    BOOST_TEST_EQ(s4.size(), 8);
    BOOST_TEST(s4 == u8s);
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

  simple_test();

  return ::boost::report_errors();
}
