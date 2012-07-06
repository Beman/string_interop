namespace std {
namespace tbd {  // to be decided
 
  //  codecs [str-x.codec]
  class narrow;   // native encoding for char
  class wide;     // native encoding for wchar_t
  class utf8;     // UTF-8 encoding for char
  class utf16;    // UTF-16 encoding for char16_t
  class utf32;    // UTF-32 encoding for char32_t
  class default_codec;  // see [str-x.codec.default]

  //  select_codec [str-x.codec.select]
  template <class charT> struct select_codec;
  template <> struct select_codec<char>       { typedef narrow type; };
  template <> struct select_codec<wchar_t>    { typedef wide   type; };
  template <> struct select_codec<char16_t>   { typedef utf16  type; };
  template <> struct select_codec<char32_t>   { typedef utf32  type; };
 
  //  conversion_iterator [str-x.cvt-iter]
  template <class ToCodec, class FromCodec, class ForwardIterator>
    class conversion_iterator;

  //  convert string function [str-x.cvt.ctr]
  template <class ToCodec,
            class FromCodec = default_codec,
            class ToString = std::basic_string<typename ToCodec::value_type>,
            class FromString>
  ToString convert(const FromString& ctr);

  //  convert null terminated iterator function [str-x.cvt.size]
  template <class ToCodec,
            class FromCodec = default_codec,
            class ToString = std::basic_string<typename ToCodec::value_type>,
            class ForwardIterator>
  ToString convert(ForwardIterator begin);

  //  convert iterator with size function [str-x.cvt.null]
  template <class ToCodec,
            class FromCodec = default_codec,
            class ToString = std::basic_string<typename ToCodec::value_type>,
            class ForwardIterator>
  ToString convert(ForwardIterator begin, std::size_t sz);

  //  convert iterator range function [str-x.cvt.range]
  template <class ToCodec,
            class FromCodec = default_codec,
            class ToString = std::basic_string<typename ToCodec::value_type>,
            class ForwardIterator,
            class ForwardIterator2>
  ToString convert(ForwardIterator begin, ForwardIterator2 end);

}  // namespace tbd

  //  converting stream inserter [str-x.cvt.inserter]
  template <class Ostream, class charT, class Traits, class Allocator>
  Ostream& operator<<(Ostream& os, const basic_string<charT, Traits, Allocator>& str);

  basic_ostream<char>& operator<<(basic_ostream<char>& os, const wchar_t* p);
  basic_ostream<char>& operator<<(basic_ostream<char>& os, const char16_t* p);
  basic_ostream<char>& operator<<(basic_ostream<char>& os, const char32_t* p);
  
}  // namespace std


//---------------------------  codec requirements  -------------------------------------//

  typedef char value_type;

  template <class charT>
  struct codec { typedef narrow type; };

  template <class ForwardIterator>  
  class from_iterator
  {
  public:
    
    from_iterator();
    from_iterator(ForwardIterator begin);
    from_iterator(ForwardIterator begin, std::size_t sz);
    template <class T>
      from_iterator(ForwardIterator begin, T end);
  };

  template <class ForwardIterator>  
  class to_iterator
  {
  public:
    to_iterator();
    to_iterator(ForwardIterator begin);
  };

