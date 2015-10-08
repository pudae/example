#pragma once

#include <functional>
#include <string>
#include <tuple>

#include "boost/asio.hpp"
#include "rxcpp/rx.hpp"

namespace rxasio {
namespace detail {

template <typename... T>
struct value_type;

template <>
struct value_type<>
{
  typedef nullptr_t type;
};

template <typename T>
struct value_type<T>
{
  typedef T type;
};

template <typename Head, typename... Tail>
struct value_type<Head, Tail...>
{
  typedef std::tuple<Head, Tail...> type;
};

template <typename... T>
using value_t = typename value_type<T...>::type;

template <typename... Types>
struct asio_wrapper
{
  using value_t     = value_t<Types...>;

  rxcpp::schedulers::worker worker;
  rxcpp::subscriber<value_t> subscriber;

  asio_wrapper(rxcpp::schedulers::worker worker, rxcpp::subscriber<value_t> subscriber)
    : worker(worker), subscriber(subscriber)
  { }

  void operator()(boost::system::error_code ec) const
  {
    if (ec)
    {
      // std::error_code sec(ec.value(), std::system_category());
      // TODO : on_error 
      try {
        std::string msg = "huk!! " + ec.message();
        throw std::exception(msg.c_str());
      }
      catch (...) {
        subscriber.on_error(std::current_exception());
      }
    }
    else
    {
      subscriber.on_next(nullptr);
      subscriber.on_completed();
    }
  }

  template <typename T>
  void operator()(boost::system::error_code ec, T t) const
  {
    if (ec)
    {
      // TODO : on_error 
      try {
        std::string msg = "huk!! " + ec.message();
        throw std::exception(msg.c_str());
      }
      catch (...) {
        subscriber.on_error(std::current_exception());
      }
    }
    else
    {
      subscriber.on_next(std::forward<T>(t));
      subscriber.on_completed();
    }
  }

  template <typename... T>
  void operator()(boost::system::error_code ec, T... t) const
  {
    if (ec)
    {
      // TODO : on_error 
      try {
        std::string msg = "huk!! " + ec.message();
        throw std::exception(msg.c_str());
      }
      catch (...) {
        subscriber.on_error(std::current_exception());
      }
    }
    else
    {
      subscriber.on_next(std::make_tuple(std::forward<T>(t)...));
      subscriber.on_completed();
    }
  }

  template <typename Function>
  friend void asio_handler_invoke(Function function, asio_wrapper* context)
  {
    std::cout << "dispatch ------------- " << std::endl;
    context->worker.schedule([=](const rxcpp::schedulers::schedulable& scbl) { function(); });
  }
};
}

template <typename Op>
auto from_asio(rxcpp::schedulers::worker worker, Op op)
{
  using value_t = nullptr_t;
  return [=](auto ...args) {
    return rxcpp::observable<>::create<value_t>(
      [=](rxcpp::subscriber<value_t> s) {
        op(args..., detail::asio_wrapper<value_t>(worker, s));
        auto on_dispose = [] {
          std::cout << "on_dispose" << std::endl;
        };
        s.add(on_dispose);
    }).as_dynamic();
  };
}

template <typename T, typename Op>
auto from_asio(rxcpp::schedulers::worker worker, Op op)
{
  using value_t = T;
  return [=](auto ...args) {
    return rxcpp::observable<>::create<value_t>(
      [=](rxcpp::subscriber<value_t> s) {
        op(args..., detail::asio_wrapper<value_t>(worker, s));
      	auto on_dispose = [] {
      	  std::cout << "on_dispose" << std::endl;
      	};
      	s.add(on_dispose);
    }).as_dynamic();
  };
}

template <typename T1, typename T2, typename Op>
auto from_asio(rxcpp::schedulers::worker worker, Op op)
{
  using value_t = std::tuple<T1, T2>;
  return [=](auto ...args) {
    return rxcpp::observable<>::create<value_t>(
      [=](rxcpp::subscriber<value_t> s) {
        op(args..., detail::asio_wrapper<value_t>(worker, s));
      	auto on_dispose = [] {
      	  std::cout << "on_dispose" << std::endl;
      	};
      	s.add(on_dispose);
    }).as_dynamic();
  };
}

}

