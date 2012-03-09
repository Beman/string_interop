//  boost/interop/string_tr.hpp  -------------------------------------------------------//

//  Copyright Beman Dawes 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_STRING_CONVERSION_HPP
#define BOOST_STRING_CONVERSION_HPP

//--------------------------------------------------------------------------------------//
//                                                                                      //
//           Experimental string extentions for the C++11 standard library              //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <climits>  // for MB_LEN_MAX
#include <cwchar>   // for mbstate_t
#include <iterator>
#include <algorithm>

////--------------------------------------------------------------------------------------//
////                                                                                      //
////                              Header <cuchar> support                                 //
////                                                                                      //
////  Very few C++11 standard library implementations support <cuchar>, so for now        //
////  declare the contents here.                                                          //
////                                                                                      //
////--------------------------------------------------------------------------------------//
//
//#define  __STDC_UTF_16__
//#define  __STDC_UTF_32__
//
//namespace std
//{
//  std::size_t mbrtoc16(char16_t* pc16,
//    const char* s, size_t n,
//    std::mbstate_t* ps);
//
//  std::size_t c16rtomb(char* s, char16_t c16,
//    std::mbstate_t* ps);
//
//  std::size_t mbrtoc32(char32_t* pc32, // target
//    const char* s, std::size_t n,      // source
//    std::mbstate_t* ps);
//
//  std::size_t c32rtomb(char* s,        // target
//    char32_t c32,                      // source
//    std::mbstate_t* ps);
//}

//--------------------------------------------------------------------------------------//

namespace std
{
namespace tbd
{

//--------------------------------------------------------------------------------------//
//                                       codecs                                         //
//--------------------------------------------------------------------------------------//

  std::size_t from_char_codec(const char* first, const char* last, char32_t* result,
    std::mbstate_t* ps);

  std::size_t from_wchar_codec(const wchar_t* first, const wchar_t* last,
    char32_t* result, std::mbstate_t* ps);

  std::size_t from_char16_codec(const char16_t* first, const char16_t* last,
    char32_t* result, std::mbstate_t* ps);

  std::size_t from_char32_codec(const char32_t* first, const char32_t, char32_t* result,
    std::mbstate_t*)
  {
    *result = *first;
    return 1;
  }

  std::size_t to_char_codec(char32_t src, char* result,  std::mbstate_t* ps);
  std::size_t to_wchar_codec(char32_t src, wchar_t* result,  std::mbstate_t* ps);
  std::size_t to_char16_codec(char32_t src, char16_t* result,  std::mbstate_t* ps);
  std::size_t to_char32_codec(char32_t src, char32_t* result,  std::mbstate_t* ps)
  {
    *result = src;
    return 1;
  }

//--------------------------------------------------------------------------------------//
//                              default_error_handler                                   //
//--------------------------------------------------------------------------------------//

  struct default_error_handler
  {
  };

//--------------------------------------------------------------------------------------//
//                              recode_copy algorithm                                   //
//--------------------------------------------------------------------------------------//

  template <class InputIterator, class OutputIterator,
            class FromCodec, class ToCodec, class ErrorHandler>
    OutputIterator
      recode_copy(InputIterator first, InputIterator last, FromCodec from_codec,
                  OutputIterator result, ToCodec to_codec, ErrorHandler eh)
  {
    typedef typename iterator_traits<InputIterator>::value_type   source_value_type;
    typedef typename iterator_traits<OutputIterator>::value_type  result_value_type;

    std::mbstate_t in_state; //???? verify initialization
    std::mbstate_t out_state; // ditto
    char32_t value;
    result_value_type result_buf[MB_LEN_MAX];

    while(first != last)
    {
      // convert source character[s] and shift info to one utf-32 character
      size_t n =                                     
        from_codec(&*first, std::min(&*last, &*first+MB_LEN_MAX), &value, &in_state);
// TODO: error handling here
// TODO: need to protect against from_codec returning n > std::min(&*last, &*first+MB_LEN_MAX)
      std::advance(first, n);
      // convert the utf-32 character to target character[s] and shift info
      n = to_codec(value, result_buf, &out_state);    
// TODO: error handling here
      // copy the target character[s] to result
      std::copy(result_buf, result_buf + n, result);
    }
    return result;
  }

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                  string_cast                                         //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  //template <class InputString, class OutputString, class FromCodec, class ToCodec,
  //          class ErrorHandler>
  //OutputString string_cast(const InputString& from, FromCodec from_codec,
  //                         ToCodec to_codec,
  //                         ErrorHandler error_handler=default_error_handler())
  //{
  //  OutputString result;
  //  recode_copy(from.cbegin(), from.cend(), from_codec,
  //    std::back_insert_iterator(result), to_codec, error_handler);
  //  return result;
  //}

  //template <class InputString, class OutputString, class FromCodec,
  //          class ErrorHandler>
  //inline
  //OutputString string_cast(const InputString& from, FromCodec from_codec,
  //                         ErrorHandler error_handler=default_error_handler)
  //{
  //  return string_cast<OutputString>(from, from_codec,
  //    default_to_codec<typename OutputString::value_type>(), error_handler);
  //}

  //template <class InputString, class OutputString, class ToCodec, 
  //          class ErrorHandler>
  //inline
  //OutputString string_cast(const InputString& from, ToCodec to_codec,
  //                         ErrorHandler error_handler=default_error_handler);

  template <class OutputString, class InputString, class ErrorHandler>
  inline
  OutputString string_cast(const InputString& from,
                            ErrorHandler error_handler)
  {
    OutputString result;
    recode_copy(from.cbegin(), from.cend(), from_codec,
      std::back_inserter(result), to_codec, error_handler);
    return result;
  }

  template <class OutputString, class InputString>
  inline
  OutputString string_cast(const InputString& from)
  {
    OutputString result;
    recode_copy(from.cbegin(), from.cend(), from_codec_16,
      std::back_inserter(result), to_codec_char,
      default_error_handler());
    return result;
  }

}  // namespace tbd
}  // namespace std

#endif  // BOOST_STRING_CONVERSION_HPP
