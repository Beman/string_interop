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
#include <string>
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

  std::string smeow(meow);
  std::wstring swmeow(wmeow);
  u8string su8meow(u8meow);
  u16string su16meow(u16meow);
  u32string su32meow(u32meow);

  void c_str_test()
  {
    cout << "  c_str() template test...\n";

    xop::wstring wstr(L"Meow\n");

    std::string str;

    for (auto itr=wstr.c_str<char>(); *itr; ++itr)
      str.push_back(*itr);

    BOOST_TEST_EQ(str, std::string("Meow\n"));
  }

  void construct_without_conversion_test()
  {
    cout << "  construct_without_conversion test...\n";

    const char catfood[] = "catfood";

    xop::wstring s0;
    BOOST_TEST(s0.empty());
    xop::string s1("catfood");
    BOOST_TEST_EQ(s1.size(), 7);
    BOOST_TEST(std::strcmp(s1.c_str(), "catfood") == 0);
    xop::string s2(s1);
    BOOST_TEST_EQ(s2.size(), 7);
    BOOST_TEST_EQ(s1, s2);
    xop::string s3(catfood, catfood+sizeof(catfood)-1);
    BOOST_TEST_EQ(s3.size(), 7);
    BOOST_TEST_EQ(s3, s2);

    s3.clear();
    BOOST_TEST(s3.empty());
  }

  void append_test()
  {
    cout << "  append test...\n";

    const char catfood[] = "catfood";
    const wchar_t wcatfood[] = L"catfood";

    //  signatures tested in the order they appear in the standard [string.modifiers]

    xop::string foo("foo");
    foo += xop::string("bar");
    BOOST_TEST_EQ(foo, xop::string("foobar"));
    foo += foo;  // test self append via operator +=
    BOOST_TEST_EQ(foo, xop::string("foobarfoobar"));

    foo += "bar";
    BOOST_TEST_EQ(foo, xop::string("foobarfoobarbar"));

    foo += '!';
    BOOST_TEST_EQ(foo, xop::string("foobarfoobarbar!"));

    BOOST_TEST_EQ(xop::string().append(xop::string("Meow")), xop::string("Meow"));

    BOOST_TEST_EQ(xop::string().append(xop::string("Meow?"), 1, 3), xop::string("eow"));

    BOOST_TEST_EQ(xop::string().append("Meow!!", 4), xop::string("Meow"));

    BOOST_TEST_EQ(xop::string().append("Meow"), xop::string("Meow"));

    BOOST_TEST_EQ(xop::string("Meow").append(3, '!'), xop::string("Meow!!!"));

    BOOST_TEST_EQ(xop::string().append(catfood, catfood+sizeof(catfood)-1),
      xop::string("catfood"));
 
    xop::string meow0("Meow");
    meow0.push_back('!');
    BOOST_TEST_EQ(meow0, xop::string("Meow!"));

    // converting appends

    BOOST_TEST_EQ(xop::string().append(xop::wstring(L"Meow")), xop::string("Meow"));
 
    BOOST_TEST_EQ(xop::string().append(xop::wstring(L"Meow?"), 1, 3), xop::string("eow"));

    BOOST_TEST_EQ(xop::string().append(L"Meow!!", 4), xop::string("Meow"));

    BOOST_TEST_EQ(xop::string().append(L"Meow"), xop::string("Meow"));

    BOOST_TEST_EQ(xop::string("Meow").append(3, L'!'), xop::string("Meow!!!"));

    BOOST_TEST_EQ(xop::string().append(wcatfood,
      wcatfood+sizeof(wcatfood)/sizeof(wchar_t)-1), xop::string("catfood"));

    xop::string meow1("Meow");
    meow1.push_back(L'!');
    BOOST_TEST_EQ(meow1, xop::string("Meow!"));
  }

  void construct_with_conversion_test()
  {
    cout << "  construct_with_conversion test...\n";
  }

  void assignment_operator_test()
  {
    cout << "  assignment_operator test...\n";

    xop::string s0("bar");
    xop::string s1("foo");
    xop::wstring ws1(L"foo");

    BOOST_TEST_EQ(s0 = s1, xop::string("foo"));
    BOOST_TEST_EQ(s0 = s0, xop::string("foo"));  // test self assignment

    BOOST_TEST_EQ(s0 = "blah", xop::string("blah"));

    BOOST_TEST_EQ(s0 = '!', xop::string("!"));

    BOOST_TEST_EQ(s0 = ws1, xop::string("foo"));

    BOOST_TEST_EQ(s0 = L"blah", xop::string("blah"));

    BOOST_TEST_EQ(s0 = L'!', xop::string("!"));
  }


  void assign_test()
  {
    cout << "  assign test...\n";

    xop::string s0("bar");
    xop::string s1("foo!");
    xop::wstring ws1(L"foo!");
    const char catfood[] = "catfood";
    const wchar_t wcatfood[] = L"catfood";
 
    BOOST_TEST_EQ(s0.assign(s1), xop::string("foo!"));
    BOOST_TEST_EQ(s0 = s0, xop::string("foo!"));  // test self assignment

    BOOST_TEST_EQ(s0.assign(s1, 1, 2), xop::string("oo"));

    BOOST_TEST_EQ(s0.assign("blob!", 4), xop::string("blob"));

    BOOST_TEST_EQ(s0.assign("fob!"), xop::string("fob!"));

    BOOST_TEST_EQ(s0.assign(5, '*'), xop::string("*****"));

    BOOST_TEST_EQ(s0.assign(catfood, catfood+sizeof(catfood)-1),
      xop::string("catfood"));

    BOOST_TEST_EQ(s0.assign(ws1), xop::string("foo!"));

    BOOST_TEST_EQ(s0.assign(ws1, 1, 2), xop::string("oo"));

    BOOST_TEST_EQ(s0.assign(L"blob!", 4), xop::string("blob"));

    BOOST_TEST_EQ(s0.assign(L"fob!"), xop::string("fob!"));

    BOOST_TEST_EQ(s0.assign(5, L'*'), xop::string("*****"));

    BOOST_TEST_EQ(s0.assign(wcatfood, wcatfood+sizeof(wcatfood)/sizeof(wchar_t)-1),
      xop::string("catfood"));
  }

  bool arg_cvt_test(const xop::string& str)
  {
    return str == xop::string("Meow");
  }

  void argument_with_conversion_test()
  {
    cout << "  argument_with_conversion test...\n";

    arg_cvt_test("Meow");
    arg_cvt_test(L"Meow");
# ifndef BOOST_NO_UNICODE_LITERALS
    arg_cvt_test(u8"Meow");
    arg_cvt_test(u"Meow");
    arg_cvt_test(U"Meow");
# endif
    arg_cvt_test(meow);
    arg_cvt_test(wmeow);
    arg_cvt_test(u8meow);
    arg_cvt_test(u16meow);
    arg_cvt_test(u32meow);

    arg_cvt_test(smeow);
    arg_cvt_test(swmeow);
    arg_cvt_test(su8meow);
    arg_cvt_test(su16meow);
    arg_cvt_test(su32meow);
  }

}  // unnamed namespace

int cpp_main(int, char*[])
{
  cout << "string interoperability unit test...\n" << hex;

  c_str_test();  // subsequent tests rely on this working

  construct_without_conversion_test();
  append_test();  // also covers push_back() and operator+=()
  //construct_with_conversion_test();
  assignment_operator_test();
  assign_test();
  argument_with_conversion_test();
  //  insert?
  //  erase?
  //  replace?
  //  iterators

  return ::boost::report_errors();
}
