#include <iostream>
#include <future>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "use_boost_future.h"
#include "coroutine_traits.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

auto do_echo(tcp::socket socket) -> boost::shared_future<void>
{
  try
  {
    for (;;)
    {
      const std::size_t max_length = 1024;
      char data[max_length];

      auto read_bytes = await socket.async_read_some(asio::buffer(data, max_length), myco::use_boost_future);
      await asio::async_write(socket, asio::buffer(data, read_bytes), myco::use_boost_future);
    }
  }
  catch (const std::exception& e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}

auto do_accept(asio::io_service& io_service, short port) -> boost::shared_future<void>
{
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    tcp::socket socket(io_service);
    await acceptor.async_accept(socket, myco::use_boost_future);
    do_echo(std::move(socket));
  }
}

auto do_tick(asio::io_service& io_service, int ms) -> boost::shared_future<void>
{
  asio::deadline_timer t(io_service);
  for (;;)
  {
    t.expires_from_now(boost::posix_time::milliseconds(ms));
    await t.async_wait(myco::use_boost_future);
    std::cout << "tick \n";
  }
}

int start_echo_server(short port)
{
  std::cout << "start simple echo server\n";
  try
  {
    asio::io_service io_service;
    do_accept(io_service, port);
    do_tick(io_service, 1000);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
