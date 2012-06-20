//  interop/testmake_string_test.cpp  --------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <boost/interop/make_string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using namespace boost;
using namespace boost::interop;

namespace
{
  //  Test with cases that require UTF-16 surrogate pairs
  //  U+1F60A SMILING FACE WITH SMILING EYES
  //  U+1F60E SMILING FACE WITH SUNGLASSES

  u32_t utf32[] = {0x1F60A, 0x1F60E, 0};
  u16_t utf16[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
  u8_t  utf8[] = {0xF0, 0x9F, 0x98, 0x8A, 0xF0, 0x9F, 0x98, 0x8E, 0};

//------------------------------------ simple_test -------------------------------------//

  void simple_test()
  {
    std::cout << "simple_test..." << std::endl;

    std::string s = make_string<std::string>(u32string(utf32));
    BOOST_TEST_EQ(s.size(), 8);
  }

}

//--------------------------------------------------------------------------------------//
//                                         main                                         //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{
  simple_test();

  return ::boost::report_errors();
}
