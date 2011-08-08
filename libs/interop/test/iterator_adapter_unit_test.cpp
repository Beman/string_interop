//  libs/interop/test/iterator_adapter_unit_test.cpp  ----------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/interop/string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::cout; using std::endl; using std::hex;

using namespace boost::xop;
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

  template <class I, class F, class T>
  void construct_conversion_iterators(I itr)
  {
    //iterator_by_range<I, F, T> itr_r; 
    //iterator_by_terminator<I, F, T> itr_t;
    //iterator_by_size<I, F, T> itr_s;

//    iterator_by_range<I, F, T> itr_r2(itr, itr); 
    iterator_by_terminator<I, F, T> itr_t2(itr);
//    iterator_by_size<I, F, T> itr_s2(itr, std::size_t(0));
  }

  void constructor_test()
  {
    cout << "constructor test..." << endl;



//    construct_conversion_iterators<const char*, char, char>();
//    construct_conversion_iterators<const char*, char, wchar_t>();
    construct_conversion_iterators<const char*, char, u8_t>("foo");
//    construct_conversion_iterators<const char*, char, u16_t>();
//    construct_conversion_iterators<const char*, char, u32_t>();

    cout << "  constructor test complete" << endl;
  }


}  // unnamed namespace

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int, char*[])
{
  cout << "iterator_adapter_unit_test...\n" << hex;

  constructor_test();

  return ::boost::report_errors();
}
