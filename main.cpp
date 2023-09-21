#include <iostream>

int main() {

#ifdef NDEBUG
	#if __LANGUAGE==CXX
		std::cout << "Hello C++, this is Release!" << std::endl;
	#elif __LANGUAGE==C
		printf("Hello C, this is Release!\n");
	#else
		#error "which language is this?"
	#endif
#else
	#if __LANGUAGE==CXX
		std::cout << "Hello C++, this is Debug!" << std::endl;
	#elif __LANGUAGE==C
		printf("Hello C, this is Debug!\n");
	#else
		#error "which language is this?"
	#endif
#endif

	return 0;
}