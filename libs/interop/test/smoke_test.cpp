//  libs/interop/test/smoke_test.cpp  --------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/interop/string.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

//  name clashes between the standard library and this library are not desirable because
//  interoperability is critical, so provide using namespaces for std and boost to detect
//  such clashes
using namespace std;
using namespace boost;

int cpp_main(int, char*[])
{
  istring si;   
  iwstring siw;  
  iu8string siu8; 
  iu16string siu16;
  iu32string siu32;

  si.append("si");
  BOOST_TEST_EQ(si.size(), 2U);
  siw.append(L"siw");
  BOOST_TEST_EQ(siw.size(), 3U);
  u8_t csu8[] = {'s', 'i', 'u', '8', '\0'};
  siu8.append(csu8);
  BOOST_TEST_EQ(siu8.size(), 4U);
  u16_t csu16[] = {'s', 'i', 'u', '1', '6', '\0'};
  siu16.append(csu16);
  BOOST_TEST_EQ(siu16.size(), 5U);
  u32_t csu32[] = {'s', 'i', 'u', '3', '2', '\0'};
  siu32.append(csu32);
  BOOST_TEST_EQ(siu32.size(), 5U);

  return ::boost::report_errors();
}
