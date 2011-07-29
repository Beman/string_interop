//  libs/interop/test/smoke_test.cpp  --------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using namespace std;
#include <boost/interop/string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

//  name clashes between the standard library and this library are not desirable because
//  interoperability is critical, so provide using namespaces for std and boost to detect
//  such clashes
using namespace boost;

namespace
{
  template <class T>
  void dump(const T& s)
  {
    for (size_t i = 0; i < s.size(); ++i)
    {
      if (i)
        cout << ',';
      cout << char(s[i]);
    }
    cout << '\n';
  }
}

int cpp_main(int, char*[])
{
  cout << "smoke test...\n" << hex;

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


  iu8string s8;
  iu16string s16;
  iu32string s32;

  u32_t U024B62[] = { 0x024B62, 0x0 };  // see http://en.wikipedia.org/wiki/Utf-8
  s32.append(U024B62);
  BOOST_TEST_EQ(s32.size(), 1U);
  s8.new_append(s32.c_str());
  BOOST_TEST_EQ(s8.size(), 4U);

  cout << hex << int(s8[0]) << ' ' << int(s8[1]) << ' '
       << int(s8[2]) << ' ' << int(s8[3]) << '\n';

  BOOST_TEST(s8[0] == 0xF0);
  BOOST_TEST(s8[1] == 0xA4);
  BOOST_TEST(s8[2] == 0xAD);
  BOOST_TEST(s8[3] == 0xA2);

  //  probe each of the combinations we have codecs for

  iu8string t8;
  iu16string t16;
  iu32string t32;

  s8.clear();
  s16.clear();
  s32.clear();
  s8.push_back('8');
  dump(s8);
  BOOST_TEST_EQ(s8.size(), 1U);
  s16.push_back('u');
  dump(s16);
  BOOST_TEST_EQ(s16.size(), 1U);
  s32.push_back('U');
  dump(s32);
  BOOST_TEST_EQ(s32.size(), 1U);

  cout << "\nUTF-8 <-- UTF-8\n";
  t8.new_append(s8.c_str());
  dump(t8);
  BOOST_TEST_EQ(t8.size(), 1U);

  cout << "\nUTF-8 <-- UTF-16\n";
  t8.new_append(s16.c_str());
  dump(t8);
  BOOST_TEST_EQ(t8.size(), 2U);

  cout << "\nUTF-8 <-- UTF-32\n";
  t8.new_append(s32.c_str());
  dump(t8);
  BOOST_TEST_EQ(t8.size(), 3U);

  cout << "\nUTF-16 <-- UTF-8\n";
  t16.new_append(s8.c_str());
  dump(t16);
  BOOST_TEST_EQ(t16.size(), 1U);

  cout << "\nUTF-16 <-- UTF-16\n";
  t16.new_append(s16.c_str());
  dump(t16);
  BOOST_TEST_EQ(t16.size(), 2U);

  cout << "\nUTF-16 <-- UTF-32\n";
  t16.new_append(s32.c_str());
  dump(t16);
  BOOST_TEST_EQ(t16.size(), 3U);

  cout << "\nUTF-32 <-- UTF-8\n";
  t32.new_append(s8.c_str());
  dump(t32);
  BOOST_TEST_EQ(t32.size(), 1U);

  cout << "\nUTF-32 <-- UTF-16\n";
  t32.new_append(s16.c_str());
  dump(t32);
  BOOST_TEST_EQ(t32.size(), 2U);

  cout << "\nUTF-32 <-- UTF-32\n";
  t32.new_append(s32.c_str());
  dump(t32);
  BOOST_TEST_EQ(t32.size(), 3U);

  return ::boost::report_errors();
}
