//  string_interop/tools/report_codecvt_results.cpp  -----------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

/*--------------------------------------------------------------------------------------//

The specs for std::codecvt are opaque as regards the returned result.

This program reports the returned results for various test cases using Microsoft
codecvt implementations.

//--------------------------------------------------------------------------------------*/

#include <iostream>
#include <iomanip>
#include <locale>
#include <string>
#include <vector>
#include <cstdint>
#include <cvt/utf8>

using namespace std;

namespace
{
  vector<string> tests;

  stdext::cvt::codecvt_utf8<char32_t> cvt;

  const char* results_table[] = {"ok", "partial", "error", "noconv"};

  void build_tests()
  {
    tests.push_back("test");
    tests.push_back("t");
    tests.push_back("\xEF\xBB\xBFt");  // UTF-8 byte order mark, 't' 
    tests.push_back("\xF0\x9F\x98\x8A");  // U+1F60A SMILING FACE WITH SMILING EYES
    tests.push_back("\xF0\x9F\x98");  // partial U+1F60A SMILING FACE WITH SMILING EYES
    tests.push_back("\xc3\x28");  // Invalid 2 Octet Sequence
    tests.push_back("\xe2\x82\x28");  // Invalid 3 Octet Sequence (in 3rd Octet)
    tests.push_back("\xf8\xa1\xa1\xa1\xa1");  // Valid 5 Octet Sequence (but not Unicode!)

  }

  void report(const string& s)
  {
    size_t ct = 0;
    for (unsigned char c : s)
    {
      cout << uint16_t(c);
      if (++ct < s.size())
        cout << ',';
    }
    char32_t     buf[128];
    mbstate_t    state = mbstate_t();
    const char*  from_begin = s.c_str();
    const char*  from_end = from_begin + s.size();
    const char*  from_next = 0;
    char32_t*    to_begin = buf;
    char32_t*    to_end = buf+1;
    char32_t*    to_next = 0;
    codecvt_base::result result =
      cvt.in(state, from_begin, from_end, from_next,
      to_begin, to_end, to_next);

    cout << " result=" << results_table[result];
    if (from_next)
      cout << ", from_next=from_begin+" << from_next - from_begin;
    else
      cout << ", from_next=n/a";
    if (to_next)
      cout << ", to_next=to_begin+" << to_next - to_begin;
    else
      cout << ", to_next=n/a";

    cout << " ";
    for (; to_next && to_begin != to_next; ++to_begin)
    {
      cout << uint32_t(*to_begin);
      if (to_begin != (to_next-1))
        cout << ',';
    }

    cout << endl;
  }

  void report_tests()
  {
    for (string s : tests)
      report(s);
  }
} 

int main()
{
  cout << hex;
  build_tests();
  report_tests();
  return 0;
}
