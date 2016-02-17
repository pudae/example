#include <iostream>
#include <future>
#include <memory>
#include <boost/asio.hpp>

#include "waitable.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  auto start() -> std::future<void>
  {
    auto self(shared_from_this());
    try
    {
      for (;;)
      {
        auto read_bytes = await socket_.async_read_some(asio::buffer(data_, max_length), myco::use_waitable);
        await asio::async_write(socket_, asio::buffer(data_, read_bytes), myco::use_waitable);
      }
    }
    catch (std::exception& ex)
    {
      std::cout << "Exception: " << ex.what() << "\n";
    }
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(asio::io_service& io_service, short port)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
  }

  auto do_accept() -> std::future<void>
  {
    for (;;)
    {
      tcp::socket socket(acceptor_.get_io_service());
      await acceptor_.async_accept(socket, myco::use_waitable);
      std::make_shared<session>(std::move(socket))->start();
    }
  }

  tcp::acceptor acceptor_;
};

int start_echo_server(int port)
{
  std::cout << "start echo server\n";
  try
  {
    asio::io_service io_service;

    server s(io_service, port);
    s.do_accept();

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}