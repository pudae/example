#pragma once

#include <map>

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
  typedef typename MsgHolder::types Types;
  typedef decltype(Trait<typename boost::mpl::front<Types>::type>::id()) TypeId;

  static const MsgHolder& type(TypeId id) { return instance()[id]; }

private:
  TypeTable() 
  { 
    boost::mpl::for_each<Types>(TableBuilder(*this));
  }

  struct TableBuilder
  {
    TableBuilder(TypeTable& obj_) : obj(obj_) { }

    template <typename T>
    void operator()(T t)
    {
      MsgHolder msg = t;
      auto v = std::make_pair(Trait<T>::id(), t);
      obj.type_table_.insert(v);
    }

    TypeTable& obj;
  };

  // TODO (pudae@neowiz.com) use nifty counter
  // http://en.wikibooks.org/wiki/More_C++_Idioms/Nifty_Counter
  static TypeTable& instance() 
  { 
    static TypeTable s_instance;
    return s_instance; 
  }

  const MsgHolder& operator[](TypeId id) const 
  { 
    auto p = type_table_.find(id);
    assert(type_table_.end() != p);
    return p->second;
  }

  std::map<TypeId, MsgHolder> type_table_;
};
  
}  // windup 
