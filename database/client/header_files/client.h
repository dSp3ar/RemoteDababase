#ifndef CLIENT_H_
#define CLIENT_H_

void Registration(char*, char*);

inline void Red(void)
{
	std::cout << "\033[31m";
}

inline void Green(void)
{
	std::cout << "\033[32m";
}

void Parsing_feedback(char*);
void Checking_int16(int16_t&, char*);

#define BUFFER_SIZE 1024

#endif