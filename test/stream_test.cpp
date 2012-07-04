//  libs/interop/test/stream_test.cpp  -------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using std::cout; using std::endl; using std::hex;
#include <boost/interop/stream.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <string>
#include <sstream>

using namespace boost;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;

namespace
{

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

  template <class T>
  void test_inserter(T x)
  {
    cout << "      test...\n";
    std::stringstream ss;
    std::string result;
    const std::string expected("HelloPipsqueek");

    ss << "Hello";
    ss << x;
    ss << '\n';
    ss >> result;
    BOOST_TEST_EQ(result, expected);
  }

  void stream_inserter_test()
  {
    cout << "  stream inserter test...\n";

    cout << "    standard libary overloads...\n";

    test_inserter(pipsqueek);
    test_inserter("Pipsqueek");
    test_inserter(std::string(pipsqueek));
    test_inserter(std::wstring(wpipsqueek));
//    test_inserter(xop::string(pipsqueek));

    cout << "    boost container overloads in namespace std...\n";

    test_inserter(std::wstring(wpipsqueek));
    test_inserter(boost::u8string(u8pipsqueek));
    test_inserter(boost::u16string(u16pipsqueek));
    test_inserter(boost::u32string(u32pipsqueek));
    //test_inserter(xop::wstring(wpipsqueek));
    //test_inserter(xop::u8string(u8pipsqueek));
    //test_inserter(xop::u16string(u16pipsqueek));
    //test_inserter(xop::u32string(u32pipsqueek));

    cout << "    boost pointer overloads in namespace std...\n";

    test_inserter(L"Pipsqueek");
    test_inserter(wpipsqueek);
    test_inserter(u16pipsqueek);
    test_inserter(u32pipsqueek);

#ifndef BOOST_NO_UNICODE_LITERALS
    test_inserter(u"Pipsqueek");
    test_inserter(U"Pipsqueek");
#endif
  }

}  // unnamed namespace

int cpp_main(int, char*[])
{
  cout << "stream tests...\n" << hex;
  stream_inserter_test();
  //stream_extractor_test();

  return ::boost::report_errors();
}
