#include <iostream>

#define BOOST_TEST_MODULE MyTest
#include "boost/test/unit_test.hpp"

#include "windup/dispatcher.hpp"
#include "messages.hpp"

using namespace windup;

namespace 
{

std::string s_name;
  
class MessageHandler
{
public:
  void handle(MsgA& msg) { s_name = msg.name(); }
  void handle(MsgB& msg) { s_name = msg.name(); }
  void handle(MsgC& msg) { s_name = msg.name(); }

  const std::string& dispatched_name() const { return name_; }

private:
  std::string name_;
};

//============================================================
// handler_adaptoer.hpp 
// 핸들러 adapter - (in New App code)
#ifdef CPP14_GENERIC_LAMBDA_SUPPORT

  auto make_handler = [](auto& handler)
  {  return [&handler](auto msg) { handler.handle(msg); };  };  
  
#else // !CPP14_GENERIC_LAMBDA_SUPPORT

  template <typename UserHandler>
  struct HandlerAdaptor 
  {
      HandlerAdaptor(UserHandler &uh) : uh_(uh) {};
      UserHandler& uh_;
      
      template<typename Msg>
      void operator()(Msg msg) { uh_.handle(msg); }      
  };
  
  template <typename UserHandler>
  HandlerAdaptor<UserHandler> make_handler(UserHandler&  uh)
  { return HandlerAdaptor<UserHandler>(uh); }
  
#endif // end of CPP14_GENERIC_LAMBDA_SUPPORT

} 

BOOST_AUTO_TEST_CASE(dispatcher)
{
  MessageHandler handler;

  auto dispatcher = make_dispatcher(make_handler(handler));

  MsgHolder msg_a = MsgA();
  dispatcher(msg_a);
  BOOST_REQUIRE(s_name == "MsgA");

  MsgHolder msg_b = MsgB();
  dispatcher(msg_b);
  BOOST_REQUIRE(s_name == "MsgB");

  MsgHolder msg_c = MsgC();
  dispatcher(msg_c);
  BOOST_REQUIRE(s_name == "MsgC");
}
