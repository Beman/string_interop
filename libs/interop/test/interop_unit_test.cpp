//  libs/interop/test/interop_unit_test.cpp  -------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using std::cout; using std::endl; using std::hex;
#include <boost/interop/string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace boost;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  The objective of these tests is to call each boost::xop::basic_string signature     //
//  at least once.                                                                      //
//                                                                                      //
//  See interop_test.cpp for tests that probe combinations of character types and       //
//  encodings.                                                                          //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace
{
//  template <class T>
//  void dump(const T& s)
//  {
//    for (size_t i = 0; i < s.size(); ++i)
//    {
//      if (i)
//        cout << ',';
//      cout << char(s[i]);
//    }
//    cout << '\n';
//  }
  char     pipsqueek[]  = { 'P', 'i', 'p', 's', 'q', 'u', 'e', 'e', 'k', 0 };
  wchar_t  wpipsqueek[] = { 'P', 'i', 'p', 's', 'q', 'u', 'e', 'e', 'k', 0 };
  u8_t     u8pipsqueek[]    = { 'P', 'i', 'p', 's', 'q', 'u', 'e', 'e', 'k', 0 };
  u16_t    u16pipsqueek[]   = { 'P', 'i', 'p', 's', 'q', 'u', 'e', 'e', 'k', 0 };
  u32_t    u32pipsqueek[]   = { 'P', 'i', 'p', 's', 'q', 'u', 'e', 'e', 'k', 0 };

  char     meow[]  = { 'M', 'e', 'o', 'w', 0 };
  wchar_t  wmeow[] = { 'M', 'e', 'o', 'w', 0 };
  u8_t     u8meow[]    = { 'M', 'e', 'o', 'w', 0 };
  u16_t    u16meow[]   = { 'M', 'e', 'o', 'w', 0 };
  u32_t    u32meow[]   = { 'M', 'e', 'o', 'w', 0 };

  void c_str_test()
  {
    cout << "  c_str() template test...\n";

    xop::wstring wstr(L"Meow\n");

    std::string str;

    for (auto itr=wstr.c_str<char>(); *itr; ++itr)
      str.push_back(*itr);

    BOOST_TEST_EQ(str, std::string("Meow\n"));
  }


}  // unnamed namespace

int cpp_main(int, char*[])
{
  cout << "string interoperability unit test...\n" << hex;

  c_str_test();  // subsequent tests rely on these functions

  return ::boost::report_errors();
}
