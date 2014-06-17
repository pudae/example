
///
/// Copyright 2014 Neowiz Games, All rights reserved
///
/// author pudae(pudae@neowiz.com)
///

#pragma once

#include <functional>

#include "expected.hpp"

namespace railway {

/// 우선 Expected<std::string> (Expected<std::string>) 형태에 한해서만 만들어 봄 
class Rail
{
public:
  explicit Rail(std::function<Expected<std::string>(std::string)> f)
      : f_(f)
  { }

  Rail& operator>>(std::function<Expected<std::string>(Expected<std::string>)> f)
  {
    f_ = std::bind(f, std::bind(f_, std::placeholders::_1));
    return *this;
  } 

  Expected<std::string> operator()(std::string s) { return f_(s); }

private:
  std::function<Expected<std::string>(std::string)> f_;
};

}  // end namespace railway
