
///
/// Copyright 2014 Neowiz Games, All rights reserved
///
/// author pudae(pudae@neowiz.com)
///

#pragma once

#include <string>

#include "boost/optional.hpp"

namespace railway {

class Unexpected
{
public:
  explicit Unexpected(const std::string& e) : e_(e) 
  { }

  std::string error() const { return e_; }

private:
  std::string e_;
};

template <typename T>
class Expected
{
public:
  Expected() { }

  Expected(const T& v) : v_(v) 
  { }

  Expected(const Unexpected& u)
      : e_(u.error())
  { }

  operator bool() { return v_; }

  T value() const { return *v_; }

  std::string error() const { return e_; }

private:
  boost::optional<T>  v_; 
  std::string         e_;
};

// An adapter that takes a switch function and creates a new function that
// accepts two-track values as input.
std::function<Expected<std::string>(Expected<std::string>)> 
bind(std::function<Expected<std::string>(std::string)> f)
{
  return [f] (Expected<std::string> v)
  {
    if (v)
      return f(v.value());
    else
      return v;
  };
}

// An adapter that takes a normal one-track function and turns it into a
// two-track function. (Also known as a "lift" in some contexts.) 
std::function<Expected<std::string>(Expected<std::string>)> 
map(std::function<std::string(std::string)> f) 
{
  return [f] (Expected<std::string> v)
  {
    if (v)
      return Expected<std::string>(f(v.value()));
    else
      return v;
  };
}

// An adapter that takes a dead-end function and turns it into a one-track
// function that can be used in a data flow. (Also known as tap.) 
std::function<std::string(std::string)> 
tee(std::function<void(std::string)> f)
{
  return [f] (std::string s)
  {
    f(s);
    return s;
  };
}

// An adapter that takes a normal one-track function and turns it into a switch function, 
// but also catches exceptions.
std::function<Expected<std::string>(Expected<std::string>)> 
try_catch(std::function<std::string(std::string)> f)
{
  return [f] (Expected<std::string> v) -> Expected<std::string>
  {
    if (!v)
      return v;

    try
    {
      f(v.value());
      return v.value();
    }
    catch (const std::exception& e)
    {
      return Unexpected(e.what());
    }
  };
}

}  // end namespace railway
