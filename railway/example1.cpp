#include <iostream>
#include <cctype>
#include <algorithm>

#include "expected.hpp"
#include "rail.hpp"

struct database_error : std::exception 
{
  const char* what() const noexcept { return "database error"; }
};

railway::Expected<std::string> validate_email(std::string s)
{
  if (s.find("e-mail") == std::string::npos)
    return railway::Unexpected("no e-mail field");
  return s;
}

railway::Expected<std::string> validate_name(std::string s)
{
  if (s.find("name") == std::string::npos)
    return railway::Unexpected("no name field");
  return s;
}

std::string to_lower(std::string s)
{
  std::transform(begin(s), end(s), begin(s), ::tolower);
  return s;
}

void update_database(std::string s)
{
  if (s.find("database") == std::string::npos)
    throw database_error();
}

railway::Expected<std::string> send_response(std::string s)
{
  // do something
  return s;
}

////////////////////////////////////////////////////////////////////////////////
/// imperative
////////////////////////////////////////////////////////////////////////////////
void imperative_approach(std::string s)
{
  auto result_a = validate_email(s);
  if (!result_a)
  {
    std::cerr << "invalid email" << std::endl;
    return;
  }

  auto result_b = validate_name(s);
  if (!result_b)
  {
    std::cerr << "invalid name" << std::endl;
    return;
  }

  s = to_lower(s);

  update_database(s);

  auto result_c = send_response(s);
  if (!result_c)
  {
    std::cerr << "fail to send response" << std::endl;
    return;
  }

  std::cout << "success:" << s << std::endl;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// railway
////////////////////////////////////////////////////////////////////////////////
void railway_approach_test(std::string s)
{
  auto result = 
      railway::bind(send_response)(
          railway::bind(railway::try_catch(railway::tee(update_database)))(
              railway::bind(railway::map(to_lower))(
                  railway::bind(validate_name)(
                      validate_email(s)))));

  if (!result)
    std::cerr << "fail:" << result.error() << std::endl;
  else
    std::cout << "success:" << result.value() << std::endl; 
}

void railway_approach(std::string s)
{
  auto f = 
      railway::Rail(validate_email) >>
      railway::bind(validate_name) >>
      railway::map(to_lower) >>
      railway::try_catch(railway::tee(update_database)) >>
      railway::bind(send_response);

  auto result = f(s);

  if (!result)
    std::cerr << "fail:" << result.error() << std::endl;
  else
    std::cout << "success:" << result.value() << std::endl; 
}

int main()
{
  std::string valid("{ e-mail: \"pudae@neowiz.com\", name: \"Pudae\", database: true }");  
  std::string noemail("{ email: \"pudae@neowiz.com\", name: \"Pudae\", database: true }");  
  std::string noname("{ e-mail: \"pudae@neowiz.com\", database: true }");  
  std::string nodb("{ e-mail: \"pudae@neowiz.com\", name: \"Pudae\"}");

  std::cout << "= railway =====================" << std::endl;
  railway_approach(valid);
  railway_approach(noemail);
  railway_approach(noname);
  railway_approach(nodb);

  return 0;
}
