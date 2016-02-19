
#include <iostream>
#include <thread>
#include <future>
#include <experimental/resumable>
#include <experimental/generator>

auto async_quadruple(int n) -> std::future<int>
{
  auto x = await std::async([n] { return n + n; });
  auto y = await std::async([n] { return n + n; });
  return x + y;
}

void simple_async()
{
  std::cout << async_quadruple(3).get() << std::endl;
}