//  boost/interop/conversion_iterator_adapters.hpp  ------------------------------------//

//  Copyright Beman Dawes 2011
//  Copyright (c) 2004 John Maddock

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//  John Maddock introduced me to the idea of performing conversion via iterator        //
//  Adapters. The code for the UTF-8 <--> UTF-32 and UTF-16 <--> UTF-32 adapers was     //
//  adapted from his boost/regex/pending/unicode_iterator.hpp header.                   //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP)
#define BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP

#include <boost/interop/string_0x.hpp>

namespace boost
{

//--------------------------------------------------------------------------------------//
//  For n encodings, it is desirable to provide 2n adapters rather than n squared       //
//  adapters. This is achieved by conceptually converting to UTF-32 and from UTF-32 as  //
//  an intermediate encoding, and then composing a converting_iterator from one         //
//  to_utf_32_iterator and one from_utf_32_iterator. For efficiency, specializations of //
//  converting_iterator can provide direct conversion without an intermedate UTF32 step.//
//--------------------------------------------------------------------------------------//

//-------------------------------  to_utf_32_iterator  ---------------------------------//

  template <class Iterator, class T>  // primary template
  class to_utf_32_iterator;           // partial specializations must be provided

  template <class Iterator>
  class to_utf_32_iterator<Iterator, u8_t>;

  template <class Iterator>
  class to_utf_32_iterator<Iterator, u16_t>;

//------------------------------  from_utf_32_iterator  --------------------------------//

  template <class Iterator, class T>  // primary template
  class from_utf_32_iterator;         // partial specializations must be provided

  template <class Iterator>
  class from_utf_32_iterator<Iterator, u8_t>;

  template <class Iterator>
  class from_utf_32_iterator<Iterator, u16_t>;

//-------------------------------  converting_iterator  --------------------------------//

  template <class Iterator, class From, class To>  // primary template
  class converting_iterator                   // partial specializations *may* be provided
    :  public from_utf_32_iterator<to_uft_32_iterator<Iterator, From>, To>
  {
  public:
    explicit converting_iterator(Iterator it)
      : from_utf_32_iterator<to_uft_32_iterator<Iterator, From>, To>(it) {}
  };

//------------------------------------  helpers  ---------------------------------------//  

//--------------------------------  specializations  -----------------------------------//  

  template <class Iterator>
  class to_utf_32_iterator<Iterator, u8_t>
  {
  }

  template <class Iterator>
  class to_utf_32_iterator<Iterator, u16_t>
  {
  }

  template <class Iterator>
  class from_utf_32_iterator<Iterator, u8_t>
  {
  }

  template <class Iterator>
  class from_utf_32_iterator<Iterator, u16_t>
  {
  }

}

#endif  // BOOST_INTEROP_CONVERSION_ITERATOR_ADAPTERS_HPP
