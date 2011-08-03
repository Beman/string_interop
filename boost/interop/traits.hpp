//  boost/interop/traits.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_TRAITS_HPP)
#define BOOST_INTEROP_TRAITS_HPP

#include <boost/interop/string_0x.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/integral_constant.hpp>

namespace boost
{
namespace interop
{
  //--------------------------------  is_character  ------------------------------------//

  template<class T> class is_character        : public boost::false_type {};

  template<> class is_character<char>         : public boost::true_type {}; 
  template<> class is_character<wchar_t>      : public boost::true_type {}; 
  template<> class is_character<boost::u8_t>  : public boost::true_type {}; 
  template<> class is_character<boost::u16_t> : public boost::true_type {}; 
  template<> class is_character<boost::u32_t> : public boost::true_type {};

  //---------------------------  is_character_container  -------------------------------//

  template<class T> class is_character_container            : public boost::false_type {};

  template<> class is_character_container<boost::string>    : public boost::true_type {}; 
  template<> class is_character_container<boost::wstring>   : public boost::true_type {}; 
  template<> class is_character_container<boost::u8string>  : public boost::true_type {}; 
  template<> class is_character_container<boost::u16string> : public boost::true_type {}; 
  template<> class is_character_container<boost::u32string> : public boost::true_type {};

  //--------------------------------  is_character_pointer  ---------------------------------//

  template<class T> class is_character_iterator         : public boost::false_type {};

  template<> class is_character_iterator<*char>         : public boost::true_type {}; 
  template<> class is_character_iterator<*wchar_t>      : public boost::true_type {}; 
  template<> class is_character_iterator<*boost::u8_t>  : public boost::true_type {}; 
  template<> class is_character_iterator<*boost::u16_t> : public boost::true_type {}; 
  template<> class is_character_iterator<*boost::u32_t> : public boost::true_type {};

  template<> class is_character_iterator<boost::string::iterator>    : public boost::true_type {}; 
  template<> class is_character_iterator<boost::wstring::iterator>   : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u8string::iterator>  : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u16string::iterator> : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u32string::iterator> : public boost::true_type {};

  template<> class is_character_iterator<boost::string::const_iterator>    : public boost::true_type {}; 
  template<> class is_character_iterator<boost::wstring::const_iterator>   : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u8string::const_iterator>  : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u16string::const_iterator> : public boost::true_type {}; 
  template<> class is_character_iterator<boost::u32string::const_iterator> : public boost::true_type {};

}
}

# endif  // BOOST_INTEROP_TRAITS_HPP
