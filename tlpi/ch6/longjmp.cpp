#include <iostream>
#include <csetjmp>
#include <boost/shared_ptr.hpp>

class TestClass 
{
 public:
  TestClass (const char* id)
      : id_(id)
  {
    std::cout << "Construct id : " << id_ << std::endl;
  }
  virtual ~TestClass ()
  {
    std::cout << "Destruct id : " << id_ << std::endl;
  }

  void func(void)
  {
    std::cout << " do nothing " << std::endl;
  }

 private:
  std::string id_;
};

static jmp_buf g_env;

static void f3(void)
{
  longjmp(g_env, 3);
}

static void f2(int argc)
{
  if (argc == 2)
    longjmp(g_env, 2);

  TestClass tc("local");
  TestClass* p_tc = new TestClass("normal pointer");
  boost::shared_ptr<TestClass> sp_tc(new TestClass("shared pointer"));

  tc.func();
  p_tc->func();
  sp_tc->func();

  f3();
}

static void f1(int argc)
{
  if (argc == 1)
    longjmp(g_env, 1);
  f2(argc);
}

int main(int argc, const char *argv[])
{
  switch (setjmp(g_env))
  {
    case 0:
      std::cout << "calling f1() after initial setjmp() " << std::endl;
      f1(argc);
      break;
    case 1:
      std::cout << "we jumped back from f1() " << std::endl;
      break;
    case 2:
      std::cout << "we jumped back from f2() " << std::endl;
      break;
    case 3:
      std::cout << "we jumped back from f3() " << std::endl;
      break;
  }
  return 0;
}
