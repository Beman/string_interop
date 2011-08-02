//  boost/interop/string.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_STRING_HPP)
# define BOOST_INTEROP_STRING_HPP

#include <boost/interop/string_0x.hpp>
#include <boost/interop/conversion_iterator_adapters.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/decay.hpp>

/*****************************************************************************************

  TODO

  * Every std::basic_string function that returns basic_string& needs to be forwarded
    to get the right return type (boost::basic_string&)

*****************************************************************************************/

namespace boost
{
namespace xop  // short for interoperability
{
  template<class charT, class traits = std::char_traits<charT>,
    class Allocator = std::allocator<charT> > class basic_string;

  typedef basic_string<char>     string;     // narrow with native encoding
  typedef basic_string<wchar_t>  wstring;    // wide with native encoding
  typedef basic_string<u8_t>     u8string;   // u8_t with UTF-8 encoding
  typedef basic_string<u16_t>    u16string;  // u16_t with UTF-16 encoding
  typedef basic_string<u32_t>    u32string;  // u32_t with UTF-32 encoding
    
  template <class T>
  struct is_xop_container { static const bool value = false; };

  template<> struct is_xop_container<boost::xop::string>     { static const bool value = true; };
  template<> struct is_xop_container<boost::xop::wstring>    { static const bool value = true; };
  template<> struct is_xop_container<boost::xop::u8string>   { static const bool value = true; };
  template<> struct is_xop_container<boost::xop::u16string>  { static const bool value = true; };
  template<> struct is_xop_container<boost::xop::u32string>  { static const bool value = true; };

  //---------------------------------  implementation  ---------------------------------//
  
  template<class charT, class traits, class Allocator >
  class basic_string : public std::basic_string<charT,traits,Allocator>
  {
  public:

    //  append  ------------------------------------------------------------------------//

    basic_string& append(const charT* it)
    { 
      cout << "append(const charT*)\n";
      std::basic_string<charT,traits,Allocator>::append(it);
      return *this;
    }

    // needed as an artifact of the over aggressive template <class NTCSIterator>
    basic_string& append(charT* it)
    { 
      cout << "append(charT*)\n";
      std::basic_string<charT,traits,Allocator>::append(it);
      return *this;
    } 

    template <class NTCSIterator>
    typename boost::disable_if<is_xop_container
      <typename boost::decay<NTCSIterator>::type>,
      basic_string&>::type     
    append(NTCSIterator it)
    {
      converting_iterator<NTCSIterator,
        typename std::iterator_traits<NTCSIterator>::value_type,
        typename std::basic_string<charT,traits,Allocator>::value_type>
          converting_it(it);
      for (;;)
      {
        value_type c = *converting_it;
        if (!c)
          break;
        push_back(c);
        ++converting_it;
      }
      return *this;
    }

    template <class Container>
    typename boost::enable_if<is_xop_container
      <typename boost::decay<Container>::type>,
      basic_string&>::type     
    append(const Container& ctr)
    {
      typedef
        converting_iterator<typename Container::const_iterator,
          typename Container::value_type,
          typename std::basic_string<charT,traits,Allocator>::value_type>
        iterator_adapter_type;
      return append(iterator_adapter_type(ctr.cbegin()),
        iterator_adapter_type(ctr.cend()));
    }

    template <class InputIterator>
    basic_string& append(InputIterator first, InputIterator last)
    {
      std::basic_string<charT,traits,Allocator>::append(first, last);
      return *this;
    }

    // copy assign  --------------------------------------------------------------------//

    basic_string& operator=(const basic_string& str)
    {
      std::basic_string<charT,traits,Allocator>::assign(str);
      return *this;
    }
    //basic_string& operator=(basic_string&& str) noexcept;
    basic_string& operator=(const charT* s)
    {
      std::basic_string<charT,traits,Allocator>::assign(s);
      return *this;
    }
    basic_string& operator=(charT* s)
    {
      std::basic_string<charT,traits,Allocator>::assign(s);
      return *this;
    }

    template <class NTCSIterator>
    typename boost::disable_if<is_xop_container
      <typename boost::decay<NTCSIterator>::type>,
      basic_string&>::type     
    operator=(NTCSIterator it)
    {
      clear();
      return append(it);
    }

    template <class Container>
    typename boost::enable_if<is_xop_container
      <typename boost::decay<Container>::type>,
      basic_string&>::type     
    operator=(const Container& ctr)
    {
      clear();
      return append(ctr);
    }

    //basic_string& operator=(charT c);
    //basic_string& operator=(initializer_list<charT>);

    // assign  -------------------------------------------------------------------------//

    basic_string& assign(const basic_string& str)
    {
      std::basic_string<charT,traits,Allocator>::assign(str);
      return *this;
    }
    //basic_string& assign(basic_string&& str) noexcept;
    //basic_string& assign(const basic_string& str, size_type pos, size_type n);
    //basic_string& assign(const charT* s, size_type n);
    basic_string& assign(const charT* s)
    {
      std::basic_string<charT,traits,Allocator>::assign(s);
      return *this;
    }

    basic_string& assign(charT* s)
    {
      std::basic_string<charT,traits,Allocator>::assign(s);
      return *this;
    }

    template <class NTCSIterator>
    typename boost::disable_if<is_xop_container
      <typename boost::decay<NTCSIterator>::type>,
      basic_string&>::type     
    assign(NTCSIterator it)
    {
      clear();
      return append(it);
    }

    template <class Container>
    typename boost::enable_if<is_xop_container
      <typename boost::decay<Container>::type>,
      basic_string&>::type     
    assign(const Container& ctr)
    {
      clear();
      return append(ctr);
    }
    //basic_string& assign(size_type n, charT c);
    template<class InputIterator>
      basic_string& assign(InputIterator first, InputIterator last);
    //basic_string& assign(initializer_list<charT>);


  };
}  // namespace xop
}  // namespace boost

#endif  // BOOST_INTEROP_STRING_HPP
