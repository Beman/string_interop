//--------------------------------------------------------------------------------------//
//                                                                                      //
//   In namespace boost, provide typdefs for C++0x types if present, otherwise for      //
//   C++03 equivalent types. By using the boost:: typedefs, these Ox features become    //
//   available for use with C++03 compilers.                                            //
//                                                                                      //
//   Boost              C++0x           C++03                                           //
//   ----------------   --------------  --------------------------------                //
//   boost::char16      char16_t        uint16_t                                        //
//   boost::char32      char32_t        uint32_t                                        //
//   boost::u16string   std::u16string  std::basic_string<boost::char16>                //
//   boost::u32string   std::u32string  std::basic_string<boost::char32>                //
//                                                                                      //
//   Will use the typedefs provided by Microsoft Visual C++ 2010 if present             //
//                                                                                      //
//   Thanks to Mathias Gaunard and others for discussions leading to the final form     //
//   of these typedefs.                                                                 //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_STRING_0X_HPP)
# define BOOST_STRING_0X_HPP

# include <boost/config.hpp>
# include <boost/cstdint.hpp>
# include <string>

namespace boost
{

# if defined(BOOST_NO_CHAR16_T) && (!defined(_MSC_VER) || _MSC_VER < 1600)  // VC++10 
    typedef boost::uint16_t  char16;
    typedef std::basic_string<boost::char16> u16string;
# else
    typedef char16_t char16;
    typedef std::u16string u16string;
# endif

# if defined(BOOST_NO_CHAR32_T) && (!defined(_MSC_VER) || _MSC_VER < 1600)  // VC++10 
    typedef  boost::uint32_t  char32;
    typedef std::basic_string<boost::char32> u32string;
# else
    typedef char32_t char32;
    typedef std::u32string u32string;
# endif

}  // namespace boost

#endif  // !defined(BOOST_STRING_0X_HPP)
