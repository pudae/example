// coroutine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <future>
#include <experimental/generator>
#include <experimental/resumable>

#include "echo.h"
#include "test_coroutine.h"

auto hello(const char* p)
{
  while (*p)
  {
    yield *p++;
  }
}

void test_expected();

int main()
{
  std::cout << "Hello, world\n";
  for (auto c : hello("Hello, coroutines\n"))
    std::cout << c;

  test_expected();
  // test_coroutine();

  start_echo_server(20001);

  return 0;
}