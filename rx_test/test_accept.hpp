#pragma once

#include <iostream>
#include <thread>

#include "rxcpp/rx.hpp"
#include "rxasio/io_service.hpp"
#include "rxasio/from_asio.hpp"


namespace test {

using boost::asio::ip::tcp;

template <typename Value, typename Op>
rxcpp::observable<Value>
repeat_async(rxcpp::schedulers::worker worker, Op op)
{
  return rxcpp::observable<>::create<Value>([=](rxcpp::subscriber<Value> s) {
    worker.schedule([=](rxcpp::schedulers::schedulable scbl) {
      op().subscribe(
        [=](Value v) { s.on_next(v); },
        [=](std::exception_ptr e) { s.on_error(e); },
        [=]() {
          if (scbl.is_subscribed())
        	  scbl.schedule();
        }
      );
    });
  });
}

rxcpp::observable<std::shared_ptr<tcp::socket>>
async_accept(rxcpp::schedulers::worker worker, std::shared_ptr<tcp::acceptor> acceptor)
{
  using rxasio::from_asio;
  using socket_ptr = std::shared_ptr<tcp::socket>;

  auto socket = std::make_shared<tcp::socket>(acceptor->get_io_service());
  auto observable = from_asio(worker, [=](auto handler) { 
    acceptor->async_accept(*socket, handler);
  });
  return observable().map([=](nullptr_t v) { return socket; }).as_dynamic();
}

rxcpp::observable<std::shared_ptr<tcp::socket>>
to_observable(rxcpp::schedulers::worker worker, std::shared_ptr<tcp::acceptor> acceptor)
{
  using socket_ptr = std::shared_ptr<tcp::socket>;
  return repeat_async<socket_ptr>(worker, [=]() {return async_accept(worker, acceptor); });
}

void test_accept()
{
  std::cout << "Starting on threadId: " << std::this_thread::get_id() << std::endl;

  boost::asio::io_service io_service;

  auto scheduler = rxasio::make_io_service(io_service);
  auto acceptor = std::make_shared<tcp::acceptor>(io_service, tcp::endpoint(tcp::v4(), 8989));
  auto worker = scheduler.create_worker();
  auto observable = to_observable(worker, acceptor);

  auto subscription = observable.subscribe(
    [](auto socket) { std::cout << socket->remote_endpoint().address().to_string() << " is connected. " << std::endl; },
    [](auto error) { std::cout << "error!!!" << std::endl; },
    []() { std::cout << "on_completed." << std::endl; }
  );

  std::array<std::thread, 2> thrs{ std::thread([&] { io_service.run(); }),
                                   std::thread([&] { io_service.run(); })};

  std::cout << "Wait..." << std::endl;
  std::getchar();
  std::cout << "Close..." << std::endl;
  worker.schedule([=](auto s) { acceptor->close(); });
  std::getchar();
  subscription.unsubscribe();
  std::cout << "Worker1 is unsubscribed..." << std::endl;
  std::getchar();

  std::for_each(thrs.begin(), thrs.end(), std::mem_fn(&std::thread::join));
}

}