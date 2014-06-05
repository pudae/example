#include <iostream>

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

using namespace boost::xpressive;

namespace  {

const std::string kStr("123 ::a()bd 1a2[d34b a]bcd");

} /*  */

void replace(std::string str, std::string fmt, sregex re)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;

  std::cout << "result: " << regex_replace(str, re, fmt) << std::endl;

  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void sum_integer_d()
{
}

void sum_integer_s()
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;
  int sum = 0;

  sregex integer = (s1 = +_d) [ref(sum) += as<int>(s1)];
  sregex reg = (*~_d) >> *(integer | (*~_d));
  smatch m;
  regex_match(kStr, m, reg);

  std::cout << "sum:" << sum << std::endl;
  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void tokenize(std::string s, sregex re)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;
  sregex_token_iterator begin(s.begin(), s.end(), re);
  sregex_token_iterator end;

  std::ostream_iterator<std::string> os_itr(std::cout, "\n"); 
  std::copy(begin, end, os_itr);

  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void print_date_s(std::string s)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;

  sregex re = repeat<1,2>(_d) >> '/' >> 
              repeat<1,2>(_d) >> '/' >> 
              repeat<1,2>(repeat<2,2>(_d));
  
  sregex_iterator cur(s.begin(), s.end(), re);
  sregex_iterator end;

  sregex re2 = as_xpr('/');
  auto fmt = std::string(". ");
  for (; cur != end; ++cur)
  {
    const smatch& m = *cur;
    std::cout << regex_replace(m[0].str(), re2, fmt) << std::endl;
  }

  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void print_date_d(std::string s)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;

  auto re = sregex::compile("(\\d{1,2})/(\\d{1,2})/((?:\\d{2}){1,2})");
  
  sregex_iterator cur(s.begin(), s.end(), re);
  sregex_iterator end;

  sregex re2 = sregex::compile("/");
  auto fmt = std::string(". ");
  for (; cur != end; ++cur)
  {
    const smatch& m = *cur;
    std::cout << regex_replace(m[0].str(), re2, fmt) << std::endl;
  }

  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void replace_date_s(std::string s)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;

  sregex re = (s1 = repeat<1,2>(_d)) >> '/' >> 
              (s2 = repeat<1,2>(_d)) >> '/' >> 
              (s3 = repeat<1,2>(repeat<2,2>(_d)));
  auto fmt = std::string("$1. $2. $3");
  std::cout << regex_replace(s, re, fmt) << std::endl;
  
  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

void replace_date_d(std::string s)
{
  std::cout << "= " << __FUNCTION__ << " =============" << std::endl;

  auto re = sregex::compile("(\\d{1,2})/(\\d{1,2})/((?:\\d{2}){1,2})");
  auto fmt = std::string("$1. $2. $3");
  std::cout << regex_replace(s, re, fmt) << std::endl;

  std::cout << "- end " << __FUNCTION__ << " ---------" << std::endl;
  std::cout << std::endl;
}

int main()
{
  replace(kStr, "#", sregex::compile("\\d"));
  replace(kStr, "#", _d);

  sum_integer_d(); 
  sum_integer_s(); 

  tokenize(kStr, +(~punct));
  tokenize(kStr, sregex::compile("[^[:punct:]]+"));

  print_date_s("2/21/2008 | 03/21/2014");
  print_date_d("2/21/2008 | 03/21/2014");

  replace_date_s("2/21/2008 | 03/21/2014");
  replace_date_d("2/21/2008 | 03/21/2014");

  return 0;
}

