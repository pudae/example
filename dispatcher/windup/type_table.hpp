#pragma once

#include <unordered_map>

#include "boost/noncopyable.hpp"
#include "boost/variant.hpp"
#include "boost/mpl/for_each.hpp"

#include "trait.hpp"

namespace windup 
{

template <typename MsgHolder>
class TypeTable : private boost::noncopyable
{
public:  
  /// @todo  "TypeId" 타입을 T타입 기반으로 알아내기
  static const MsgHolder& type(int id) { return instance()[id]; }

private:
  TypeTable() 
  { 
    boost::mpl::for_each<typename MsgHolder::types>(TableBuilder(*this));
  }

  struct TableBuilder
  {
    TableBuilder(TypeTable& obj_) : obj(obj_) { }

    template <typename T>
    void operator()(T t)
    {
      MsgHolder msg = t;
      auto v = std::make_pair(static_cast<int>(Trait<T>::id()), t);
      obj.type_table_.insert(v);
    }

    TypeTable& obj;
  };

  static TypeTable& instance() 
  { 
    static TypeTable s_instance;
    return s_instance; 
  }

  const MsgHolder& operator[](int id) const 
  { 
    auto p = type_table_.find(id);
    assert(type_table_.end() != p);
    return p->second;
  }

  std::unordered_map<int, MsgHolder> type_table_;
};
  
}  // windup 
