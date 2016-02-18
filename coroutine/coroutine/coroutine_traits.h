#pragma once

#include <experimental/resumable>
#include <boost/thread/future.hpp>

namespace std {
namespace experimental {

template<class T, class... Args>
struct coroutine_traits<boost::shared_future<T>, Args...>
{
	struct promise_type
	{
		boost::promise<T> promise_;

		boost::shared_future<T> get_return_object()
		{
      return promise_.get_future().share();
		}

		bool initial_suspend() const
		{
			return false;
		}

		bool final_suspend() const
		{
			return false;
		}

		template<class U>
		void return_value(U&& value)
		{
			promise_.set_value(std::forward<U>(value));
		}

		void set_exception(std::exception_ptr ex)
		{
			promise_.set_exception(std::move(ex));
		}
	};
};

template<class... Args>
struct coroutine_traits<boost::shared_future<void>, Args...>
{
	struct promise_type
	{
		boost::promise<void> promise_;

		boost::shared_future<void> get_return_object()
		{
			return promise_.get_future().share();
		}

		bool initial_suspend() const
		{
			return false;
		}

		bool final_suspend() const
		{
			return false;
		}

		void return_void()
		{
			promise_.set_value();
		}

		void set_exception(std::exception_ptr ex)
		{
			promise_.set_exception(std::move(ex));
		}
	};
};

}	// namespace experimental
} // namespace std

namespace boost {

template<class T>
bool await_ready(boost::shared_future<T>& fut)
{
  return fut.is_ready();
}

template<class T>
void await_suspend(boost::shared_future<T>& fut, std::experimental::coroutine_handle<> resume)
{
  fut.then([resume](auto f) { 
    resume();
  });
}

template<class T>
auto await_resume(boost::shared_future<T>& fut)
{
  try
  {
    return fut.get();
  }
  /// TODO : 올바르게 처리하는 방법 찾기
  catch (boost::exception_detail::clone_impl<std::exception_ptr>& e)
  {
    std::rethrow_exception(e);
  }
}

} // namespace boost
