//  boost/libs/interop/example/sweet_dreams.cpp  ---------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_XOP)
# include <string>
  using std::wstring;
#else
# include <boost/interop/string.hpp>
  using boost::xop::wstring;
#endif
#include <iostream>

int main()
{
  std::cout << "Sweet dreams!\n";
  wstring wstr(L"\u00A1Dulces sue\u00F1os!\n");
  std::cout << wstr;
}