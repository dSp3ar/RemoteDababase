#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <json-c/json.h>
#include "../header_files/client.h"


int main(int argc, const char* argv[])
{
	int32_t client_deskriptor;
	struct sockaddr_in client_address;
	const int16_t PORT = 1615; // You can change it
	const int16_t MAX_CONNECTION = 1; // You can chage it
	const char* server_ip = "127.0.0.1"; // You can change it, but for testing this value is ok

	Green();

	if ((client_deskriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Red();
		std::cout << "Sorry, but socket was not created for client!\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Nice! Socket was created for client!\n";

	client_address.sin_port = htons(PORT); // Network byte order
	client_address.sin_family = AF_INET; // For network protocol IPv4
	client_address.sin_addr.s_addr = htons(INADDR_ANY); // I don't need to bind a socket to a specific IP
	inet_pton(AF_INET, server_ip, &client_address.sin_addr);

	if ((connect(client_deskriptor, reinterpret_cast <const struct sockaddr*>(&client_address), sizeof(client_address))) < 0)
	{
		Red();
		std::cout << "Sorry, failed attemp to connect to server!\n";
		close(client_deskriptor);
		exit(EXIT_FAILURE);
	}
	
	char buffer[BUFFER_SIZE];
	if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
	{
		Red();
		std::cout << "Unfortunately client did not get message from server!\n";
		close(client_deskriptor);
		exit(EXIT_FAILURE);
	}
	std::cout << buffer << std::endl;

	std::cout << "Client was connected to server successfully!\n";
	std::cout << "Please, press any key to clear screen and open menu...";
	while (std::cin.get() != '\n')
		continue;
	std::cout << "\033[H\033[2J";
	char login[100];

	do
	{
		Registration(buffer, login);
		if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
		{
			Red();
			std::cout << "Unfortunately client could not send message.\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}
		
		if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
		{
			Red();
			std::cout << "Unfortunately client did not get message from server!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		Parsing_feedback(buffer);
		if (!strcmp(buffer, "ok"))
		{
			std::cout << "Weclome!\n";
			break;
		}
		else
		{
			Red();
			if (!strcmp(buffer, "login colision"))
				std::cout << "Your entered login is exist already!\n";
			else if (!strcmp(buffer, "no file"))
				std::cout << "You cannot enter, because there are no registrated users!\n";
			else if (!strcmp(buffer, "no login"))
				std::cout << "There are no such user!\n";
			Green();
		}
	} while (true);

	while (true)
	{
		char mode;

		std::cout << "Please, press any key to clear screen and open menu...";
		while (std::cin.get() != '\n')
			continue;
		std::cout << "\033[H\033[2J";

		std::cout << "Hello, ";
		Red();
		std::cout << login;
		Green();
		std::cout << "!\n";
		std::cout << "You are in the main menu. Select what you want to do:\n";
		std::cout << "1. Create new database.\n";
		std::cout << "2. Work with exist database.\n";
		std::cout << "3. Exit.\n";
		Red();
		std::cout << login;
		Green();
		std::cout << "$ Your choice is: ";
		while ((mode = std::cin.get()) != '1' && mode != '2' && mode != '3' || std::cin.get() != '\n')
		{
			while (std::cin.get() != '\n')
				continue;
			Red();
			std::cout << "Sorry, but you have entered incorrect data!\n";
			std::cout << login;
			Green();
			std::cout << "$ Your choice is: ";
		}

		if (mode == '3')
		{
			strcpy(buffer, "{ \"mode\":\"exit\" }");
			if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
			{
				Red();
				std::cout << "Unfortunately client could not send message.\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			close(client_deskriptor);
			return 0;
		}

		if (mode == '1')
		{
			int16_t int_number, double_number, string_number;
			char **int_titles = NULL, **double_titles = NULL, **string_titles = NULL, database_name[100];

			while (true)
			{
				Red();
				std::cout << login;
				Green();
				std::cout << "$ Please, enter the database name (do not use whitespaces): ";
				std::cin.getline(database_name, 100);
				size_t i = 0;
				bool status = false;
				while (database_name[i] != '\0')
				{
					if (database_name[i] == ' ')
					{
						Red();
						std::cout << "You have not use whitespaces!\n";
						Green();
						status = true;
						break;
					}
					i++;
				}

				if (!status)
					break;
			}

			Red();
			std::cout << login;
			Green();
			std::cout << "$ Please, enter the number of <int> filds in your element: ";
			Checking_int16(int_number, login);
			Red();
			std::cout << login;
			Green();
			std::cout << "$ Please, enter the number of <double> filds in your element: ";
			Checking_int16(double_number, login);
			Red();
			std::cout << login;
			Green();
			std::cout << "$ Please, enter the number of <string> filds in your element: ";
			Checking_int16(string_number, login);

			if ((int_titles = new char*[int_number]) == NULL)
			{
				Red();
				std::cout << "Sorry, but memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < int_number; ++i)
				if ((int_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			if ((double_titles = new char*[double_number]) == NULL)
			{
				Red();
				std::cout << "Sorry, but memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < double_number; ++i)
				if ((double_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			if ((string_titles = new char*[string_number]) == NULL)
			{
				Red();
				std::cout << "Sorry, but memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < string_number; ++i)
				if ((string_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			Red();
			std::cout << login;
			Green();
			std::cout << "$ Now, you have to enter the title of these filds (do not use whitespaces!):\n";

			std::cout << "For int:\n";
			for (size_t i = 0; i < int_number; ++i)
			{
				std::cout << "int_fild #" << i + 1 << " is: ";
				std::cin.getline(int_titles[i], 100);
				size_t j = 0;
				while (int_titles[i][j] != '\0')
				{
					if (int_titles[i][j] == ' ')
					{
						Red();
						std::cout << "You have not use whitespaces!\n";
						i--;
						Green();
						break;
					}
					j++;
				}
			}

			std::cout << "\nFor double:\n";
			for (size_t i = 0; i < double_number; ++i)
			{
				std::cout << "double_fild #" << i + 1 << " is: ";
				std::cin.getline(double_titles[i], 100);
				size_t j = 0;
				while (double_titles[i][j] != '\0')
				{
					if (double_titles[i][j] == ' ')
					{
						Red();
						std::cout << "You have not use whitespaces!\n";
						i--;
						Green();
						break;
					}
					j++;
				}
			}

			std::cout << "\nFor string:\n";
			for (size_t i = 0; i < string_number; ++i)
			{
				std::cout << "string_fild #" << i + 1 << " is: ";
				std::cin.getline(string_titles[i], 100);
				size_t j = 0;
				while (string_titles[i][j] != '\0')
				{
					if (string_titles[i][j] == ' ')
					{
						Red();
						std::cout << "You have not use whitespaces!\n";
						i--;
						Green();
						break;
					}
					j++;
				}
			}
			char temp[10];

			strcpy(buffer, "{ \"mode\":\"");
			strcat(buffer, "creating\", \"name\":\"");
			strcat(buffer, database_name);
			strcat(buffer, "\", \"int\":");
			sprintf(temp, "%d", int_number);
			strcat(buffer, temp);
			strcat(buffer, ", \"double\":");
			sprintf(temp, "%d", double_number);
			strcat(buffer, temp);
			strcat(buffer, ", \"string\":");
			sprintf(temp, "%d", string_number);
			strcat(buffer, temp);
			strcat(buffer, ", \"ints\":[");
			for (size_t i = 0; i < int_number; ++i)
			{
				strcat(buffer, "\"");
				strcat(buffer, int_titles[i]);
				if (i == int_number - 1)
					strcat(buffer, "\"], ");
				else
					strcat(buffer, "\", ");
			}
			strcat(buffer, "\"doubles\":[");
			for (size_t i = 0; i < double_number; ++i)
			{
				strcat(buffer, "\"");
				strcat(buffer, double_titles[i]);
				if (i == double_number - 1)
					strcat(buffer, "\"], ");
				else
					strcat(buffer, "\", ");
			}
			strcat(buffer, "\"strings\":[");
			for (size_t i = 0; i < string_number; ++i)
			{
				strcat(buffer, "\"");
				strcat(buffer, string_titles[i]);
				if (i == string_number - 1)
					strcat(buffer, "\"] }");
				else
					strcat(buffer, "\", ");
			}

			if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
			{
				Red();
				std::cout << "Unfortunately client could not send message.\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < int_number; ++i)
				delete int_titles[i];
			delete[] int_titles;

			for (size_t i = 0; i < double_number; ++i)
				delete double_titles[i];
			delete[] double_titles;

			for (size_t i = 0; i < string_number; ++i)
				delete string_titles[i];
			delete[] string_titles;

			if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
			{
				Red();
				std::cout << "Unfortunately client did not get message from server!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			Parsing_feedback(buffer);
			if (!strcmp(buffer, "ok"))
				std::cout << "Good! Database template was created!\n";
			else
			{
				Red();
				std::cout << "This database name is exist already!\n";
				Green();
			}
		}

		if (mode == '2')
		{
			char database_name[100];

			while (true)
			{
				Red();
				std::cout << login;
				Green();
				std::cout << "$ Please, enter the database name that you want to work with (do not use whitespaces): ";				
				std::cin.getline(database_name, 100);
				size_t i = 0;
				bool status = false;
				while (database_name[i] != '\0')
				{
					if (database_name[i] == ' ')
					{
						Red();
						std::cout << "You have not use whitespaces!\n";
						Green();
						status = true;
						break;
					}
					i++;
				}

				if (!status)
					break;
			}

			strcpy(buffer, "{ \"mode\":\"working\", \"name\":\"");
			strcat(buffer, database_name);
			strcat(buffer, "\" }");

			if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
			{
				Red();
				std::cout << "Unfortunately client could not send message.\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
			{
				Red();
				std::cout << "Unfortunately client did not get message from server!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			struct json_object *parsed_json, *mode, *int_number, *double_number, *string_number, *array, *element;

			parsed_json = json_tokener_parse(buffer);
			if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				exit(EXIT_FAILURE);
			}

			if (!strcmp(json_object_get_string(mode), "no"))
			{
				Red();
				std::cout << "There are no such database name at the server!\n";
				Green();
				continue;
			}

			int32_t int_length, double_length, string_length;
			char **int_titles = NULL, **double_titles = NULL, **string_titles = NULL;

			if (!(json_object_object_get_ex(parsed_json, "int", &int_number)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			int_length = json_object_get_int(int_number);

			if ((int_titles = new char*[int_length]) == NULL)
			{
				Red();
				std::cout << "Memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < int_length; ++i)
				if ((int_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			if (!(json_object_object_get_ex(parsed_json, "double", &double_number)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			double_length = json_object_get_int(double_number);

			if ((double_titles = new char*[double_length]) == NULL)
			{
				Red();
				std::cout << "Memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < double_length; ++i)
				if ((double_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			if (!(json_object_object_get_ex(parsed_json, "string", &string_number)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			string_length = json_object_get_int(string_number);

			if ((string_titles = new char*[string_length]) == NULL)
			{
				Red();
				std::cout << "Memory was not alloced!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < string_length; ++i)
				if ((string_titles[i] = new char[100]) == NULL)
				{
					Red();
					std::cout << "Sorry, but memory was not alloced!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

			if (!(json_object_object_get_ex(parsed_json, "ints", &array)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < int_length; ++i)
			{
				element = json_object_array_get_idx(array, i);
				strcpy(int_titles[i], json_object_get_string(element));
			}

			if (!(json_object_object_get_ex(parsed_json, "doubles", &array)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < double_length; ++i)
			{
				element = json_object_array_get_idx(array, i);
				strcpy(double_titles[i], json_object_get_string(element));
			}

			if (!(json_object_object_get_ex(parsed_json, "strings", &array)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < string_length; ++i)
			{
				element = json_object_array_get_idx(array, i);
				strcpy(string_titles[i], json_object_get_string(element));
			}

			while (true)
			{
				std::cout << "Now, you can work with your database!\n";
				Red();
				std::cout << login;
				Green();
				std::cout << "$ Make a choice what you want to do:\n";
				std::cout << "1. Show database.\n";
				std::cout << "2. Add new node to this database.\n";
				std::cout << "3. Delete specific node.\n";
				std::cout << "4. Go back.\n";
				char choice;
				Red();
				std::cout << login;
				Green();
				std::cout << "$ Your choice is: ";
				while ((choice = std::cin.get()) != '1' && choice != '2' && choice != '3' && choice != '4' || std::cin.get() != '\n')
				{
					while (std::cin.get() != '\n')
						continue;
					Red();
					std::cout << "Sorry, but you have entered incorrect data!\n";
					std::cout << login;
					Green();
					std::cout << "$ Your choice is: ";
				}

				if (choice == '4')
				{
					strcpy(buffer, "{ \"mode\":\"back\" }");
					if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
					{
						Red();
						std::cout << "Unfortunately client could not send message.\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					break;
				}
				if (strcmp(login, "admin") && (choice == '2' || choice == '3'))
				{
					Red();
					std::cout << "Permission denied! Only admin can use this case. If there are no admin just create him.\n";
					Green();
					continue;
				}

				if (choice == '1')
				{
					strcpy(buffer, "{ \"mode\":\"show\" }");

					if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
					{
						Red();
						std::cout << "Unfortunately client could not send message.\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					std::cout << "\nThere are this database:\n\n";
					do
					{
						if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
						{
							Red();
							std::cout << "Unfortunately client did not get message from server!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}

						parsed_json = json_tokener_parse(buffer);
						if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
						{
							Red();
							std::cout << "Incorrect style of json information!\n";
							exit(EXIT_FAILURE);
						}

						if (!strcmp(json_object_get_string(mode), "empty"))
						{
							Red();
							std::cout << "This database is empty!\n";
							Green();
							break;
						}

						if (!strcmp(json_object_get_string(mode), "done"))
							break;
				
						if (!(json_object_object_get_ex(parsed_json, "ints", &array)))
						{
							Red();
							std::cout << "Incorrect style of json information!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						for (size_t i = 0; i < int_length; ++i)
						{
							element = json_object_array_get_idx(array, i);
							std::cout << int_titles[i] << ": " << json_object_get_int(element) << std::endl;
						}

						if (!(json_object_object_get_ex(parsed_json, "doubles", &array)))
						{
							Red();
							std::cout << "Incorrect style of json information!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						for (size_t i = 0; i < double_length; ++i)
						{
							element = json_object_array_get_idx(array, i);
							std::cout << double_titles[i] << ": " << json_object_get_double(element) << std::endl;
						}

						if (!(json_object_object_get_ex(parsed_json, "strings", &array)))
						{
							Red();
							std::cout << "Incorrect style of json information!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						for (size_t i = 0; i < string_length; ++i)
						{
							element = json_object_array_get_idx(array, i);
							std::cout << string_titles[i] << ": " << json_object_get_string(element) << std::endl;
						}
						std::cout << std::endl;

					} while (true);
				}

				if (choice == '3')
				{
					Red();
					std::cout << login;
					Green();
					std::cout << "$ Enter information about node will be deleted:\n";
					strcpy(buffer, "{ \"mode\":\"delete\", \"ints\":[");
					std::cout << "Please, enter:\n";
					int32_t enter_int;
					char temp[10];
					for (size_t i = 0; i < int_length; ++i)
					{
						std::cout << int_titles[i] << ": ";
						while (!(scanf("%" SCNd32, &enter_int)) || std::cin.get() != '\n')
						{
							Red();
							while (std::cin.get() != '\n')
								continue;
							std::cout << "You have entered incorrect data!\n";
							Green();
							std::cout << int_titles[i] << ": ";
						}
						sprintf(temp, "%" PRId32, enter_int);
						strcat(buffer, temp);
						if (i == int_length - 1)
							strcat(buffer, "], \"");
						else
							strcat(buffer, ", ");
					}

					strcat(buffer, "doubles\":[");
					double enter_double;
					for (size_t i = 0; i < double_length; ++i)
					{
						std::cout << double_titles[i] << ": ";
						while (!(std::cin >> enter_double) || std::cin.get() != '\n')
						{
							Red();
							std::cout << "You have entered incorrect data!\n";
							std::cin.clear();
							while (std::cin.get() != '\n')
								continue;
							Green();
							std::cout << double_titles[i] << ": ";
						}
						sprintf(temp, "%lf", enter_double);
						strcat(buffer, temp);
						if (i == double_length - 1)
							strcat(buffer, "], \"");
						else
							strcat(buffer, ", ");
					}

					strcat(buffer, "strings\":[");
					char enter_string[100];
					for (size_t i = 0; i < string_length; ++i)
					{
						strcat(buffer, "\"");
						while (true)
						{
							std::cout << string_titles[i] << ": ";				
							std::cin.getline(enter_string, 100);
							size_t m = 0;
							bool status = false;
							while (enter_string[m] != '\0')
							{
								if (enter_string[m] == ' ')
								{
									Red();
									std::cout << "You have not use whitespaces!\n";
									Green();
									status = true;
									break;
								}
								m++;
							}

							if (!status)
								break;
						}

						strcat(buffer, enter_string);
						if (i == string_length - 1)
							strcat(buffer, "\"] }");
						else
							strcat(buffer, "\", ");
					}

					if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
					{
						Red();
						std::cout << "Unfortunately client could not send message.\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
					{
						Red();
						std::cout << "Unfortunately client did not get message from server!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					parsed_json = json_tokener_parse(buffer);
					if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						exit(EXIT_FAILURE);
					}

					if (!strcmp(json_object_get_string(mode), "no"))
					{
						Red();
						std::cout << "There are no such node in this database!\n";
						Green();
						continue;
					}

					if (!strcmp(json_object_get_string(mode), "empty"))
					{
						Red();
						std::cout << "This database is empty!\n";
						Green();
						continue;
					}

				}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (choice == '2')
				{
					strcpy(buffer, "{ \"mode\":\"add\", \"ints\":[");
					std::cout << "Please, enter:\n";
					int32_t enter_int;
					char temp[10];
					for (size_t i = 0; i < int_length; ++i)
					{
						std::cout << int_titles[i] << ": ";
						while (!(scanf("%" SCNd32, &enter_int)) || std::cin.get() != '\n')
						{
							Red();
							while (std::cin.get() != '\n')
								continue;
							std::cout << "You have entered incorrect data!\n";
							Green();
							std::cout << int_titles[i] << ": ";
						}
						sprintf(temp, "%d", enter_int);
						strcat(buffer, temp);
						if (i == int_length - 1)
							strcat(buffer, "], \"");
						else
							strcat(buffer, ", ");
					}

					strcat(buffer, "doubles\":[");
					double enter_double;
					for (size_t i = 0; i < double_length; ++i)
					{
						std::cout << double_titles[i] << ": ";
						while (!(std::cin >> enter_double) || std::cin.get() != '\n')
						{
							Red();
							std::cout << "You have entered incorrect data!\n";
							std::cin.clear();
							while (std::cin.get() != '\n')
								continue;
							Green();
							std::cout << double_titles[i] << ": ";
						}
						sprintf(temp, "%lf", enter_double);
						strcat(buffer, temp);
						if (i == double_length - 1)
							strcat(buffer, "], \"");
						else
							strcat(buffer, ", ");
					}

					strcat(buffer, "strings\":[");
					char enter_string[100];
					for (size_t i = 0; i < string_length; ++i)
					{
						strcat(buffer, "\"");
						while (true)
						{
							std::cout << string_titles[i] << ": ";				
							std::cin.getline(enter_string, 100);
							size_t m = 0;
							bool status = false;
							while (enter_string[m] != '\0')
							{
								if (enter_string[m] == ' ')
								{
									Red();
									std::cout << "You have not use whitespaces!\n";
									Green();
									status = true;
									break;
								}
								m++;
							}

							if (!status)
								break;
						}

						strcat(buffer, enter_string);
						if (i == string_length - 1)
							strcat(buffer, "\"] }");
						else
							strcat(buffer, "\", ");
					}

					if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
					{
						Red();
						std::cout << "Unfortunately client could not send message.\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}


				}
		 	} // New end

			for (size_t i = 0; i < int_length; ++i)
				delete[] int_titles[i];
			delete[] int_titles;

			for (size_t i = 0; i < double_length; ++i)
				delete[] double_titles[i];
			delete[] double_titles;

			for (size_t i = 0; i < string_length; ++i)
				delete[] string_titles[i];
			delete[] string_titles;
        } // If working
	}	
}

void Parsing_feedback(char* buffer)
{
	struct json_object *parsed_json, *mode;

	parsed_json = json_tokener_parse(buffer);
	if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
	{
		Red();
		std::cout << "Incorrect style of json information!\n";
		exit(EXIT_FAILURE);
	}

	strcpy(buffer, json_object_get_string(mode));
}

void Checking_int16(int16_t& number, char* login)
{
	while (!(scanf("%" SCNd16, &number)) || number <= 0 || std::cin.get() != '\n')
	{
		Red();
		std::cout << "You have entered incorrect data!\n";
		while (std::cin.get() != '\n')
			continue;
		std::cout << login;
		Green();
		std::cout << "$ Your choice is: ";
	}
}