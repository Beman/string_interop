//  boost/interop/detail/config.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_CONFIG_HPP)
#define BOOST_INTEROP_CONFIG_HPP

#include <boost/system/api_config.hpp>  // for BOOST_POSIX_API or BOOST_WINDOWS_API

#  define BOOST_INTEROP_THROW(e) throw e

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

#endif  // BOOST_INTEROP_CONFIG_HPP
