//  libs/interop/test/iterator_adapter_unit_test.cpp  ----------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/interop/convert.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using std::cout; using std::endl; using std::hex;

using namespace boost::interop;
using boost::u8_t;
using boost::u16_t;
using boost::u32_t;

namespace
{
  template <class DefaultCtorEndIterator>  
  void default_ctor_end_iter_test(DefaultCtorEndIterator itr, std::size_t size)
  {
    std::string known_correct("Meow");

    std::size_t i=0;
    for(; itr != DefaultCtorEndIterator(); ++itr, ++i)
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

  void codex_from_iterator_test()
  {
    cout << "codex_from_iterator_test..." << endl;

    cout << "  narrow codec" << endl;
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow), 4);
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow, 3), 3);
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow, meow+2), 2);
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow+4), 0);
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow, 0), 0);
    default_ctor_end_iter_test(narrow::from_iterator<const char*>(meow, meow), 0);

    cout << "  wide" << endl;
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww), 4);
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww, 3), 3);
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww, meoww+2), 2);
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww+4), 0);
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww, 0), 0);
    default_ctor_end_iter_test(wide::from_iterator<const wchar_t*>(meoww, meoww), 0);

    cout << "  utf8" << endl;
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow), 4);
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow, 3), 3);
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow, meow+2), 2);
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow+4), 0);
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow, 0), 0);
    default_ctor_end_iter_test(utf8::from_iterator<const char*>(meow, meow), 0);

    cout << "  uft16" << endl;
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16), 4);
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16, 3), 3);
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16, meow16+2), 2);
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16+4), 0);
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16, 0), 0);
    default_ctor_end_iter_test(utf16::from_iterator<const u16_t*>(meow16, meow16), 0);

    cout << "  utf32" << endl;
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32), 4);
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32, 3), 3);
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32, meow32+2), 2);
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32+4), 0);
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32, 0), 0);
    default_ctor_end_iter_test(utf32::from_iterator<const u32_t*>(meow32, meow32), 0);

    //cout << "  wchar_t" << endl;
    //default_ctor_end_iter_test(from_iterator<const wchar_t*, wchar_t>(meoww), 4);
  }

  void codex_to_iterator_test()
  {
    cout << "codex_to_iterator_test..." << endl;

    cout << "  narrow codec" << endl;
    default_ctor_end_iter_test(
      narrow::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow)), 4);
    default_ctor_end_iter_test(
      narrow::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow+4)), 0);

    cout << "  wide" << endl;
    default_ctor_end_iter_test(
      wide::to_iterator<wide::from_iterator<const wchar_t*> >(
        wide::from_iterator<const wchar_t*>(meoww)), 4);
    default_ctor_end_iter_test(
      wide::to_iterator<wide::from_iterator<const wchar_t*> >(
        wide::from_iterator<const wchar_t*>(meoww+4)), 0);

    cout << "  utf8" << endl;
    default_ctor_end_iter_test(
      utf8::to_iterator<utf8::from_iterator<const char*> >(
        utf8::from_iterator<const char*>(meow)), 4);
    default_ctor_end_iter_test(
      utf8::to_iterator<utf8::from_iterator<const char*> >(
        utf8::from_iterator<const char*>(meow+4)), 0);

    cout << "  utf16" << endl;
    default_ctor_end_iter_test(
      utf16::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow)), 4);
    default_ctor_end_iter_test(
      utf16::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow+4)), 0);

    cout << "  utf32" << endl;
    default_ctor_end_iter_test(
      utf32::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow)), 4);
    default_ctor_end_iter_test(
      utf32::to_iterator<narrow::from_iterator<const char*> >(
        narrow::from_iterator<const char*>(meow+4)), 0);
  }

  template <class String, class ToCodec>
  void generate_2(const String& str)
  {
    conversion_iterator<ToCodec,
      typename default_codec<typename String::value_type>::type,
      const typename String::value_type*>         itr1(str.c_str());

    default_ctor_end_iter_test(itr1, str.size());

    conversion_iterator<ToCodec,
      typename default_codec<typename String::value_type>::type,
      const typename String::value_type*>  itr2(str.c_str(), str.size()-1);

    default_ctor_end_iter_test(itr2, str.size()-1);

    conversion_iterator<ToCodec,
      typename default_codec<typename String::value_type>::type,
      typename String::const_iterator>  itr3(str.begin(), str.end()-2);

    default_ctor_end_iter_test(itr3, str.size()-2);
  }

  template <class String>
  void generate_1(const String& str)
  {
    // each target type
    generate_2<String, narrow>(str);
    generate_2<String, wide>(str);
    generate_2<String, utf8>(str);
    generate_2<String, utf16>(str);
    generate_2<String, utf32>(str);
  }

  void value_tests()
  {
    cout << "value_tests..." << endl;

    //  Test with cases that require UTF-16 surrogate pairs
    //  U+1F60A SMILING FACE WITH SMILING EYES
    //  U+1F60E SMILING FACE WITH SUNGLASSES

    u32_t utf32s[] = {0x1F60A, 0x1F60E, 0};
    u16_t utf16s[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
    wchar_t wides[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
    const char* utf8s = "\xF0\x9F\x98\x8A\xF0\x9F\x98\x8E";

    //  utf-32 to utf-16
    int i = 0;
    typedef conversion_iterator<boost::interop::utf16, boost::interop::utf32,
      const u32_t*> type_32_16;
    for (type_32_16 it(utf32s); it != type_32_16(); ++it, ++i)
        BOOST_TEST(*it == utf16s[i]);
    BOOST_TEST_EQ(i, 4);

    // utf-32 to utf-8
    i = 0;
    typedef conversion_iterator<boost::interop::utf8, boost::interop::utf32,
      const u32_t*> type_32_8;
    for (type_32_8 it(utf32s); it != type_32_8(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf8s[i]);
    BOOST_TEST_EQ(i, 8);

    // utf-8 to utf-16, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef conversion_iterator<boost::interop::utf16, boost::interop::utf8,
      const char*> type_8_16;
    for (type_8_16 it(utf8s); it != type_8_16(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf16s[i]);
    BOOST_TEST_EQ(i, 4);

    // utf-16 to utf-8, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef conversion_iterator<boost::interop::utf8, boost::interop::utf16,
      const u16_t*> type_16_8;
    for (type_16_8 it(utf16s); it != type_16_8(); ++it, ++i)
        BOOST_TEST_EQ(*it, utf8s[i]);
    BOOST_TEST_EQ(i, 8);

    // utf-8 to wide, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef conversion_iterator<boost::interop::wide, boost::interop::utf8,
      const char*> type_8_wide;
    for (type_8_wide it(utf8s); it != type_8_wide(); ++it, ++i)
        BOOST_TEST(*it == wides[i]);
    BOOST_TEST_EQ(i, 4);

    // wide to utf-8, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    typedef conversion_iterator<boost::interop::utf8, boost::interop::wide,
      const wchar_t*> type_wide_8;
    for (type_wide_8 it(wides); it != type_wide_8(); ++it, ++i)
        BOOST_TEST(*it == utf8s[i]);
    BOOST_TEST_EQ(i, 8);
  }

}  // unnamed namespace

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int, char*[])
{
  cout << "iterator_adapter_unit_test...\n" << hex;

  codex_from_iterator_test();
  codex_to_iterator_test();

  cout << "-----------------  testing with char...  -----------------" << endl;
  generate_1(std::string("Meow"));

  cout << "-----------------  testing with wchar_t...  -----------------" << endl;
  generate_1(std::wstring(L"Meow"));

  //cout << "-----------------  testing with u8_t...  -----------------" << endl;
  //u8_t u8src[] = { 'M', 'e', 'o', 'w', 0 };
  //generate_1(std::basic_string<u8_t>(u8src));

  cout << "-----------------  testing with u16_t...  -----------------" << endl;
  u16_t u16src[] = { 'M', 'e', 'o', 'w', 0 };
  generate_1(std::basic_string<u16_t>(u16src));

  cout << "-----------------  testing with u32_t...  -----------------" << endl;
  u32_t u32src[] = { 'M', 'e', 'o', 'w', 0 };
  generate_1(std::basic_string<u32_t>(u32src));

  value_tests();


  return ::boost::report_errors();
}
