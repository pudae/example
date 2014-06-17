#include <iostream>

#include "expected.hpp"

using namespace railway;

int main()
{
  std::cout << "Hello Railway!! " << std::endl;

  std::cout << "Expected:" << Expected<int>(3).value() << std::endl;
  std::cout << "Unexpected:" << Unexpected("123").error() << std::endl;
  std::cout << "From Unexpected:" << Expected<int>(Unexpected("123")).error() << std::endl;

  return 0;
}
