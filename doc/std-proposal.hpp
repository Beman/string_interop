namespace tbd   // namespace to be decided
{
  struct by_null_policy {};
  struct by_range_policy {};
  struct by_size_policy {};

  template <class InputIterator>
    class by_null;

  template <class InputIterator>
    class by_range;

  template <class InputIterator>
   class by_size;

  template <class InputIterator, class FromCharT, template<class> class EndPolicy>  
  class from_iterator; 

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u8_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u16_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, u32_t, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, char, EndPolicy>;

  template <class InputIterator, template<class> class EndPolicy>
  class from_iterator<InputIterator, wchar_t, EndPolicy>;
}