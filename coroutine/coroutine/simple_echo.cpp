#include <iostream>
#include <future>
#include <boost/asio.hpp>

#include "waitable.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

auto do_echo(tcp::socket socket) -> std::future<void>
{
  try
  {
    for (;;)
    {
      const std::size_t max_length = 1024;
      char data[max_length];

      auto read_bytes = await socket.async_read_some(asio::buffer(data, max_length), myco::use_waitable);
      await asio::async_write(socket, asio::buffer(data, read_bytes), myco::use_waitable);
    }
  }
  catch (std::exception& ex)
  {
    std::cout << "Exception: " << ex.what() << "\n";
  }
}

auto do_accept(asio::io_service& io_service, short port) -> std::future<void>
{
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    tcp::socket socket(io_service);
    await acceptor.async_accept(socket, myco::use_waitable);
    do_echo(std::move(socket));
  }
}

int start_simple_echo_server(short port)
{
  std::cout << "start simple echo server\n";
  try
  {
    asio::io_service io_service;
    do_accept(io_service, port);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
