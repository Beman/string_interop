//  boost cxx11_char_types.hpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  The purpose of this header is to emulate the C++11 char16_t and char32_t            //
//  character and string types so that they can be used in both C++11 and C++03         //
//  programs.                                                                           //
//                                                                                      //
//  The C++11 character type emulation names are at global scope. The C++ standard      //
//  library string types are in namespace std.                                          //
//                                                                                      //
//  This approach was controversial when discussed on the boost list, and so the        //
//  initial version placed names in namespace boost, similar to the approach taken      //
//  header <boost/cstdint.hpp>, and used char16_t/char32_t rather than char16_t/char32_t    //
//  to avoid use of names that are keywords in C++11.                                   //
//                                                                                      //
//  As time passed, and C++11 features became widely available, that approach began to  //
//  look dated. It also meant that source code using the emulation was harder to read   //
//  and write than necessary. Hopefully a more C++11 friendly approach is now less      //
//  controversial.                                                                      //
//                                                                                      //
//   Uses the typedefs provided by Microsoft Visual C++ 2010 and later if present.      //
//                                                                                      //
//   Thanks to Mathias Gaunard and others for discussions illuminating the issues.      //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_CXX11_CHAR_TYPES_HPP)
# define BOOST_CXX11_CHAR_TYPES_HPP

# include <boost/config.hpp>
# include <boost/cstdint.hpp>
# include <string>

# if defined(BOOST_NO_CHAR16_T) && (!defined(_MSC_VER) || _MSC_VER < 1600)  // 1600 == VC++10 
    typedef boost::uint_least16_t  char16_t;
    namespace std { typedef std::basic_string<char16_t>  u16string; }
# endif

# if defined(BOOST_NO_CHAR32_T) && (!defined(_MSC_VER) || _MSC_VER < 1600)  // 1600 == VC++10 
    typedef boost::uint_least32_t  char32_t;
    namespace std { typedef std::basic_string<char32_t>  u32string; }
# endif

#endif  // !defined(BOOST_CXX11_CHAR_TYPES_HPP)
