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
  template <class Iterator>  
  void test(Iterator itr, std::size_t size)
  {
    std::string known_correct("Meow");
    std::size_t i=0;
    for(; *itr; ++itr, ++i)
    {
      BOOST_ASSERT_MSG(i < size, "end should have been reached");
      BOOST_TEST_EQ( static_cast<char>(*itr), known_correct[i]);
    }
    BOOST_TEST_EQ(i, size);
  }

  template <class String, class To>
  void generate_2(const String& str)
  {
    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_null, To>  itr1(str.c_str());

    test(itr1, str.size());

    converting_iterator<const typename String::value_type*,
      typename String::value_type,
      by_size, To>  itr2(str.c_str(), str.size()-1);

    test(itr2, str.size()-1);

    converting_iterator<typename String::const_iterator,
      typename String::value_type,
      by_range, To>  itr3(str.begin(), str.end()-2);

    test(itr3, str.size()-2);
  }

  template <class String>
  void generate_1(const String& str)
  {
    // each target type
    generate_2<String, char>(str);
    generate_2<String, wchar_t>(str);
    generate_2<String, u8_t>(str);
    generate_2<String, u16_t>(str);
    generate_2<String, u32_t>(str);
  }

  void value_tests()
  {
    cout << "-----------------  value_tests  -----------------\n";

    //  Test with cases that require UTF-16 surrogate pairs
    //  U+1F60A SMILING FACE WITH SMILING EYES
    //  U+1F60E SMILING FACE WITH SUNGLASSES

    u32_t utf32[] = {0x1F60A, 0x1F60E, 0};
    u16_t utf16[] = {0xD83D, 0xDE0A, 0xD83D, 0xDE0E, 0};
    u8_t  utf8[] = {0xF0, 0x9F, 0x98, 0x8A, 0xF0, 0x9F, 0x98, 0x8E, 0};

    int i = 0;
    for (converting_iterator<const u32_t*, u32_t, by_null, u16_t>
      it(utf32); *it; ++it, ++i)
        BOOST_TEST_EQ(*it, utf16[i]);
    BOOST_TEST_EQ(i, 4);

    i = 0;
    for (converting_iterator<const u32_t*, u32_t, by_null, u8_t>
      it(utf32); *it; ++it, ++i)
        BOOST_TEST_EQ(*it, utf8[i]);
    BOOST_TEST_EQ(i, 8);

    // utf-8 to utf-16, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    for (converting_iterator<const u8_t*, u8_t, by_null, u16_t>
      it(utf8); *it; ++it, ++i)
        BOOST_TEST_EQ(*it, utf16[i]);
    BOOST_TEST_EQ(i, 4);

    // utf-16 to utf-8, demonstrating that utf-16 surrogate pairs are handled correctly
    i = 0;
    for (converting_iterator<const u16_t*, u16_t, by_null, u8_t>
      it(utf16); *it; ++it, ++i)
        BOOST_TEST_EQ(*it, utf8[i]);
    BOOST_TEST_EQ(i, 8);
  }

}  // unnamed namespace

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int, char*[])
{
  cout << "iterator_adapter_unit_test...\n" << hex;

  cout << "-----------------  testing with char...  -----------------\n";
  generate_1(std::string("Meow"));

  cout << "-----------------  testing with wchar_t...  -----------------\n";
  generate_1(std::wstring(L"Meow"));

  cout << "-----------------  testing with u8_t...  -----------------\n";
  u8_t u8src[] = { 'M', 'e', 'o', 'w', 0 };
  generate_1(std::basic_string<u8_t>(u8src));

  cout << "-----------------  testing with u16_t...  -----------------\n";
  u16_t u16src[] = { 'M', 'e', 'o', 'w', 0 };
  generate_1(std::basic_string<u16_t>(u16src));

  cout << "-----------------  testing with u32_t...  -----------------\n";
  u32_t u32src[] = { 'M', 'e', 'o', 'w', 0 };
  generate_1(std::basic_string<u32_t>(u32src));

  value_tests();


  return ::boost::report_errors();
}
