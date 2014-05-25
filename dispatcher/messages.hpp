#pragma once

#include "boost/variant.hpp"

#include "windup/trait.hpp"

struct MsgA { static const char* name() { return "MsgA"; } };
struct MsgB { static const char* name() { return "MsgB"; } };
struct MsgC { static const char* name() { return "MsgC"; } };

typedef boost::variant<MsgA, MsgB, MsgC> MsgHolder;

enum class TypeId
{
  kMsgA = 0,
  kMsgB = 1,
  kMsgC = 2
};

namespace windup
{

template <>
struct Trait<MsgA> { static TypeId id() { return TypeId::kMsgA; } };

template <>
struct Trait<MsgB> { static TypeId id() { return TypeId::kMsgB; } };

template <>
struct Trait<MsgC> { static TypeId id() { return TypeId::kMsgC; } };
  
} /* windup */
