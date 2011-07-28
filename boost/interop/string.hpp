//  boost/interop/string.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STRING_HPP)
# define BOOST_INTEROP_STRING_HPP

#include <boost/interop/string_0x.hpp>
#include <boost/interop/conversion_iterator_adapters.hpp>

namespace boost
{
  template<class CharT, class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT> > class basic_string;

  typedef basic_string<char>     istring;     // narrow with native encoding
  typedef basic_string<wchar_t>  iwstring;    // wide with native encoding
  typedef basic_string<u8_t>     iu8string;   // u8_t with UTF-8 encoding
  typedef basic_string<u16_t>    iu16string;  // u16_t with UTF-16 encoding
  typedef basic_string<u32_t>    iu32string;  // u32_t with UTF-32 encoding

    
  template<class CharT, class Traits, class Allocator >
  class basic_string : public std::basic_string<CharT,Traits,Allocator>
  {
  public:
  };
}

#endif  // BOOST_INTEROP_STRING_HPP
