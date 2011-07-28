//  libs/interop/test/smoke_test.cpp  --------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/interop/string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

//  name clashes between the standard library and this library are not desirable because
//  interoperability is critical, so provide using namespaces for std and boost to detect
//  such clashes
using namespace std;
using namespace boost;

int cpp_main(int, char*[])
{
  cout << "smoke test...\n";

  istring si;   
  iwstring siw;  
  iu8string siu8; 
  iu16string siu16;
  iu32string siu32;

  si.append("si");
  BOOST_TEST_EQ(si.size(), 2U);
  siw.append(L"siw");
  BOOST_TEST_EQ(siw.size(), 3U);
  u8_t csu8[] = {'s', 'i', 'u', '8', '\0'};
  siu8.append(csu8);
  BOOST_TEST_EQ(siu8.size(), 4U);
  u16_t csu16[] = {'s', 'i', 'u', '1', '6', '\0'};
  siu16.append(csu16);
  BOOST_TEST_EQ(siu16.size(), 5U);
  u32_t csu32[] = {'s', 'i', 'u', '3', '2', '\0'};
  siu32.append(csu32);
  BOOST_TEST_EQ(siu32.size(), 5U);

  const u8_t* p = siu8.c_str();
//  siu32.new_append(p);

  u32_t U024B62[] = { 0x024B62, 0x0 };  // see http://en.wikipedia.org/wiki/Utf-8
  iu32string s32;
  s32.append(U024B62);
  BOOST_TEST_EQ(s32.size(), 1U);
  iu8string s8;
  s8.new_append(s32.c_str());
  BOOST_TEST_EQ(s8.size(), 4U);

  cout << hex << int(s8[0]) << ' ' << int(s8[1]) << ' '
       << int(s8[2]) << ' ' << int(s8[3]) << '\n';

  BOOST_TEST(s8[0] == 0xF0);
  BOOST_TEST(s8[1] == 0xA4);
  BOOST_TEST(s8[2] == 0xAD);
  BOOST_TEST(s8[3] == 0xA2);

  return ::boost::report_errors();
}
