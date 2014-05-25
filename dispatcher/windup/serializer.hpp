#pragma once

#include "boost/variant.hpp"

#include "windup/type_table.hpp"
#include "windup/detail/serializer.hpp"

namespace windup 
{

template <typename MsgHolder>
class Serializer
{
public:
  typedef typename MsgHolder::types Types;
  typedef decltype(Trait<typename boost::mpl::front<Types>::type>::id()) TypeId;

  static MsgHolder deserialize(TypeId id, const std::string& buffer)
  {
    auto msg_holder = TypeTable<MsgHolder>::type(id);
    return boost::apply_visitor(
        detail::Deserializer<MsgHolder>(buffer), msg_holder);
  } 

  /// TODO serializer interface? 필요없나?
};

}  // windup 
