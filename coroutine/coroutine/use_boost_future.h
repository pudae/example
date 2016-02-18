#pragma once

#include <mutex>
#include <experimental/resumable>

#include <boost/asio/use_future.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread/future.hpp>

namespace myco {

template <typename Allocator = std::allocator<void> >
class use_boost_future_t
{
public:
  typedef Allocator allocator_type;

  use_boost_future_t()
  {
  }

  explicit use_boost_future_t(const Allocator& allocator)
  {
  }

  allocator_type get_allocator() const
  {
    return allocator_;
  }

private:
  Allocator allocator_;
};

__declspec(selectany) use_boost_future_t<> use_boost_future;

template <typename T>
class boost_promise_handler
{
public:
  template <typename Allocator>
  boost_promise_handler(use_boost_future_t<Allocator> /*uf*/)
    : promise_(std::make_shared<boost::promise<T>>())
  {
  }

  void operator()(T t)
  {
    promise_->set_value(t);
  }

  void operator()(const boost::system::error_code& ec, T t)
  {
    if (ec)
      promise_->set_exception(
          std::make_exception_ptr(
            boost::system::system_error(ec)));
    else
      promise_->set_value(t);
  }

  std::shared_ptr<boost::promise<T> > promise_;
};

template <>
class boost_promise_handler<void>
{
public:
  template <typename Allocator>
  boost_promise_handler(use_boost_future_t<Allocator> /*uf*/)
    : promise_(std::make_shared<boost::promise<void>>())
  {
  }

  void operator()()
  {
    promise_->set_value();
  }

  void operator()(const boost::system::error_code& ec)
  {
    if (ec)
      promise_->set_exception(
          std::make_exception_ptr(
            boost::system::system_error(ec)));
    else
      promise_->set_value();
  }

  std::shared_ptr<boost::promise<void> > promise_;
};

template <typename Function, typename T>
void asio_handler_invoke(Function f, boost_promise_handler<T>* h)
{
  std::shared_ptr<boost::promise<T> > p(h->promise_);
  try
  {
    f();
  }
  catch (...)
  {
    p->set_exception(std::current_exception());
  }
}

} // namespace myco

namespace boost {
namespace asio {

template <typename T>
class async_result<myco::boost_promise_handler<T> >
{
public:
  typedef boost::shared_future<T> type;

  explicit async_result(myco::boost_promise_handler<T>& h)
  {
    value_ = h.promise_->get_future();
  }

  /// boost asio 비동기 함수의 콜백으로 use_boost_future를 전달하면 이 함수의 결과 타입이 반환된다.
  /// 예를 들어, 다음과 같은 코드에서 f의 타입은 boost::shared_future<void> 이다.
  ///
  /// tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
  /// tcp::socket socket(io_service);
  /// auto f = acceptor.async_accept(socket, myco::use_waitable);
  ///
  type get() 
  { 
    return std::move(value_);
  }

private:
  type value_;
};

template <typename Allocator, typename ReturnType>
struct handler_type<myco::use_boost_future_t<Allocator>, ReturnType()>
{
  typedef myco::boost_promise_handler<void> type;
};

template <typename Allocator, typename ReturnType, typename Arg1>
struct handler_type<myco::use_boost_future_t<Allocator>, ReturnType(Arg1)>
{
  typedef myco::boost_promise_handler<Arg1> type;
};

template <typename Allocator, typename ReturnType>
struct handler_type<myco::use_boost_future_t<Allocator>,
    ReturnType(boost::system::error_code)>
{
  typedef myco::boost_promise_handler<void> type;
};

template <typename Allocator, typename ReturnType, typename Arg2>
struct handler_type<myco::use_boost_future_t<Allocator>,
    ReturnType(boost::system::error_code, Arg2)>
{
  typedef myco::boost_promise_handler<Arg2> type;
};

} // namespace asio
} // namespace boost
