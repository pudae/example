#pragma once

#include <functional>
#include "boost/asio.hpp"
#include "rxcpp/rx.hpp"

namespace rxasio {
namespace operators {

// for deadline timer
class from_asio0
{
public:
  using op_t = std::function<void(std::function<void(boost::system::error_code) >)> ;

  boost::asio::strand strand_;
  op_t op_;

  from_asio0(boost::asio::strand strand, op_t op)
    : strand_(strand), op_(op)
  { }

  rxcpp::observable<int> operator()()
  {
    auto strand = std::make_shared<boost::asio::strand>(strand_);
    auto op = op_;

    return rxcpp::observable<>::create<int>(
      [strand, op](rxcpp::subscriber<int> s) {
        op(strand->wrap(handler(s)));
        auto on_dispose = [] {
          std::cout << "on_dispose" << std::endl;
        };
        s.add(on_dispose);
    }).as_dynamic();
  }

private:
  struct handler
  {
    handler(rxcpp::subscriber<int> subscriber)
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
        subscriber.on_next(1234);
        subscriber.on_completed();
      }
    }

    rxcpp::subscriber<int> subscriber;
  };
};

}
}