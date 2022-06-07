#ifndef SERVER_H_
#define SERVER_H_

inline void Red(void)
{
	std::cout << "\033[31m";
}

inline void Green(void)
{
	std::cout << "\033[32m";
}

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

char* Check_autorization(int32_t&, char*);

#endif