#pragma once

#include <functional>
#include <string>
#include <tuple>

#include "boost/asio.hpp"
#include "rxcpp/rx.hpp"

namespace rxasio {
namespace operators {

template <typename... Types>
struct callback_type
{
  typedef std::function<void(boost::system::error_code, Types...)> type;
};

template <typename... Types>
using callback_t = typename callback_type<Types...>::type;

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
class from_asio
{
public:
  using value_t     = value_t<Types...>;
  using callback_t  = callback_t<Types...>;
  using op_t        = std::function<void(callback_t)>;

  boost::asio::strand strand_;
  op_t op_;

  from_asio(boost::asio::io_service& io_service, op_t op)
    : strand_(io_service), op_(op)
  { }

  rxcpp::observable<value_t> operator()()
  {
    auto op = op_;
    auto strand = strand_;

    return rxcpp::observable<>::create<value_t>(
      [op, strand](rxcpp::subscriber<value_t> s) {
        op(handler(s));
        auto on_dispose = [] {
          std::cout << "on_dispose" << std::endl;
        };
        s.add(on_dispose);
    }).as_dynamic();
  }

  template <typename... T>
  rxcpp::observable<value_t> operator()(T... t)
  {
    auto op = op_;
    auto strand = strand_;

    return rxcpp::observable<>::create<value_t>(
      [=](rxcpp::subscriber<value_t> s) {
        op(std::forward<T>(t)..., handler(s));
        auto on_dispose = [] {
          std::cout << "on_dispose" << std::endl;
        };
        s.add(on_dispose);
    }).as_dynamic();
  }

private:
  struct handler
  {
    rxcpp::subscriber<value_t> subscriber;

    handler(rxcpp::subscriber<value_t> subscriber)
      : subscriber(subscriber)
    { }

    void operator()(boost::system::error_code ec)
    {
      if (ec)
      {
        // TODO : on_error 
        subscriber.on_completed();
      }
      else
      {
        subscriber.on_next(nullptr);
        subscriber.on_completed();
      }
    }

    template <typename T>
    void operator()(boost::system::error_code ec, T t)
    {
      if (ec)
      {
        // TODO : on_error 
        subscriber.on_completed();
      }
      else
      {
        subscriber.on_next(std::forward<T>(t));
        subscriber.on_completed();
      }
    }

    template <typename... T>
    void operator()(boost::system::error_code ec, T... t)
    {
      if (ec)
      {
        // TODO : on_error 
        subscriber.on_completed();
      }
      else
      {
        subscriber.on_next(std::make_tuple(std::forward<T>(t)...));
        subscriber.on_completed();
      }
    }

    template <typename Function>
    friend void asio_handler_invoke(Function function, handler* context)
    {
      function();
    }
  };
};

}
}
