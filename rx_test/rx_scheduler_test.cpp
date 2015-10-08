// rx_scheduler_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>

#include "rxcpp/rx.hpp"
#include "rxasio_io_service.hpp"
#include "rxasio_from_callback.hpp"
#include "rxasio_from_callback1.hpp"
#include "rxasio_from_callback_generic.hpp"

namespace test {

namespace rxsc = rxcpp::schedulers;

void run1()
{
  std::cout << "Starting on threadId: " << std::this_thread::get_id() << std::endl;

  boost::asio::io_service io_service;
  auto& scheduler = rxasio::schedulers::make_io_service(io_service);

  auto worker = scheduler.create_worker();

  auto counter = 0;
  worker.schedule([&counter](const rxsc::schedulable& scbl) {
    std::cout << "Do 1 on threadId: " << std::this_thread::get_id() << ", Counter: " << counter++ << std::endl;
    scbl.schedule();
    std::cout << "Do 1 after calling schedule" << std::endl;
  });

  auto thr1 = std::thread([&] { io_service.run(); });
  auto thr2 = std::thread([&] { io_service.run(); });

  std::cout << "Wait..." << std::endl;
  std::getchar();
}

void run2()
{
  std::cout << "Starting on threadId: " << std::this_thread::get_id() << std::endl;

  boost::asio::io_service io_service;
  auto& scheduler = rxasio::schedulers::make_io_service(io_service);

  auto worker1 = scheduler.create_worker();
  auto worker2 = scheduler.create_worker();

  {
    auto start = worker1.now() + std::chrono::seconds(2);
  	auto period = std::chrono::seconds(1);
  	auto count = int(0);
  	worker1.schedule_periodically(start, period,
  	  [=, &count](const rxsc::schedulable& scbl) {
  	    auto nsDelta = std::chrono::duration_cast<std::chrono::milliseconds>(scbl.now() - (start + period * count));
  	    ++count;
  	    std::cout << "schedule_periodically: period " << count << ", " << nsDelta.count() << "ms delta from target time. on threadId " << std::this_thread::get_id() << std::endl;
  	});
  }

  {
    auto count = int(0);
  	worker1.schedule([&count](const rxsc::schedulable& scbl) {
      std::cout << "schedule: " << count << " on threadId " << std::this_thread::get_id() << std::endl;
      ++count;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      scbl.schedule();
  	});
  }

  std::array<std::thread, 2> thrs{ std::thread([&] { io_service.run(); }),
                                   std::thread([&] { io_service.run(); })};

  std::cout << "Wait..." << std::endl;
  std::getchar();
  worker1.unsubscribe();
  std::cout << "Worker1 is unsubscribed..." << std::endl;
  std::getchar();

  std::for_each(thrs.begin(), thrs.end(), std::mem_fn(&std::thread::join));
}

void run3()
{
  using rxasio::operators::from_asio;

  std::cout << "Starting on threadId: " << std::this_thread::get_id() << std::endl;

  boost::asio::io_service io_service;
  auto& scheduler = rxasio::schedulers::make_io_service(io_service);

  auto worker1 = scheduler.create_worker();

  auto timer = std::make_shared<boost::asio::deadline_timer>(io_service);
  timer->expires_from_now(boost::posix_time::milliseconds(3000));
  auto observable = from_asio<>(io_service, [timer](auto handler) { timer->async_wait(handler); });

  observable.operator()().subscribe(
    [](nullptr_t) { std::cout << "on_next: " << std::endl; },
    [](auto e) { std::cout << "on_error: " << std::endl; },
    []() { std::cout << "on_completed " << std::endl; });

  std::array<std::thread, 2> thrs{ std::thread([&] { io_service.run(); }),
                                   std::thread([&] { io_service.run(); })};

  std::cout << "Wait..." << std::endl;
  std::getchar();
  worker1.unsubscribe();
  std::cout << "Worker1 is unsubscribed..." << std::endl;
  std::getchar();

  std::for_each(thrs.begin(), thrs.end(), std::mem_fn(&std::thread::join));
}

using boost::asio::ip::tcp;
void async_accept(std::shared_ptr<tcp::acceptor> acceptor, 
                  std::function<void(boost::system::error_code,
                                     std::shared_ptr<tcp::socket>)> handler)
{
  auto socket = std::make_shared<tcp::socket>(acceptor->get_io_service());
  acceptor->async_accept(
    *socket, 
    [socket, handler] (boost::system::error_code ec) {
    handler(ec, socket);
  });
}

rxcpp::observable<std::shared_ptr<tcp::socket>>
create_acceptor(boost::asio::io_service& io_service, int port)
{
  // TOOD : thread issue

  using rxasio::operators::from_asio;
  using socket_ptr = std::shared_ptr<tcp::socket>;

  auto& scheduler = rxasio::schedulers::make_io_service(io_service);
  auto worker = scheduler.create_worker();

  return rxcpp::observable<>::create<socket_ptr>([&io_service, worker, port](rxcpp::subscriber<socket_ptr> s) {
    auto acceptor = std::make_shared<tcp::acceptor>(io_service, tcp::endpoint(tcp::v4(), port));
  	auto acceptor_op = [&io_service, acceptor] () {
  	  return from_asio<socket_ptr>(io_service, std::bind(async_accept, acceptor, std::placeholders::_1))();
  	};

    rxcpp::subscription subscription;
    worker.schedule([s, acceptor, acceptor_op, &subscription](const rxsc::schedulable& scbl) {
      auto on_next = [s](auto socket) { 
        s.on_next(socket);
  		};
  		auto on_error = [s](auto e) { 
        // s.on_error(e);
  		};
    	auto on_completed = [scbl]() {
    	  if (scbl.is_subscribed())
    	    scbl.schedule();
    	};

      subscription = acceptor_op().subscribe(on_next, on_error, on_completed);
    });

    s.add(worker.get_subscription());
    s.add(subscription);
    s.add([acceptor] { acceptor->close(); });
  });
}

void run4()
{
  std::cout << "Starting on threadId: " << std::this_thread::get_id() << std::endl;

  boost::asio::io_service io_service;

  auto observable = create_acceptor(io_service, 8989);
  auto subscription = observable.subscribe(
    [](auto socket) { std::cout << socket->remote_endpoint().address().to_string() << " is connected. " << std::endl; },
    [](auto error) { std::cout << "error!!!" << std::endl; },
    []() { std::cout << "on_completed." << std::endl; }
  );

  std::array<std::thread, 2> thrs{ std::thread([&] { io_service.run(); }),
                                   std::thread([&] { io_service.run(); })};

  std::cout << "Wait..." << std::endl;
  std::getchar();
  subscription.unsubscribe();
  std::cout << "Worker1 is unsubscribed..." << std::endl;
  std::getchar();

  std::for_each(thrs.begin(), thrs.end(), std::mem_fn(&std::thread::join));
}

}


int main()
{
  // test::run1();
  // test::run2();
  test::run3();
  test::run4();
  return 0;
}

