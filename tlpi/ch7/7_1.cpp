#include <iostream> 
#include <cstdlib> 
#include <unistd.h> 
#include <algorithm> 


static const size_t kArraySize = 10000;
static const size_t kAllocSize = 10000;

int main(int argc, const char *argv[])
{
	size_t alloc_size = kAllocSize;
	if (argc > 1)
		alloc_size = std::min(std::atoi(argv[1]), static_cast<int>(kAllocSize));

	size_t alloc_num = kArraySize;
	if (argc > 2)
		alloc_num = std::min(std::atoi(argv[2]), static_cast<int>(kArraySize));

	std::cout << " sbrk:" << ::sbrk(0) << std::endl;
	for (size_t i = 0; i < alloc_num; i++) 
	{
		::malloc(alloc_size);
		std::cout << "Total " << (i+1) * alloc_size << " allocated..";
		std::cout << " sbrk:" << ::sbrk(0) << std::endl;
	}

	::sbrk(10240);
	std::cout << " sbrk:" << ::sbrk(0) << std::endl;

	return 0;
}
