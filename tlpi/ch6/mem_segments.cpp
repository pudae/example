#include <iostream>
#include <cstdlib>

char g_buf[65536];                // uninitialized data segment
int g_primes[] = { 2, 3, 5, 6 };  // initialized data segment

static int square(int x)
{
  int result;

  result = x * x;
  return result;
}

static void do_calc(int val)
{
  std::cout << "The square of " << val << " is " << square(val) << std::endl;

  if (val < 1000) 
  {
    int t;
    t = val * val * val;
    std::cout << "The cube of " << val << " is " << t << std::endl;
  }
}

int main(int argc, const char *argv[])
{
  static int s_key = 9973;
  static char s_buf[10240000];
  char* p;
  int key;

  p = new char[1024];

  s_buf[0] = '1';       // for remove warning
  p[0] = '1';         // for remove warning

  do_calc(s_key);

  std::cout << "char g_buf[65536]:\t\t" << std::hex << reinterpret_cast<void*>(g_buf) << std::endl;
  std::cout << "int g_primes[] = { .. }:\t" << std::hex << reinterpret_cast<void*>(g_primes) << std::endl;
  std::cout << "static int s_key:\t\t" << std::hex << reinterpret_cast<void*>(&s_key) << std::endl;
  std::cout << "static char s_buf[100240000]:\t" << std::hex << reinterpret_cast<void*>(s_buf) << std::endl;
  std::cout << "&p:\t\t\t\t" << std::hex << reinterpret_cast<void*>(&p) << std::endl;
  std::cout << "p:\t\t\t\t" << std::hex << reinterpret_cast<void*>(p) << std::endl;
  std::cout << "key:\t\t\t\t" << std::hex << reinterpret_cast<void*>(&key) << std::endl;

  return 0;
}





