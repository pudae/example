#include <iostream>

#include "messages.hpp"

class MessageHandler
{
public:
  void handle(MsgA& msg) { std::cout << msg.name() << std::endl; }
  void handle(MsgB& msg) { std::cout << msg.name() << std::endl; }
  void handle(MsgC& msg) { std::cout << msg.name() << std::endl; }
};

template <typename T>
void deserialize(const std::string&, T& msg);
{ 
  std::cout << "deserialize " << msg.name() << std::endl;
}

MsgHolder deserialize(TypeId id, const std::string& buffer)
{
  MsgHolder ret;

}

//============================================================
// handler_adaptoer.hpp 
// 핸들러 adapter - (in New App code)
#ifdef CPP14_GENERIC_LAMBDA_SUPPORT

  auto make_handler = [](auto& handler)
  {  return [&handler](auto msg) { handler.OnRecvFuncName(msg); };  };  
  
#else // !CPP14_GENERIC_LAMBDA_SUPPORT

  template <typename UserHandler>
  struct HandlerAdaptor 
  {
      HandlerAdaptor(UserHandler &uh) : uh_(uh) {};
      UserHandler& uh_;
      
      template<typename Msg>
      void operator()(Msg msg) { uh_.OnRecvFuncName(msg); }      
  };
  
  template <typename UserHandler>
  HandlerAdaptor<UserHandler> make_handler(UserHandler&  uh)
  { return HandlerAdaptor<UserHandler>(uh); }
  
#endif // end of CPP14_GENERIC_LAMBDA_SUPPORT

//============================================================
// test.cpp
// 테스트 (in new App code)
int main()
{
  MessageHandler handler_original;

  auto dispatcher = make_dispatcher(make_handler(handler_original));

  auto msg_a = deserializer(TypeId::kMsgA);
  dispatcher(msg_a);
  // Output : MsgA

  auto msg_b = deserializer(TypeId::kMsgB);
  dispatcher(msg_b);
  // Output : MsgB

  auto msg_c = deserializer(TypeId::kMsgC);
  dispatcher(msg_c);
  // Output : MsgC

  return 0;
}
