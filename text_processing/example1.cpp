#include <iostream> 
#include <fstream> 
#include <array> 
#include <algorithm> 
#include <iterator>
#include <vector>
#include <cassert>

#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp" 

typedef std::vector<std::string> Tokens;

const size_t kColumnNum = 3;

Tokens tokenize(std::string& s)
{
  Tokens ret;
  boost::tokenizer<boost::escaped_list_separator<char>> tokens(s);

  std::copy(begin(tokens), end(tokens), std::back_inserter(ret));
  std::for_each(begin(ret), end(ret), 
                [] (std::string& s) 
                { 
                  boost::algorithm::trim(s); 
                  boost::algorithm::to_lower(s);
                  boost::algorithm::ireplace_all(s, " ", "_");
                });

  return ret;
}

Tokens parse_header(std::ifstream& in)
{
  std::string s;
  if (!std::getline(in, s))
    throw std::string("invalid format");

  auto ret = tokenize(s);
  if (ret.size() != kColumnNum)
    throw std::string("invalid format");
  ret.push_back("age");

  return ret;
}

std::vector<Tokens> parse_records(std::ifstream& in)
{
  std::vector<Tokens> ret;
  for (std::string s; std::getline(in, s); )
  {
    auto record = tokenize(s);
    if (record.size() != kColumnNum)
      throw std::string("invalid format");
    record.push_back(std::to_string(2014 - boost::lexical_cast<int>(record[2])));
    ret.push_back(record);
  }
  return ret;
}

void write(std::ostream& out, const Tokens& header, const Tokens& record)
{
  assert(header.size() == record.size());

  out << "<record>" << std::endl;
  for (size_t i = 0; i < header.size(); ++i)
    out << boost::format("\t<%1%>%2%</%1%>") % header[i] % record[i] << std::endl;
  out << "</record>" << std::endl;
}

int main()
{
  std::ifstream in("example1.txt");
  
  try
  {
    auto header = parse_header(in);
    auto records = parse_records(in);
    for (const auto& record : records)
      write(std::cout, header, record);
  }
  catch (const std::string& s)
  {
    std::cerr << s << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
