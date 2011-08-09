//  libs/interop/test/iterator_adapter_unit_test.cpp  ----------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/interop/conversion_iterator_adapters.hpp>
#include <boost/interop/string_0x.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::cout; using std::endl; using std::hex;

using namespace boost::interop;
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
//  template <class I, class F, class T>
//  void construct_conversion_iterators(I itr)
//  {
//    //iterator_by_range<I, F, T> itr_r; 
//    //iterator_by_terminator<I, F, T> itr_t;
//    //iterator_by_size<I, F, T> itr_s;
//
//    converting_iterator<I, F, by_null, T> itr_t2(itr);
//
//    cout << "\"";
//    for (;*itr_t2;)
//    {
//      cout << static_cast<char>(*itr_t2);
//      ++itr_t2;
//    }
//    cout << "\"\n";
//
//    converting_iterator<I, F, by_size, T> itr_s2(itr, 2U);
//    cout << "\"";
//    for (;*itr_s2;)
//    {
//      cout << static_cast<char>(*itr_s2);
//      ++itr_s2;
//    }
//    cout << "\"\n";
//
//    converting_iterator<I, F, by_range, T> itr_r2(itr, itr+1); 
//    cout << "\"";
//    for (;*itr_r2;)
//    {
//      cout << static_cast<char>(*itr_r2);
//      ++itr_r2;
//    }
//    cout << "\"\n";
//  }
//
//  void constructor_test()
//  {
//    cout << "constructor test..." << endl;
//
////    construct_conversion_iterators<const char*, char, char>();
////    construct_conversion_iterators<const char*, char, wchar_t>();
//    construct_conversion_iterators<const char*, char, u8_t>("foo");
////    construct_conversion_iterators<const char*, char, u16_t>();
////    construct_conversion_iterators<const char*, char, u32_t>();
//
//    cout << "  constructor test complete" << endl;
//  }

  template <class Iterator>  
  void test(Iterator itr, std::size_t size)
  {
    std::string known_correct("Meow");
    std::size_t i=0;
    for(; *itr; ++itr, ++i)
    {
      BOOST_ASSERT_MSG(i < size, "end should have been reached");
      BOOST_TEST_EQ( static_cast<char>(*itr), known_correct[i]);
    }
    BOOST_TEST_EQ(i, size);
  }

  template <class String, class To>
  void generate_2(const String& str)
  {
    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_null, To>  itr1(str.c_str());

    test(itr1, str.size());

    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_size, To>  itr2(str.c_str(), str.size()-1);

    test(itr2, str.size()-1);

    converting_iterator<typename String::const_iterator,
      typename String::value_type,
      by_range, To>  itr3(str.begin(), str.end()-2);

    //test(itr3, str.size()-2);
  }

  template <class String>
  void generate_1(const String& str)
  {
    // each target type
//    generate_2<String, char>(str);
//    generate_2<String, wchar_t>(str);
    generate_2<String, u8_t>(str);
//    generate_2<String, u16_t>(str);
//    generate_2<String, u32_t>(str);
  }


}  // unnamed namespace

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int, char*[])
{
  cout << "iterator_adapter_unit_test...\n" << hex;

  generate_1(std::string("Meow"));
//  generate_1(std::wstring(L"Meow"));

  return ::boost::report_errors();
}
