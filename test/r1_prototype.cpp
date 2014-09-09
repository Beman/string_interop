//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config/warning_disable.hpp>  // must precede other headers

#include <iostream>
#include <boost/string_interop/detail/config.hpp>
#include <boost/string_interop/cxx11_char_types.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

#include <boost/utility/string_ref.hpp>

using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace r1
{

//  Encodings and their corresponding encoded character type (i.e. value_type)

struct narrow { typedef char      value_type; };
struct wide   { typedef wchar_t   value_type; };
struct utf8   { typedef char      value_type; };
struct utf16  { typedef char16_t  value_type; };
struct utf32  { typedef char32_t  value_type; };

//  Encoded character types (i.e. value types) and their corresponding encodings

template <class EcharT>
struct encoding;

template<> struct encoding<char>     { typedef narrow  type; };
template<> struct encoding<wchar_t>  { typedef wide    type; };
template<> struct encoding<char16_t> { typedef utf16   type; };
template<> struct encoding<char32_t> { typedef utf32   type; };

//  Lazy encoding lookup for use as a default template parameter that appears before the
//  parameter that determines the value type

class default_encoding
{
public:
  template <class EcharT>
  struct encodes
  {
    typedef typename encoding<EcharT>::type type;
  };
};

//  Default ErrorHandler
//  TODO: add operator() argument to distinguish between ill-formed and unknown character,
//        or maybe call with arg of nulptr if ill-formed.

template <class ToCharT, class FromCharT>
class barf
{
  static std::basic_string<ToCharT> bad_news(ToCharT('?'));
public:
  const std::basic_string<ToCharT>& operator()(const FromCharT*)
  {
    return bad_news;
  }
};

//  Generic string conversion

template <class ToEncoding,
          class FromEncoding = default_encoding,
          class ToString = std::basic_string<typename ToEncoding::value_type>,
          class Source,
          class ErrorHandler = barf<typename ToString::value_type,
            typename source_value_type<Source>::type> >
inline ToString to_string(const Source& source, ErrorHandler eh = ErrorHandler());

//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//

template <class ToEncoding, class FromEncoding, class ToString,
  class charT, class traits, class ErrorHandler>
ToString
to_string_helper(const boost::basic_string_ref<charT, traits>& from, ErrorHandler eh)
{
  ToString temp;
  std::copy(...);
  return temp;
}

}  // namespace r1

//--------------------------------------------------------------------------------------//
//                                       main                                           //
//--------------------------------------------------------------------------------------//

int cpp_main(int, char*[])
{

  string s;
  wstring ws;
  u16string s16;
  u32string s32

  //  use cases

  to_string<char>(s);
  to_string<narrow>(s);
  to_string<utf8>(s);
  to_string<wchar_t>(s);
  to_string<wide>(s);
  to_string<char16_t>(s);
  to_string<utf16>(s);
  to_string<char32_t>(s);
  to_string<utf32>(s);

  to_string<char>(ws);
  to_string<narrow>(ws);
  to_string<utf8>(ws);
  to_string<wchar_t>(ws);
  to_string<wide>(ws);
  to_string<char16_t>(ws);
  to_string<utf16>(ws);
  to_string<char32_t>(ws);
  to_string<utf32>(ws);

  to_string<char>(s16);
  to_string<narrow>(s16);
  to_string<utf8>(s16);
  to_string<wchar_t>(s16);
  to_string<wide>(s16);
  to_string<char16_t>(s16);
  to_string<utf16>(s16);
  to_string<char32_t>(s16);
  to_string<utf32>(s16);

  to_string<char>(s32);
  to_string<narrow>(s32);
  to_string<utf8>(s32);
  to_string<wchar_t>(s32);
  to_string<wide>(s32);
  to_string<char16_t>(s32);
  to_string<utf16>(s32);
  to_string<char32_t>(s32);
  to_string<utf32>(s32);






  return ::boost::report_errors();
}
