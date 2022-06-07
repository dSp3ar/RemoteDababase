#ifndef AUTORIZATION_H_
#define AUTORIZATION_H_

#define BUFFER_SIZE 1024
void Making_json(std::string&, std::string&, std::string&, const char*);
void Registration(char*, char*);
void Checking_char(char&);
void Str_to_char(std::string&, char*);
bool Checkind_spaces(std::string&);

inline void Green(void)
{
	std::cout << "\033[32m";
}

inline void Red(void)
{
	std::cout << "\033[31m";
}

#endif
