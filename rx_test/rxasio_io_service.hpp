#pragma once

#include "boost/asio.hpp"
#include "boost/date_time.hpp"
#include "rxcpp/rx-scheduler.hpp"

namespace rxasio {
namespace schedulers {

class strand_worker;

class io_service : public rxcpp::schedulers::scheduler_interface
{
private:
  // rxcpp types
  using composite_subscription = rxcpp::composite_subscription;
  using worker = rxcpp::schedulers::worker;
  using schedulable = rxcpp::schedulers::schedulable;

  boost::asio::io_service& io_service_;

  class strand_worker : public rxcpp::schedulers::worker_interface
	{
	private:
    using action_queue = rxcpp::schedulers::detail::action_queue;

	  mutable boost::asio::strand strand_;

	public:
	  explicit strand_worker(boost::asio::io_service& io_service)
	    : strand_(io_service)
	  {
	  }

	  virtual clock_type::time_point now() const
	  {
	    return clock_type::now();
	  }

	  virtual void schedule(const schedulable& scbl) const
	  {
      if (!scbl.is_subscribed())
        return;

      strand_.post([scbl] {
        if (scbl.is_subscribed())
        {
          rxcpp::schedulers::recursion r(true);
          scbl(r.get_recurse());
        }
      });
	  }

	  virtual void schedule(clock_type::time_point when,
	                        const schedulable& scbl) const
	  {
      if (!scbl.is_subscribed())
        return;

      auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(when - now());
      auto timer = std::make_shared<boost::asio::deadline_timer>(strand_.get_io_service());
      timer->expires_from_now(boost::posix_time::milliseconds(diff_ms.count()));
      timer->async_wait(strand_.wrap([timer, scbl](const boost::system::error_code& ec) {
        if (!ec && scbl.is_subscribed())
        {
          rxcpp::schedulers::recursion r(true);
          scbl(r.get_recurse());
        }
      }));
	  }
	};

public:
  explicit io_service(boost::asio::io_service& io_service)
    : io_service_(io_service)
  {
  }

  virtual clock_type::time_point now() const
  {
    return clock_type::now();
  }

  virtual worker create_worker(composite_subscription cs) const
  {
    return worker(std::move(cs), std::make_shared<strand_worker>(io_service_));
  }
};

inline rxcpp::schedulers::scheduler make_io_service(boost::asio::io_service& io_service)
{
  return rxcpp::schedulers::make_scheduler<rxasio::schedulers::io_service>(io_service);
}

}
}
