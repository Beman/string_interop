//  libs/interop/test/iterator_adapter_unit_test.cpp  ----------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/interop/iterator_adapter.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::cout; using std::endl; using std::hex;

using namespace boost::interop;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;

namespace
{
  template <class ImplicitEndIterator>  
  void implicit_end_iterator_test(ImplicitEndIterator itr, std::size_t size)
  {
    std::string known_correct("Meow");

    std::size_t i=0;
    for(; itr != ImplicitEndIterator(); ++itr, ++i)
    {
      if (i >= size)
      {
        cout << "failed to detect end iterator" << endl;
        break;
      }
    }
    BOOST_TEST_EQ(i, size);
  }

  const char* meow = "Meow";
  const wchar_t meoww[] = { 'M', 'e', 'o', 'w', 0 };
  const u8_t meow8[] = { 'M', 'e', 'o', 'w', 0 };
  const u16_t meow16[] = { 'M', 'e', 'o', 'w', 0 };
  const u32_t meow32[] = { 'M', 'e', 'o', 'w', 0 };

  void to32_iterator_test()
  {
    cout << "to32_iterator_test..." << endl;

    cout << "  char" << endl;
    implicit_end_iterator_test(to32_iterator<const char*, char, by_null>(meow), 4);
    implicit_end_iterator_test(to32_iterator<const char*, char, by_size>(meow, 3), 3);
    implicit_end_iterator_test(to32_iterator<const char*, char, by_range>(meow, meow+2), 2);
    implicit_end_iterator_test(to32_iterator<const char*, char, by_null>(meow+4), 0);
    implicit_end_iterator_test(to32_iterator<const char*, char, by_size>(meow, 0), 0);
    implicit_end_iterator_test(to32_iterator<const char*, char, by_range>(meow, meow), 0);

    cout << "  u8_t" << endl;
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_null>(meow8), 4);
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_size>(meow8, 3), 3);
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_range>(meow8, meow8+2), 2);
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_null>(meow8+4), 0);
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_size>(meow8, 0), 0);
    implicit_end_iterator_test(to32_iterator<const u8_t*, u8_t, by_range>(meow8, meow8), 0);

    cout << "  u16_t" << endl;
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_null>(meow16), 4);
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_size>(meow16, 3), 3);
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_range>(meow16, meow16+2), 2);
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_null>(meow16+4), 0);
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_size>(meow16, 0), 0);
    implicit_end_iterator_test(to32_iterator<const u16_t*, u16_t, by_range>(meow16, meow16), 0);

    cout << "  u32_t" << endl;
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_null>(meow32), 4);
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_size>(meow32, 3), 3);
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_range>(meow32, meow32+2), 2);
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_null>(meow32+4), 0);
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_size>(meow32, 0), 0);
    implicit_end_iterator_test(to32_iterator<const u32_t*, u32_t, by_range>(meow32, meow32), 0);

    //cout << "  wchar_t" << endl;
    //implicit_end_iterator_test(to32_iterator<const wchar_t*, wchar_t, by_null>(meoww), 4);
  }

  void from32_iterator_test()
  {
    cout << "from32_iterator_test..." << endl;

    cout << "  char" << endl;
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, char>(
        to32_iterator<const char*, char, by_null>(meow)), 4);
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, char>(
        to32_iterator<const char*, char, by_null>(meow+4)), 0);

    cout << "  u8_t" << endl;
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u8_t>(
        to32_iterator<const char*, char, by_null>(meow)), 4);
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u8_t>(
        to32_iterator<const char*, char, by_null>(meow+4)), 0);

    cout << "  u16_t" << endl;
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u16_t>(
        to32_iterator<const char*, char, by_null>(meow)), 4);
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u16_t>(
        to32_iterator<const char*, char, by_null>(meow+4)), 0);

    cout << "  u32_t" << endl;
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u32_t>(
        to32_iterator<const char*, char, by_null>(meow)), 4);
    implicit_end_iterator_test(
      from32_iterator<to32_iterator<const char*, char, by_null>, u32_t>(
        to32_iterator<const char*, char, by_null>(meow+4)), 0);
  }

  template <class String, class To>
  void generate_2(const String& str)
  {
    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_null, To>  itr1(str.c_str());

    implicit_end_iterator_test(itr1, str.size());

    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_size, To>  itr2(str.c_str(), str.size()-1);

    implicit_end_iterator_test(itr2, str.size()-1);

    converting_iterator<typename String::const_iterator,
      typename String::value_type,
      by_range, To>  itr3(str.begin(), str.end()-2);

    implicit_end_iterator_test(itr3, str.size()-2);
  }

  template <class String>
  void generate_1(const String& str)
  {
    // each target type
    //generate_2<String, char>(str);
    //generate_2<String, wchar_t>(str);
    //generate_2<String, u8_t>(str);
    //generate_2<String, u16_t>(str);
    //generate_2<String, u32_t>(str);
  }

  void value_tests()
  {
    cout << "value_tests..." << endl;

    //  Test with cases that require UTF-16 surrogate pairs
    //  U+1F60A SMILING FACE WITH SMILING EYES
    //  U+1F60E SMILING FACE WITH SUNGLASSES

    u32_t utf32[] = {0x1F60A, 0x1F60E, 0};
    u16_t utf16[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
    u8_t  utf8[] = {0xF0, 0x9F, 0x98, 0x8A, 0xF0, 0x9F, 0x98, 0x8E, 0};

    //  utf-32 to utf-16
    int i = 0;
    typedef converting_iterator<const u32_t*, u32_t, by_null, u16_t> type_32_16;
    for (type_32_16 it(utf32); it != type_32_16(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf16[i]);
    BOOST_TEST_EQ(i, 4);

    // utf-32 to utf-8
    i = 0;
    typedef converting_iterator<const u32_t*, u32_t, by_null, u8_t> type_32_8;
    for (type_32_8 it(utf32); it != type_32_8(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf8[i]);
    BOOST_TEST_EQ(i, 8);

    // utf-8 to utf-16, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef converting_iterator<const u8_t*, u8_t, by_null, u16_t> type_8_16;
    for (type_8_16 it(utf8); it != type_8_16(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf16[i]);
    BOOST_TEST_EQ(i, 4);

    // utf-16 to utf-8, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef converting_iterator<const u16_t*, u16_t, by_null, u8_t> type_16_8;
    for (type_16_8 it(utf16); it != type_16_8(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf8[i]);
    BOOST_TEST_EQ(i, 8);
  }

}  // unnamed namespace

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int, char*[])
{
  cout << "iterator_adapter_unit_test...\n" << hex;

  to32_iterator_test();
  from32_iterator_test();

  cout << "-----------------  testing with char...  -----------------" << endl;
  generate_1(std::string("Meow"));

  //cout << "-----------------  testing with wchar_t...  -----------------" << endl;
  //generate_1(std::wstring(L"Meow"));

  //cout << "-----------------  testing with u8_t...  -----------------" << endl;
  //u8_t u8src[] = { 'M', 'e', 'o', 'w', 0 };
  //generate_1(std::basic_string<u8_t>(u8src));

  //cout << "-----------------  testing with u16_t...  -----------------" << endl;
  //u16_t u16src[] = { 'M', 'e', 'o', 'w', 0 };
  //generate_1(std::basic_string<u16_t>(u16src));

  //cout << "-----------------  testing with u32_t...  -----------------" << endl;
  //u32_t u32src[] = { 'M', 'e', 'o', 'w', 0 };
  //generate_1(std::basic_string<u32_t>(u32src));

  value_tests();


  return ::boost::report_errors();
}
