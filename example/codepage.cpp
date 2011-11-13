#include <iostream>

int main()
{
  for (int c = 0x20; c <= 0xff; ++c)
  {
    if (c != 0x20)
	  std::cout << ", ";
    std::cout << std::hex << int(c) << " " << unsigned char(c);
  }
  std::cout << std::endl;
}