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
  typedef void type;
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

inline void test()
{
  {
    // using param_t = std::tuple<int, std::string, double>;
  	// std::cout << std::tuple_size<param_t>::value << std::endl;
  	// callback_t<param_t>::type f = [](boost::system::error_code ec, int  i, std::string s, double d) {
  	//   std::cout << ec.message() << ", " << i << ", " << s << ", " << d << std::endl;
  	// };

  	// f(boost::system::error_code(), 0, "hahaha", 0.3);
  }

  {
    callback_t<> f = [](boost::system::error_code ec) {
      std::cout << ec.message() << std::endl;
  	};
    f(boost::system::error_code());
  }


  {
    callback_t<int> f = [](boost::system::error_code ec, int i) {
      std::cout << ec.message() << ", " << i << std::endl;
  	};
    f(boost::system::error_code(), 1);
  }

  {
    callback_t<int, double> f = [](boost::system::error_code ec, int i, double d) {
      std::cout << ec.message() << ", " << i << ", " << d << std::endl;
  	};
    f(boost::system::error_code(), 1, 2.3);
  }
}

#define AAA
#ifdef AAA
template <typename... Types>
class from_asio
{
public:
  using value_t     = value_t<Types...>;
  using callback_t  = callback_t<Types...>;
  using op_t        = std::function<void(callback_t)>;
  op_t op_;

  from_asio(op_t op)
    : op_(op)
  { }

  rxcpp::observable<value_t> operator()()
  {
    auto op = op_;

    return rxcpp::observable<>::create<value_t>(
      [op](rxcpp::subscriber<value_t> s) {
        op(handler(s));
        auto on_dispose = [] {
          std::cout << "on_dispose" << std::endl;
        };
        s.add(on_dispose);
    }).as_dynamic();
  }

private:
  struct handler
  {
    handler(rxcpp::subscriber<value_t> subscriber)
      : subscriber(subscriber)
    { }

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

    rxcpp::subscriber<value_t> subscriber;
  };
};
#endif

}
}
