#include "boost/variant.hpp"

template <typename Handler>
class Dispatcher
{
public:
  Dispatcher(Handler& h) : imple_(h) { }

  template <typename MsgHolder>
  void operator()(MsgHolder holder)
  { boost::apply_visitor(imple_, holder);  }

private:
  struct Imple : public boost::static_visitor<void>
  {
    Imple(Handler &h) :  handler_(h) { }
  
    template <typename Msg> 
    void operator()(Msg msg) const { handler_(msg);}      
  
    Handler& handler_;
  } imple_;
};

template <typename Handler>
Dispatcher<Handler> make_dispatcher(Handler handler)
{ 
  return Dispatcher<Handler>(handler); 
}
