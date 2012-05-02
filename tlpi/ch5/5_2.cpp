#include <iostream> 
#include <cstring> 
#include <errno.h> 
#include <fcntl.h> 

int main(int argc, const char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage : 5_2 filename append_string" << std::endl;
    return -1;
  }

  int fd = ::open(argv[1], O_RDWR | O_APPEND);
  if (-1 == fd)
  {
    std::cerr << "open fail : " << errno << std::endl;
    return -1;
  }

  if (-1 == ::lseek(fd, 0, SEEK_SET))
  {
    std::cerr << "lseek fail : " << errno << std::endl;
    return -1;
  }
  std::cout << "seek to the beginning of the file" << std::endl;

  if (-1 == ::write(fd, reinterpret_cast<const void*>(argv[2]), ::strlen(argv[2])))
  {
    std::cerr << "write fail : " << errno << std::endl;
    return -1;
  }
  std::cout << "write " << argv[2] << std::endl;

  std::cout << "= file_content ========== " << std::endl;
  if (-1 == ::lseek(fd, 0, SEEK_SET))
  {
    std::cerr << "lseek fail : " << errno << std::endl;
    return -1;
  }

  char buf[1024];
  int ret = 0;
  do 
  {
    ret = ::read(fd, buf, 1023);
    if (0 > ret)
    {
      std::cerr << "read fail : " << errno << std::endl;
      return -1;
    }
    buf[ret] = '\0';
    std::cout << buf;
  } while (ret > 0);
  std::cout << std::endl;

  ::close(fd);

  return 0;
}


