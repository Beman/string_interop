//  libs/interop/tools/table_generator.cpp  --------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>

using namespace std;

//--------------------------------------------------------------------------------------//
//
//   Generate codec tables from http://unicode.org/Public/MAPPINGS/ Format A
//
//     Format: Three tab-separated columns
//        Column #1 is the cp437_DOSLatinUS code (in hex)
//        Column #2 is the Unicode (in hex as 0xXXXX)
//        Column #3 is the Unicode name (follows a comment sign, '#')
//
//--------------------------------------------------------------------------------------//

namespace
{
  unsigned int  wide_char[256];
  unsigned char narrow_char[256*256];

  const int n_slices = 512;  // sweet spot for both codepage 437 and codepage 1252
  const int chars_per_slice = (256*256) / n_slices;

  unsigned char slice_index[n_slices];


  int active_slices = 0;
}

int main()
{
  cout << hex;
  while (!cin.eof())
  {
    string str;
    getline(cin, str);
    if (str.size() && str[0] == '0')
    {
      //cout << str << '\n';

      char* endptr;
      long v1 = strtol(str.c_str()+2, &endptr, 16);
      long v2 = strtol(str.c_str()+7, &endptr, 16);

      wide_char[v1] = v2;

      assert(v1 <= 0xFF);
      narrow_char[v2] = unsigned char(v1);

      //cout << hex << v1 << " " << v2 << '\n';
    }
  }

  //  generate to_utf16

  cout << "const boost::u16_t  to_utf16[] =\n{\n";
  for (int i = 0; i < 256; ++i)
  {
    if (i)
      cout << ',';
    if (i % 16 == 0)
      cout << '\n';
    cout << "0x" << wide_char[i];
  }
  cout << "\n};\n\nBOOST_STATIC_ASSERT(sizeof(to_utf16) == 2*256);\n\n";

  //  generate to_char output and build slice_index

  cout << "const unsigned char to_char[] =\n{\n";

  //  slice 0 - characters with no codepage representation
  cout << "//  slice 0 - characters with no codepage representation\n";
  for (int j = 0; j < chars_per_slice; ++j)
  {
    cout << "'?',";
    if ((j+1) % 16 == 0)
      cout << '\n';
  }

  //  for each (potential) slice
  for (int i = 0; i < n_slices; ++i)
  {
    // find out if the slice is active
    bool slice_active = !i ? true : false;   // character '\0' must be active
    for (int j = 0; j < chars_per_slice; ++j)
    {
      if (narrow_char[i * chars_per_slice + j])
        slice_active = true;
    }

    if (slice_active)
    {
      slice_index[i] = ++active_slices;

      cout << "\n//  slice " << dec << active_slices
           << hex << " - 0x" << i * chars_per_slice << '\n';

      // for each character in the slice
      for (int j = 0; j < chars_per_slice; ++j)
      {
        if (active_slices == 1 && j == 0)
          cout << "0x00";  // character '\0' must be itself
        else if (narrow_char[i * chars_per_slice + j])
          cout << "0x" << unsigned int(narrow_char[i * chars_per_slice + j]);
        else
          cout << "'?'";
        if ((i < n_slices-1) || (j < chars_per_slice-1))
          cout << ',';
        if ((j+1) % 16 == 0)
          cout << '\n';
      }
    }
  }

  cout << "\n};\n\nBOOST_STATIC_ASSERT(sizeof(to_char) == "
       << dec << (active_slices + 1) * chars_per_slice << hex << ");\n\n";

  //  generate slice_index

  cout << "const boost::uint8_t slice_index[] =\n{\n";

  for (int i = 0; i < n_slices; ++i)
  {
    cout << dec << int(slice_index[i]);
    if (i < n_slices-1)
      cout << ',';
    if ((i+1) % 16 == 0)
      cout << '\n';
  }

  cout << "\n};\n\nBOOST_STATIC_ASSERT(sizeof(slice_index) == "
       << dec << n_slices << hex << ");\n\n";


  return 0;
}

