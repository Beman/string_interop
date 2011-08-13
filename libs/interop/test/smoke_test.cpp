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

using namespace boost;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;

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


  //template <class String, class To>
  //void generate_2(const String& str)
  //{
  //  converting_iterator<const typename String::value_type*,
  //    typename String::value_type,
  //    by_null, To>  itr1(str.c_str());

  //  test(itr1, str.size());

  //  converting_iterator<const typename String::value_type*,
  //    typename String::value_type,
  //    by_size, To>  itr2(str.c_str(), str.size()-1);

  //  test(itr2, str.size()-1);

  //  converting_iterator<typename String::const_iterator,
  //    typename String::value_type,
  //    by_range, To>  itr3(str.begin(), str.end()-2);

  //  test(itr3, str.size()-2);
  //}

  //template <class String>
  //void generate_1(const String& str)
  //{
  //  // each target type
  //  generate_2<String, char>(str);
  //  generate_2<String, wchar_t>(str);
  //  generate_2<String, u8_t>(str);
  //  generate_2<String, u16_t>(str);
  //  generate_2<String, u32_t>(str);
  //}




  struct xappend
  {
    template <class T, class U>
    void operator()(T& str, U arg) const
    {
      cout << "        xappend str.append(arg)" << std::endl;
      str.append(arg);
      cout << "        done" << std::endl;
    }
  };
  struct xassign
  {
    template <class T, class U> 
    void operator()(T& str, U arg) const { str = arg; }
  };

  template <class T, class U, class Func>
  void test(const T& obj, U x, const Func& func)
  // obj must be a xop::basic_string, x is an argument to be applied to a
  // copy of obj by func, and expected is the expected result
  {
    T xop_result;
    std::copy(obj.begin(), obj.end(), std::back_inserter(xop_result));
    func(xop_result, x);
    std::basic_string<typename T::value_type> result(xop_result.c_str());

    // perform the function on std::basic_string arguments, and then
    // use that result as the expected result.
    std::basic_string<typename T::value_type> expected = obj;
    xop::basic_string<typename T::value_type> tmp = x;  // convert if needed
    std::basic_string<typename T::value_type> argument = tmp;
    func(expected, argument);
    BOOST_TEST_EQ(result, expected); 
  }

  template <class T, class U, class Func>
  void call_test(const T& obj, const U& arg)
  {
    cout << "      argument is object\n";
    test(obj, arg, Func());
    cout << "      done" << std::endl;
    //cout << "      argument is object.c_str()...\n";
    //test(obj, arg.c_str(), Func());
    //cout << "      done" << std::endl;
  }

  template <class T, class U>
  void generate2(const T& obj, const U& arg)
  {
    cout << "    operation append\n";
    call_test<T, U, xappend>(obj, arg);
    cout << "    done" << std::endl;
//    call_test<T, U, xassign>(obj, arg);
  }

  template <class T>
  void generate1(const T& obj)
  {
    u8_t  u8src[]  = { 'M', 'e', 'o', 'w', 0 };
    u16_t u16src[] = { 'M', 'e', 'o', 'w', 0 };
    u32_t u32src[] = { 'M', 'e', 'o', 'w', 0 };

    //cout << "  argument souce xop::string...\n";
    //generate2(obj, xop::string("Meow"));
    cout << "  argument source xop::wstring\n";
    generate2(obj, xop::wstring(L"Meow"));
    cout << "  done" << std::endl;
    //generate2(obj, xop::u8string(u8src));
    //generate2(obj, xop::u16string(u16src));
    //generate2(obj, xop::u32string(u32src));

    //generate2(obj, std::string("Meow"));
    //generate2(obj, std::wstring(L"Meow"));
    //generate2(obj, std::u8string(u8src));
    //generate2(obj, std::u16string(u16src));
    //generate2(obj, std::u32string(u32src));
  }


}  // unnamed namespace

int cpp_main(int, char*[])
{
  cout << "smoke test...\n" << hex;

  cout << "test with xop::string object\n";
  generate1(xop::string("Pipsqueek")); 
  cout << "done" << std::endl;

  return ::boost::report_errors();
}
