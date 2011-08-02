#include <iostream>

//  On Windows, the easiest way to verify this works is:
//
//     hello_world_utf8 >test.html
//     test.html

int main()
{
  std::cout << "\xef\xbb\xbf\xe6\x82\xa8\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c\n";
}
