#include <iostream> 
#include <fstream> 
#include <map> 
#include <regex> 
#include <functional>

#include "boost/filesystem.hpp"
#include "boost/optional.hpp"
#include "boost/format.hpp"

typedef std::map<std::string, std::vector<std::string>> Parsed;

boost::optional<std::string> match_ns(const std::string& s)
{
  std::regex r_ns("\\s*namespace\\s+.+\\s+([_[:alpha:]][\\._[:alnum:]]*).*");
  std::smatch m;
  if (true == std::regex_match(s, m, r_ns))
    return std::string(m[1]);
  return boost::optional<std::string>();
}

boost::optional<std::string> match_type(const std::string& s)
{
  std::regex r_ns("\\s*struct\\s+([_[:alpha:]][\\._[:alnum:]]*).*");
  std::smatch m;
  if (true == std::regex_match(s, m, r_ns))
    return std::string(m[1]);
  return boost::optional<std::string>();
}

Parsed parse_thrift(const std::string& filename, Parsed& parsed)
{
  std::ifstream in(filename);

  std::string ns;
  std::vector<std::string> types;
  for (std::string s; std::getline(in, s); )
  {
    auto ret_ns = match_ns(s);
    if (ret_ns)
    {
      assert(ns.empty());
      assert(types.empty());
      ns = *ret_ns;
      continue;
    }

    auto ret_type = match_type(s);
    if (ret_type)
    {
      types.push_back(*ret_type);
      continue;
    }
  }

  std::copy(begin(types), end(types), std::back_inserter(parsed[ns]));

  return parsed;
}

Parsed parse_directory(const std::string& directory)
{
  Parsed ret;
  boost::filesystem::path dir(directory);
  for (auto p = boost::filesystem::directory_iterator(dir);
       p != boost::filesystem::directory_iterator(); ++p)
  {
    if (p->path().extension() == ".thrift")
      ret = parse_thrift(p->path().native(), ret);
  }

  return ret;
}

void write_type_id(const Parsed& parsed)
{
  for (auto kv : parsed)
  {
    std::cout << "filename:" << kv.first << std::endl;
    // 그냥 namespace * 네임스페이스이름 형식으로 써버림
    std::cout << "namespace * " << kv.first << std::endl;
    std::cout << "enum MessageType\n{\n";
    for (auto v : kv.second)
      std::cout << boost::format("\tk%1%\t= %2%\n") % v % std::hash<decltype(v)>()(v);
    std::cout << "}\n";
  }
}

int main()
{
  auto parsed = parse_directory(".");
  write_type_id(parsed);

  return 0;
}
