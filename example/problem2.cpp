//  libs/interop/example/problem2.cpp  -------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <boost/interop/stream.hpp>

int main()
{
  std::u32string s32(U"您好世界");  // Hello World

  std::cout << U"您好世界" << std::endl;
  std::cout << s32 << std::endl;
  std::cout << s32.c_str() << std::endl;

  return 0;
} 
