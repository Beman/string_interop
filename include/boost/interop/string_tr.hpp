//  boost/interop/string_tr.hpp  -------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//           Experimental string extentions for the C++11 standard library              //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                              Header <cuchar> support                                 //
//                                                                                      //
//  Very few C++11 standard library implementations support <cuchar>, so for now        //
//  declare the contents here.                                                          //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#define  __STDC_UTF_16__
#define  __STDC_UTF_32__

namespace std
{
  std::size_t mbrtoc16(char16_t* pc16,
    const char* s, size_t n,
    std::mbstate_t* ps);

  std::size_t c16rtomb(char* s, char16_t c16,
    std::mbstate_t* ps);

  std::size_t mbrtoc32(char32_t* pc32,
    const char* s, std::size_t n,
    std::mbstate_t* ps);

  std::size_t c32rtomb(char* s, std::char32_t c32,
    std::mbstate_t* ps);
}


