#pragma once

#include "boost/variant.hpp"

namespace windup
{
namespace detail
{

template <typename MsgHolder>
class Deserializer : public boost::static_visitor<MsgHolder>
{
public:
  Deserializer(const std::string& s) : buffer(s) { }

  template <typename Msg> 
  MsgHolder operator()(const Msg&) const 
  { 
    Msg msg;
    deserialize(buffer, &msg);

    MsgHolder ret = msg;
    return ret;
  }

private:
  const std::string& buffer;
};

}  // detail
}  // windup
