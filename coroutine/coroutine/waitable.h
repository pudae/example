#pragma once

#include <mutex>
#include <experimental/resumable>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/optional.hpp>

namespace myco {

template <typename Allocator = std::allocator<void> >
class use_waitable_t
{
public:
  typedef Allocator allocator_type;

  use_waitable_t()
  {
  }

  explicit use_waitable_t(const Allocator& allocator)
  {
  }

  allocator_type get_allocator() const
  {
    return allocator_;
  }

private:
  Allocator allocator_;
};

__declspec(selectany) use_waitable_t<> use_waitable;

template <typename T>
class waitable_handler
{
public:
  struct state_t
  {
    std::recursive_mutex m;
    std::experimental::coroutine_handle<> resume;
  	boost::optional<T> t;
  	std::exception_ptr ex;
  };

  struct awaiter
  {
    std::shared_ptr<state_t> s;

    awaiter(const std::shared_ptr<state_t>& s)
      : s(s)
    {
    }

    bool await_ready()
    { 
      std::lock_guard<std::recursive_mutex> l(s->m);
      return s->t || s->ex;
    }

	  void await_suspend(std::experimental::coroutine_handle<> h)
	  {
      std::lock_guard<std::recursive_mutex> l(s->m);
      if (s->t)
        h();
      else if (s->ex)
        std::rethrow_exception(s->ex);
      else
        s->resume = h;
	  }

	  T await_resume()
	  {
      std::lock_guard<std::recursive_mutex> l(s->m);
      if (s->t)
        return s->t.get();
      std::rethrow_exception(s->ex);
	  }
  };

  std::shared_ptr<state_t> state_;
  awaiter awaiter_;

  template <typename Allocator>
  waitable_handler(use_waitable_t<Allocator> /* uf */)
    : state_(std::make_shared<state_t>()), awaiter_(state_)
  {
  }

  void operator()(T t)
  {
    std::lock_guard<std::recursive_mutex> l(state_->m);
    state_->t = t;
    if (state_->resume)
      state->resume();
  }

  void operator()(const boost::system::error_code& ec, T t)
  {
    std::lock_guard<std::recursive_mutex> l(state_->m);
    if (ec)
      state_->ex = std::make_exception_ptr(boost::system::system_error(ec));
    else
      state_->t = t;
    if (state_->resume)
      state_->resume();
  }
};

// Completion handler to adapt a void promise as a completion handler.
template <>
class waitable_handler<void>
{
public:
  struct state_t
  {
    std::recursive_mutex m;
    std::experimental::coroutine_handle<> resume;
    bool completed = false;
  	std::exception_ptr ex;
  };

  struct awaiter
  {
    std::shared_ptr<state_t> s;

    awaiter(const std::shared_ptr<state_t>& s)
      : s(s)
    {
    }

    bool await_ready()
    { 
      std::lock_guard<std::recursive_mutex> l(s->m);
      return s->completed || s->ex;
    }

	  void await_suspend(std::experimental::coroutine_handle<> h)
	  {
      std::lock_guard<std::recursive_mutex> l(s->m);
      if (s->completed)
        h();
      else if (s->ex)
        std::rethrow_exception(s->ex);
      else
        s->resume = h;
	  }

	  void await_resume()
	  {
      std::lock_guard<std::recursive_mutex> l(s->m);
      if (s->completed)
        return;
      if (s->ex)
        std::rethrow_exception(s->ex);
	  }
  };

  std::shared_ptr<state_t> state_;
  awaiter awaiter_;

  template <typename Allocator>
  waitable_handler(use_waitable_t<Allocator> uf)
    : state_(std::make_shared<state_t>()), awaiter_(state_)
  {
  }

  void operator()()
  {
    std::lock_guard<std::recursive_mutex> l(state_->m);
    state_->completed = true;
    if (state_->resume)
      state_->resume();
  }

  void operator()(const boost::system::error_code& ec)
  {
    std::lock_guard<std::recursive_mutex> l(state_->m);
    if (ec)
      state_->ex = std::make_exception_ptr(boost::system::system_error(ec));
    state_->completed = true;
    if (state_->resume)
      state_->resume();
  }
};

}

namespace boost {
namespace asio {

template <typename T>
class async_result<myco::waitable_handler<T> >
{
public:
  using type = typename myco::waitable_handler<T>::awaiter;
  typename type w;

  explicit async_result(myco::waitable_handler<T>& h)
    : w(h.awaiter_)
  {
  }

  type get() { return std::move(w); }
};

template <typename Allocator, typename ReturnType>
struct handler_type<myco::use_waitable_t<Allocator>, ReturnType()>
{
  typedef myco::waitable_handler<void> type;
};

template <typename Allocator, typename ReturnType, typename Arg1>
struct handler_type<myco::use_waitable_t<Allocator>, ReturnType(Arg1)>
{
  typedef myco::waitable_handler<Arg1> type;
};

template <typename Allocator, typename ReturnType>
struct handler_type<myco::use_waitable_t<Allocator>,
    ReturnType(boost::system::error_code)>
{
  typedef myco::waitable_handler<void> type;
};

template <typename Allocator, typename ReturnType, typename Arg2>
struct handler_type<myco::use_waitable_t<Allocator>,
    ReturnType(boost::system::error_code, Arg2)>
{
  typedef myco::waitable_handler<Arg2> type;
};

}
}
