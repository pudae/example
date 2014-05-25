#pragma once

#include "boost/variant.hpp"

#include "type_table.hpp"

namespace windup 
{

template <typename MsgHolder>
class Serializer
{
public:
  typedef typename MsgHolder::types Types;
  typedef decltype(Trait<typename boost::mpl::front<Types>::type>::id()) TypeId;

  static MsgHolder deserialize_tmp(TypeId id, const std::string& buffer)
  {
    auto msg_holder = TypeTable<MsgHolder>::type(id);
    return boost::apply_visitor(Impl(buffer), msg_holder);
  } 

private:
  struct Impl : public boost::static_visitor<MsgHolder>
  {
    Impl(const std::string& s) : buffer(s) { }

    template <typename Msg> 
    MsgHolder operator()(const Msg&) const 
    { 
      Msg msg;
      deserialize(buffer, &msg);

      MsgHolder ret = msg;
      return ret;
    }
  
    const std::string& buffer;
  };
};

}  // windup 
