//  boost/interop/string.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STRING_HPP)
# define BOOST_INTEROP_STRING_HPP

#include <boost/interop/string_0x.hpp>
#include <boost/interop/conversion_iterator_adapters.hpp>
#include <boost/interop/traits.hpp>

/*****************************************************************************************

  TODO

  * Every std::basic_string function that returns basic_string& needs to be forwarded
    to get the right return type (boost::basic_string&)

*****************************************************************************************/

namespace boost
{
namespace xop  // short for interoperability
{
  template<class CharT, class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT> > class basic_string;

  typedef basic_string<char>     string;     // narrow with native encoding
  typedef basic_string<wchar_t>  wstring;    // wide with native encoding
  typedef basic_string<u8_t>     u8string;   // u8_t with UTF-8 encoding
  typedef basic_string<u16_t>    u16string;  // u16_t with UTF-16 encoding
  typedef basic_string<u32_t>    u32string;  // u32_t with UTF-32 encoding
    
  template<class CharT, class Traits, class Allocator >
  class basic_string : public std::basic_string<CharT,Traits,Allocator>
  {
  public:

    basic_string& new_append(const CharT* it)
    { 
      cout << "new_append (const CharT*)\n";
      append(it);
      return *this;
    } 

    template <class NTCSIterator>
    basic_string& new_append(NTCSIterator it)
    {
      converting_iterator<NTCSIterator,
        typename std::iterator_traits<NTCSIterator>::value_type,
        typename std::basic_string<CharT,Traits,Allocator>::value_type>
          converting_it(it);
      for (; *converting_it; ++converting_it)
      {
        push_back(*converting_it);
      }
      return *this;
    }

    //template <class NTCSIterator>
    //  typename boost::disable_if<boost::is_same<CharT*,
    //    typename boost::remove_const<NTCSIterator>::type>, basic_string&>::type
    //append(NTCSIterator it)
    //{
    //  std::cout << "********\n";
    //  //return append(converting_iterator<NTCSIterator,
    //  //  std::iterator_traits<NTCSIterator>::value_type, value_type>(it));
    //  return *this;
    //}
  };
}  // namespace xop
}  // namespace boost

#endif  // BOOST_INTEROP_STRING_HPP
