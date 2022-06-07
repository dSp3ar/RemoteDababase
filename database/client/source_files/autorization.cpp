#include <iostream>
#include <string>
#include "../header_files/autorization.h"

void Registration(char* message, char* login_c)
{
	char choice;
	std::string login, password, json;

	Green();
	std::cout << "You are in the autorization menu.\n";
	std::cout << "Please, select what you want to do: \n";
	std::cout << "1. Registration.\n";
	std::cout << "2. Sing in.\n";
	Checking_char(choice);
	if (choice == '1')
		std::cout << "Now, you will be registrated.\n";
	else
		std::cout << "Now, you will sing in.\n";

	do
	{
		std::cout << "Please, enter your login (do not use whitespaceses): ";
		std::getline(std::cin, login);
		std::cout << "Please, enter your password(do not use whitespaceses): ";
		std::getline(std::cin, password);
		if (Checkind_spaces(login) || Checkind_spaces(password))
		{
			Red();
			std::cout << "You should not use whitespaceses!\n";
			Green();
		}
		else
			break;
	} while(true);

	if (choice == '1')
		Making_json(json, login, password, "registration");
	else
		Making_json(json, login, password, "sing in");
	Str_to_char(json, message);
	Str_to_char(login, login_c);
}

bool Checkind_spaces(std::string& str)
{
	bool status = false;
	size_t i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			status = true;
			break;
		}
		i++;
	}
	return status;
}

void Making_json(std::string& json, std::string& login, std::string& password, const char* mode)
{
	json = "{ \"mode\":\"";
	json += mode;
	json += "\", \"login\":\"" + login + "\", \"password\":\"" + password + "\" }";
}

void Checking_char(char& symbol)
{
	std::cout << "Your choice is: ";
	while((symbol = std::cin.get()) != '1' && symbol != '2' || std::cin.get() != '\n')
	{
		while (std::cin.get() != '\n')
			continue;
		Red();
		std::cout << "Sorry, but you have entered incorrect data!\n";
		Green();
		std::cout << "Your choice is: ";
	}
}

void Str_to_char(std::string& obj_string, char* c_string)
{
	size_t i = 0;

	if (obj_string.size() > BUFFER_SIZE)
	{
		Red();
		std::cout << "Buffer overflow!\n";
		exit(EXIT_FAILURE);
	}

	while (obj_string[i] != '\0')
	{
		c_string[i] = obj_string[i];
		++i;
	}	
	c_string[i] = '\0';
}