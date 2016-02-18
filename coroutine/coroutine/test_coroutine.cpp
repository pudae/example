
#include <iostream>
#include <thread>
#include <future>
#include <experimental/generator>
#include <experimental/resumable>

auto trace_coroutine() -> std::future<int>
{
  struct awaiter
  {
    bool await_ready()
    {
      std::cout << "awaiter::await_ready" << std::endl;
      return false;
    }

    void await_suspend(std::experimental::coroutine_handle<> h)
    {
      std::cout << "awaiter::await_suspend" << std::endl;
      h();
    }

    int await_resume()
    {
      std::cout << "awaiter::await_resume" << std::endl;
      return 1234;
    }
  };

  return await awaiter{};
}

auto trace_coroutine2() -> std::future<int>
{
  auto a = await trace_coroutine();
  auto b = await trace_coroutine();
  return a + b;
}

auto do_trace() -> std::future<void>
{
  for (int i = 0; i < 5; ++i)
    std::cout << await trace_coroutine2() << std::endl;
}

void test_coroutine()
{
  do_trace();
}