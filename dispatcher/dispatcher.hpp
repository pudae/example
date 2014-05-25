#pragma once

#include "boost/variant.hpp"

namespace windup 
{

template <typename Handler>
class Dispatcher
{
public:
  Dispatcher(Handler& h) : impl_(h) { }

  template <typename MsgHolder>
  void operator()(MsgHolder holder)
  { boost::apply_visitor(impl_, holder);  }

private:
  struct Impl : public boost::static_visitor<void>
  {
    Impl(Handler &h) : handler_(h) { }
  
    template <typename Msg> 
    void operator()(Msg msg) const { handler_(msg);}      
  
    Handler& handler_;
  } impl_;
};

template <typename Handler>
Dispatcher<Handler> make_dispatcher(Handler handler)
{ 
  return Dispatcher<Handler>(handler); 
}
  
}  // windup 
