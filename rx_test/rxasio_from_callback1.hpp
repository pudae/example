#pragma once

#include <functional>
#include "boost/asio.hpp"
#include "rxcpp/rx.hpp"

namespace rxasio {
namespace operators {

// for deadline timer
template <typename T>
class from_asio1
{
public:
  using op_t = std::function<void(std::function<void(boost::system::error_code, T) >)> ;

  op_t op_;

  from_asio1(op_t op)
    : op_(op)
  { }

  rxcpp::observable<T> operator()()
  {
    auto op = op_;

    return rxcpp::observable<>::create<T>(
      [op](rxcpp::subscriber<T> s) {
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
    handler(rxcpp::subscriber<T> subscriber)
      : subscriber(subscriber)
    { }

    void operator()(boost::system::error_code ec, T t)
    {
      if (ec)
      {
        // TODO : on_error 
        subscriber.on_completed();
      }
      else
      {
        subscriber.on_next(t);
        subscriber.on_completed();
      }
    }

    rxcpp::subscriber<T> subscriber;
  };
};

}
}
