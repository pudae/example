
#include <iostream>
#include <experimental/generator>
#include <experimental/resumable>

#include "expected.h"

myco::expected<int> my_div(int a, int b) {
  if (0 == b)
    throw std::logic_error("divide by zero");
  return a / b;
}

myco::expected<int> throw_if_odd(int a) {
  try {
    if (a % 2)
  	  throw std::runtime_error("is odd!!");
    return myco::expected<int>(a);
  }
  catch (...) {
    return myco::expected<int>(std::current_exception());
  }
}

myco::expected<int> cal(int a, int b) {
  auto x = await my_div(a, b);
  auto y = await throw_if_odd(a);
  return x + y;
}

void test_expected()
{
  try {
    std::cout << cal(20, 10).get() << std::endl;
  	// std::cout << cal(21, 10).get() << std::endl;
  	std::cout << cal(21, 0).get() << std::endl;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
