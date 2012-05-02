#include <iostream> 
#include <limits.h> 
#include <time.h> 

int main(int argc, const char *argv[])
{
  std::cout << "It takes " << UINT32_MAX / 100 << " seconds " << std::endl;

  std::cout << "sizeof(clock_t) : " << sizeof(clock_t) << std::endl;
  std::cout << "INT32_MAX / CLOCKS_PER_SEC = " << INT32_MAX / CLOCKS_PER_SEC << std::endl;
  std::cout << "INT64_MAX / CLOCKS_PER_SEC = " << INT64_MAX / CLOCKS_PER_SEC << std::endl;
  return 0;
}
