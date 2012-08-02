//  libs/interop/src/codepage_437.cpp  -------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// define BOOST_INTEROP_SOURCE so that <boost/interop/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#define BOOST_INTEROP_SOURCE 

#include <boost/interop/detail/config.hpp>
#include <boost/interop/cxx11_char_types.hpp>
#include <boost/static_assert.hpp>

namespace boost
{
namespace interop
{
namespace detail
{

/****************** tables generated by tools/table_generator.cpp ***********************/

/****************************** DO NOT EDIT BY HAND *************************************/

BOOST_INTEROP_DECL extern const boost::char16  to_utf16[] =
{
0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0xc7,0xfc,0xe9,0xe2,0xe4,0xe0,0xe5,0xe7,0xea,0xeb,0xe8,0xef,0xee,0xec,0xc4,0xc5,
0xc9,0xe6,0xc6,0xf4,0xf6,0xf2,0xfb,0xf9,0xff,0xd6,0xdc,0xa2,0xa3,0xa5,0x20a7,0x192,
0xe1,0xed,0xf3,0xfa,0xf1,0xd1,0xaa,0xba,0xbf,0x2310,0xac,0xbd,0xbc,0xa1,0xab,0xbb,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255d,0x255c,0x255b,0x2510,
0x2514,0x2534,0x252c,0x251c,0x2500,0x253c,0x255e,0x255f,0x255a,0x2554,0x2569,0x2566,0x2560,0x2550,0x256c,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256b,0x256a,0x2518,0x250c,0x2588,0x2584,0x258c,0x2590,0x2580,
0x3b1,0xdf,0x393,0x3c0,0x3a3,0x3c3,0xb5,0x3c4,0x3a6,0x398,0x3a9,0x3b4,0x221e,0x3c6,0x3b5,0x2229,
0x2261,0xb1,0x2265,0x2264,0x2320,0x2321,0xf7,0x2248,0xb0,0x2219,0xb7,0x221a,0x207f,0xb2,0x25a0,0xa0
};

BOOST_STATIC_ASSERT(sizeof(to_utf16) == 2*256);

BOOST_INTEROP_DECL extern const unsigned char to_char[] =
{
//  slice 0 - characters with no codepage representation
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 1 - 0x0
0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,

//  slice 2 - 0x80
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xff,0xad,0x9b,0x9c,'?',0x9d,'?','?','?','?',0xa6,0xae,0xaa,'?','?','?',
0xf8,0xf1,0xfd,'?','?',0xe6,'?',0xfa,'?','?',0xa7,0xaf,0xac,0xab,'?',0xa8,
'?','?','?','?',0x8e,0x8f,0x92,0x80,'?',0x90,'?','?','?','?','?','?',
'?',0xa5,'?','?','?','?',0x99,'?','?','?','?','?',0x9a,'?','?',0xe1,
0x85,0xa0,0x83,'?',0x84,0x86,0x91,0x87,0x8a,0x82,0x88,0x89,0x8d,0xa1,0x8c,0x8b,
'?',0xa4,0x95,0xa2,0x93,'?',0x94,0xf6,'?',0x97,0xa3,0x96,0x81,'?','?',0x98,

//  slice 3 - 0x180
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?',0x9f,'?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 4 - 0x380
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?',0xe2,'?','?','?','?',0xe9,'?','?','?','?','?','?','?',
'?','?','?',0xe4,'?','?',0xe8,'?','?',0xea,'?','?','?','?','?','?',
'?',0xe0,'?','?',0xeb,0xee,'?','?','?','?','?','?','?','?','?','?',
0xe3,'?','?',0xe5,0xe7,'?',0xed,'?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 5 - 0x2000
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',0xfc,

//  slice 6 - 0x2080
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?',0x9e,'?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 7 - 0x2200
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?',0xf9,0xfb,'?','?','?',0xec,'?',
'?','?','?','?','?','?','?','?','?',0xef,'?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?',0xf7,'?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?',0xf0,'?','?',0xf3,0xf2,'?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 8 - 0x2300
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xa9,'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xf4,0xf5,'?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 9 - 0x2500
0xc4,'?',0xb3,'?','?','?','?','?','?','?','?','?',0xda,'?','?','?',
0xbf,'?','?','?',0xc0,'?','?','?',0xd9,'?','?','?',0xc3,'?','?','?',
'?','?','?','?',0xb4,'?','?','?','?','?','?','?',0xc2,'?','?','?',
'?','?','?','?',0xc1,'?','?','?','?','?','?','?',0xc5,'?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
0xcd,0xba,0xd5,0xd6,0xc9,0xb8,0xb7,0xbb,0xd4,0xd3,0xc8,0xbe,0xbd,0xbc,0xc6,0xc7,
0xcc,0xb5,0xb6,0xb9,0xd1,0xd2,0xcb,0xcf,0xd0,0xca,0xd8,0xd7,0xce,'?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

//  slice 10 - 0x2580
0xdf,'?','?','?',0xdc,'?','?','?',0xdb,'?','?','?',0xdd,'?','?','?',
0xde,0xb0,0xb1,0xb2,'?','?','?','?','?','?','?','?','?','?','?','?',
0xfe,'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',
'?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',

};

BOOST_STATIC_ASSERT(sizeof(to_char) == 1408);

BOOST_INTEROP_DECL extern const boost::uint8_t slice_index[] =
{
1,2,0,3,0,0,0,4,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
5,6,0,0,7,0,8,0,0,0,9,10,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

};

BOOST_STATIC_ASSERT(sizeof(slice_index) == 512);

}  // namesapce detail

}  // namespace interop

}  // namespace boost
