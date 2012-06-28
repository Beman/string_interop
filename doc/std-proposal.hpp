namespace tbd   // namespace to be decided
{
  typedef unsigned char    char8_t;
  
  //  End policies [str-x.end]
  struct by_null_policy {};
  struct by_range_policy {};
  struct by_size_policy {};

  template <class InputIterator>
    class by_null;
  template <class InputIterator>
    class by_range;
  template <class InputIterator>
   class by_size;

  //  from_iterator [str-x.from_iter]
  template <class InputIterator, class FromCharT, template<class> class EndPolicy>  
    class from_iterator;
  template <class InputIterator, template<class> class EndPolicy>
    class from_iterator<InputIterator, char, EndPolicy>;
  template <class InputIterator, template<class> class EndPolicy>
    class from_iterator<InputIterator, wchar_t, EndPolicy>;
  template <class InputIterator, template<class> class EndPolicy>
    class from_iterator<InputIterator, char8_t, EndPolicy>;
  template <class InputIterator, template<class> class EndPolicy>
    class from_iterator<InputIterator, char16_t, EndPolicy>;
  template <class InputIterator, template<class> class EndPolicy>
    class from_iterator<InputIterator, char32_t, EndPolicy>;

  //  to_iterator [str-x.to_iter]
  template <class InputIterator, class ToCharT>  
    class to_iterator; 
  template <class InputIterator>
    class to_iterator<InputIterator, char>;
  template <class InputIterator>
    class to_iterator<InputIterator, wchar_t>;
  template <class InputIterator>
    class to_iterator<InputIterator, char8_t>;
  template <class InputIterator>
    class to_iterator<InputIterator, char16_t>;
  template <class InputIterator>
    class to_iterator<InputIterator, char32_t>;

  //  utf-8 iterators [str-x.utf8]
  template <class InputIterator, class FromCharT, template<class> class EndPolicy>  
    class from_utf8; 
  template <class InputIterator, class ToCharT = charT>
    class to_utf8;

  //  conversion_iterator [str-x.cvt-iter
  template <class InputIterator, class FromCharT, template<class> class EndPolicy,
    class ToCharT, template <class, class> class ToIterator = to_iterator,
    template <class, class,
      template<class> class> class SourceIterator = from_iterator>
    class conversion_iterator;

  //  convert functions [str-x.cvt]

  //  convert container [str-x.cvt.ctr]
  template <class ToContainer,
      template <class, class> class ToIterator = to_iterator,
      template <class, class,
        template<class> class> class FromIterator = from_iterator,
      class FromContainer>
    ToContainer convert(const FromContainer& ctr);

  //  convert null terminated iterator [str-x.cvt.size]
  template <class ToContainer,
      template <class, class> class ToIterator = to_iterator,
      template <class, class,
        template<class> class> class FromIterator = from_iterator,
      class InputIterator>
    ToContainer convert(InputIterator begin);

  //  convert iterator, size [str-x.cvt.null]
  template <class ToContainer,
      template <class, class> class ToIterator = to_iterator,
      template <class, class,
        template<class> class> class FromIterator = from_iterator,
      class InputIterator>
    ToContainer convert(InputIterator begin, std::size_t sz);

  //  convert iterator range [str-x.cvt.range]
  template <class ToContainer,
      template <class, class> class ToIterator = to_iterator,
      template <class, class,
        template<class> class> class FromIterator = from_iterator,
      class InputIterator, class InputIterator2>
    ToContainer convert(InputIterator begin, InputIterator2 end);

  //  converting stream inserter [str-x.cvt.inserter]
  template <class Ostream, class charT, class Traits, class Allocator>
  Ostream& operator<<(Ostream& os, const basic_string<charT, Traits, Allocator>& str);

  basic_ostream<char>& operator<<(basic_ostream<char>& os, const wchar_t* p);
  basic_ostream<char>& operator<<(basic_ostream<char>& os, const char8_t* p);
  basic_ostream<char>& operator<<(basic_ostream<char>& os, const char16_t* p);
  basic_ostream<char>& operator<<(basic_ostream<char>& os, const char32_t* p);
  
}