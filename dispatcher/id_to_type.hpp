#pragma once

#include <unordered_map>
#include <mutex>

#include "boost/noncopyable.hpp"
#include "boost/variant.hpp"
#include "boost/mpl/for_each.hpp"

#include "trait.hpp"

/// 
///
template <typename TT>
class IdToType : private boost::noncopyable
{
public:  
  /// @todo  "TypeId" 타입을 T타입 기반으로 알아내기
  static const TT& type(int id) { return instance()[id]; }

private:
  IdToType() 
  { 
    boost::mpl::for_each<typename TT::types>(TableBuilder(*this));
  }

  struct TableBuilder
  {
    TableBuilder(IdToType& obj_) : obj(obj_) { }

    template <typename T>
    void operator()(T t)
    {
      TT msg = t;
      auto v = std::make_pair(static_cast<int>(Trait<T>::id()), t);
      obj.type_table_.insert(v);
    }

    IdToType& obj;
  };

  static IdToType& instance() 
  { 
    static IdToType s_instance;
    return s_instance; 
  }

  const TT& operator[](int id) const 
  { 
    auto p = type_table_.find(id);
    assert(type_table_.end() != p);
    return p->second;
  }

  std::unordered_map<int, TT> type_table_;
};

template <typename TT>
class Serializer
{
public:
  /// @todo  "TypeId" 타입을 T타입 기반으로 알아내기
  static TT deserialize_tmp(int id, const std::string& buffer)
  {
    auto msg_holder = IdToType<TT>::type(id);
    return boost::apply_visitor(Impl(buffer), msg_holder);
  } 

private:
  struct Impl : public boost::static_visitor<TT>
  {
    Impl(const std::string& s) : buffer(s) { }

    template <typename Msg> 
    TT operator()(const Msg&) const 
    { 
      Msg msg;
      deserialize(buffer, &msg);

      TT ret = msg;
      return ret;
    }
  
    const std::string& buffer;
  };
};

