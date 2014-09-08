//  boost/string_interop/detail/config.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_STRING_INTEROP_CONFIG_HPP)
#define BOOST_STRING_INTEROP_CONFIG_HPP

#include <boost/config.hpp>
#include <boost/system/api_config.hpp>  // for BOOST_POSIX_API or BOOST_WINDOWS_API

#  define BOOST_STRING_INTEROP_THROW(e) throw e

//  For internal use only
#ifdef BOOST_XOP_DEBUGGING_LOG
#  define BOOST_XOP_LOG(MSG) std::cout << (MSG) << std::endl
#else
#  define BOOST_XOP_LOG(MSG)
#endif

//  For internal use only
#ifdef BOOST_XOP_STRING_DEBUGGING_LOG
#  define BOOST_XOP_STRING_LOG(MSG) std::cout << (MSG) << std::endl
#else
#  define BOOST_XOP_STRING_LOG(MSG)
#endif

//----------------------------  separate compilation  ----------------------------------//

//  This header implements separate compilation features as described in
//  http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(BOOST_STRING_INTEROP_DYN_LINK) && !defined(BOOST_STRING_INTEROP_STATIC_LINK) \
  && !defined(BOOST_ALL_DYN_LINK) && !defined(BOOST_ALL_STATIC_LINK)
# define BOOST_STRING_INTEROP_STATIC_LINK
#endif

#if defined(BOOST_ALL_DYN_LINK) && !defined(BOOST_STRING_INTEROP_DYN_LINK)
# define BOOST_STRING_INTEROP_DYN_LINK 
#elif defined(BOOST_ALL_STATIC_LINK) && !defined(BOOST_STRING_INTEROP_STATIC_LINK)
# define BOOST_STRING_INTEROP_STATIC_LINK 
#endif

#if defined(BOOST_STRING_INTEROP_DYN_LINK) && defined(BOOST_STRING_INTEROP_STATIC_LINK)
# error Must not define both BOOST_STRING_INTEROP_DYN_LINK and BOOST_STRING_INTEROP_STATIC_LINK
#endif

#if defined(BOOST_ALL_NO_LIB) && !defined(BOOST_STRING_INTEROP_NO_LIB)
# define BOOST_STRING_INTEROP_NO_LIB 
#endif

//  enable dynamic linking  ------------------------------------------------------------//

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_STRING_INTEROP_DYN_LINK)
# if defined(BOOST_STRING_INTEROP_SOURCE)
#   define BOOST_STRING_INTEROP_DECL BOOST_SYMBOL_EXPORT
# else 
#   define BOOST_STRING_INTEROP_DECL BOOST_SYMBOL_IMPORT
# endif
#else
# define BOOST_STRING_INTEROP_DECL
#endif

//  enable automatic library variant selection  ----------------------------------------// 

#if !defined(BOOST_STRING_INTEROP_SOURCE) && !defined(BOOST_ALL_NO_LIB) \
  && !defined(BOOST_STRING_INTEROP_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define BOOST_LIB_NAME boost_interop
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_STRING_INTEROP_DYN_LINK)
#  define BOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <boost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif  // BOOST_STRING_INTEROP_CONFIG_HPP
