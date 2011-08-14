//  libs/interop/test/smoke_test.cpp  --------------------------------------------------//

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
//
//
//  struct xappend
//  {
//    template <class T, class U>
//    void operator()(T& str, U u) const {str.append(u);}
//    template <class T, class U, class V>
//    void operator()(T& str, U u, V v) const {str.append(u, v);}
//    template <class T, class U, class V, class W>
//    void operator()(T& str, U u, V v, W w) const {str.append(u, v, w);}
//  };
//
//  struct xassign
//  {
//    template <class T, class U> 
//    void operator()(T& str, U arg) const { str = arg; }
//  };
//
//  template <class T, class U, class Func> // T must be a xop::basic_string
//  void test(const T& obj, U u, const Func& func)
//  // u is an argument to be applied to a
//  // copy of obj by func
//  {
//    T tmp;
//    // use copy in case construct, cctor, copy assign don't work right yet
//    std::copy(obj.begin(), obj.end(), std::back_inserter(tmp));
//    func(tmp, u);
//    std::basic_string<typename T::value_type> xop_result
//      (tmp.c_str<std::basic_string<typename T::value_type> >());
//
//    std::basic_string<typename T::value_type> std_result
//      (obj.c_str<std::basic_string<typename T::value_type> >());
//    func(std_result, u);
//
//    BOOST_TEST_EQ(xop_result, std_result); 
//  }
//
//  //template <class T, class U, class V, class Func>
//  //void test(const T& obj, U u, V v, const Func& func)
//  //// obj must be a xop::basic_string, u and v are arguments to be applied to a
//  //// copy of obj by func
//  //{
//  //  T xop_result;
//  //  std::copy(obj.begin(), obj.end(), std::back_inserter(xop_result));
//  //  func(xop_result, u, v);
//  //  std::basic_string<typename T::value_type> result(xop_result.c_str());
//
//  //  // perform the function on std::basic_string arguments, and then
//  //  // use that result as the expected result.
//  //  std::basic_string<typename T::value_type> expected = obj;
//  //  //xop::basic_string<typename T::value_type> tmp = u;  // convert if needed
//  //  //std::basic_string<typename T::value_type> argument = tmp;
//  //  func(expected, u, v);
//  //  BOOST_TEST(result == expected); 
//  //}
//
//  //template <class T, class U, class Func>
//  //void call_test(const T& obj, const U& arg)
//  //{
//  //  test(obj, arg, Func());
//  //  test(obj, arg.c_str(), Func());
//
//  //  test(obj, 3, arg[0], Func());
//  //}
//
//  template <class T, class U> // T must be a xop::basic_string
//                              // U must be a std::basic_string
//  void generate2(const T& obj, const U& arg)
//  {
////    call_test<T, U, xappend>(obj, arg);
////    call_test<T, U, xassign>(obj, arg);
//
//    test(obj, arg, xappend());
//    test(obj, arg.c_str(), xappend());
////    test(obj, 2, arg.c_str(), xappend());
//
//  }
//
//  template <class T>  // T must be a xop::basic_string
//  void generate1(const T& obj)
//  {
//    u8_t  u8src[]  = { 'M', 'e', 'o', 'w', 0 };
//    u16_t u16src[] = { 'M', 'e', 'o', 'w', 0 };
//    u32_t u32src[] = { 'M', 'e', 'o', 'w', 0 };
//
//    generate2(obj, std::string("Meow"));
//    generate2(obj, std::wstring(L"Meow"));
//    generate2(obj, std::basic_string<u8_t>(u8src));
//    generate2(obj, std::u16string(u16src));
//    generate2(obj, std::u32string(u32src));
//  }
//

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

  template <class T, class U>  // T must be xop:basic_string
                               // U may be xop:basic_string or std::basic_string
  void append_test2(const T& t, const U& u)
  {
    std::basic_string<typename U::value_type> expected;
    for (auto itr = t.c_str<typename U::value_type>(); *itr; ++itr)
      expected.push_back(*itr);
    expected.append(u.c_str());

    T tmp(t);
    tmp.append(u);
    std::basic_string<typename U::value_type> actual;
    for (auto itr = tmp.c_str<typename U::value_type>(); *itr; ++itr)
      actual.push_back(*itr);
    BOOST_TEST(actual == expected);
  }

  template<class T>
  void append_test1(const T& t)
  {
    append_test2(t, xop::string(meow));
    append_test2(t, xop::wstring(wmeow));
    append_test2(t, xop::u8string(u8meow));
    append_test2(t, xop::u16string(u16meow));
    append_test2(t, xop::u32string(u32meow));
    append_test2(t, std::string(meow));
    append_test2(t, std::wstring(wmeow));
    append_test2(t, boost::u8string(u8meow));
    append_test2(t, boost::u16string(u16meow));
    append_test2(t, boost::u32string(u32meow));
  }

  void append_test()
  {
    cout << "appends test...\n";

    append_test1(xop::string(pipsqueek));
    append_test1(xop::wstring(wpipsqueek));
    append_test1(xop::u8string(u8pipsqueek));
    append_test1(xop::u16string(u16pipsqueek));
    append_test1(xop::u32string(u32pipsqueek));
  }

  template <class T, class U>
  void constructor_test2(const U& s)
  {
    std::basic_string<typename U::value_type> expected(s.c_str());

    typedef xop::basic_string<T> string_type;

    string_type x1;
    BOOST_TEST_EQ(x1.size(), 0U);

    string_type x2(s);
    std::basic_string<typename U::value_type> actual;
    for (auto itr = x2.c_str<typename U::value_type>(); *itr; ++itr)
      actual.push_back(*itr);
    BOOST_TEST(actual == expected);

    string_type x3(x2);
    actual.clear();
    for (auto itr = x3.c_str<typename U::value_type>(); *itr; ++itr)
      actual.push_back(*itr);
    BOOST_TEST(actual == expected);

    string_type x4(expected);
    actual.clear();
    for (auto itr = x4.c_str<typename U::value_type>(); *itr; ++itr)
      actual.push_back(*itr);
    BOOST_TEST(actual == expected);

    string_type x5(expected.begin(), expected.end());
    actual.clear();
    for (auto itr = x5.c_str<typename U::value_type>(); *itr; ++itr)
      actual.push_back(*itr);
    BOOST_TEST(actual == expected);

  }

  template<class U>
  void constructor_test1(const U& u)
  {
    constructor_test2<char>(u);
    constructor_test2<wchar_t>(u);
    constructor_test2<u8_t>(u);
    constructor_test2<u16_t>(u);
    constructor_test2<u32_t>(u);
  }

  void constructor_test()
  {
    cout << "constructors test...\n";

    constructor_test1(xop::string(pipsqueek));
    constructor_test1(xop::wstring(wpipsqueek));
    constructor_test1(xop::u8string(u8pipsqueek));
    constructor_test1(xop::u16string(u16pipsqueek));
    constructor_test1(xop::u32string(u32pipsqueek));
  }

  void misc_functions_test()
  {
    cout << "miscellaneous functions test...\n";

    xop::wstring wstr(L"Meow\n");

    std::string str;

    for (auto itr=wstr.c_str<char>(); *itr; ++itr)
      str.push_back(*itr);

    BOOST_TEST_EQ(str, std::string("Meow\n"));
  }

}  // unnamed namespace

int cpp_main(int, char*[])
{
  cout << "unit test test...\n" << hex;

  misc_functions_test();  // subsequent tests rely on these functions
  constructor_test();
  //append_test();

  //cout << "----------------  test with xop::string object  ----------------\n";
  //generate1(xop::string("Pipsqueek"));

  //cout << "----------------  test with xop::wstring object  ----------------\n";
  //generate1(xop::wstring(L"Pipsqueek"));

  //cout << "----------------  test with xop::u8string object  ----------------\n";
  //generate1(xop::u8string(u8src)); 

  //cout << "----------------  test with xop::u16string object  ----------------\n";
  //generate1(xop::u16string(u16src)); 

  //cout << "----------------  test with xop::u32string object  ----------------\n";
  //generate1(xop::u32string(u32src)); 

  return ::boost::report_errors();
}
