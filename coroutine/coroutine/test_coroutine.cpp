
#include <iostream>
#include <thread>
#include <future>
#include <experimental/generator>
#include <experimental/resumable>

template <typename F>
auto my_async(F f) -> std::future<int>
{
  struct awaiter
  {
    int result;
    F f;
    awaiter(F f) : f(std::move(f)) { }

    bool await_ready() { return false; }

    void await_suspend(std::experimental::coroutine_handle<> h)
    {
      std::thread{ [this, h] { result = f(); h(); } }.detach();
    }

    int await_resume() { return result; }
  };

  return await awaiter{f};
}

auto trace_coroutine2() -> std::future<int>
{
  auto a = await my_async([] { return 10 + 10; });
  auto b = await my_async([] { return 20 + 20; });
  return a + b;
}

auto do_trace() -> std::future<void>
{
  std::cout << await trace_coroutine2() << std::endl;
}

void test_coroutine()
{
  do_trace();
}