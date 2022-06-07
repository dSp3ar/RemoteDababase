#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <cstdio>
#include <json-c/json.h>
#include "../header_files/server.h"


int main(int argc, const char* argv[])
{
	int32_t client_deskriptor, server_deskriptor;
	struct sockaddr_in server_address;
	const int16_t PORT = 1615; // You can change it
	const int16_t MAX_CONNECTION = 1; // You can change it

	Green();

	if ((server_deskriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Red();
		std::cout << "Sorry, but socket was not created for server!\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Nice! Socket was created for server!\n";

	server_address.sin_port = htons(PORT); // Network byte order
	server_address.sin_family = AF_INET; // For network protocol IPv4
	server_address.sin_addr.s_addr = htons(INADDR_ANY); // I don't need to bind a socket to a specific IP

	if ((bind(server_deskriptor, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address))) < 0) // Binding socket with port and ip address
	{
		Red();
		std::cout << "Socket was not binded with specific address!\n";
		close(server_deskriptor);
		exit(EXIT_FAILURE);
	}
	std::cout << "Socket was binded!\n";

	std::cout << "Listening clients...\n";
	if (listen(server_deskriptor, MAX_CONNECTION) < 0) // Preparating socket to get information
	{
		Red();
		std::cout << "Failed listening!\n";
		close(server_deskriptor);
		exit(EXIT_FAILURE);
	}
	std::cout << "Socket is ready to get information!\n";

	socklen_t size_of_server_address = sizeof(server_address);
	if ((client_deskriptor = accept(server_deskriptor, reinterpret_cast <struct sockaddr*>(&server_address), &size_of_server_address)) < 0)
	{
		Red();
		std::cout << "Request acceptance was not found!\n";
		close(server_deskriptor);
		exit(EXIT_FAILURE);
	}
	std::cout << "Client and server deskriptor were conected!\n";

	close(server_deskriptor);

	//////////////////////////////////////////////////
	if ((send(client_deskriptor, "Server is connected!\n", sizeof("Server is connected!\n"), 0) < 0))
	{
		Red();
		std::cout << "Unfortunately server could not send message.\n";
		close(client_deskriptor);
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "Connected to the client!\n";

	char login[100];
	std::cout << "There are logs:\n";
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	do
	{
		char* autorization_mode = Check_autorization(client_deskriptor, login);
		memset(buffer, 0, BUFFER_SIZE);
		strcpy(buffer, "{ \"mode\":\"");
		strcat(buffer, autorization_mode);
		strcat(buffer, "\" }");

		if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
		{
			Red();
			std::cout << "Unfortunately server could not send message.\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		std::cout << buffer << std::endl;

		if (!strcmp(autorization_mode, "ok"))
		{	
			delete[] autorization_mode;
			break;
		}
		delete[] autorization_mode;
	} while (true);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char aux_buffer[100];
	while (true)
	{
		if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
		{
			Red();
			std::cout << "Unfortunately server did not get message from client!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		std::cout << buffer << std::endl;

		struct json_object *parsed_json, *mode, *name, *int_number, *double_number, *string_number, *array, *element;
		size_t length;

		parsed_json = json_tokener_parse(buffer);
		if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
		{
			Red();
			std::cout << "Incorrect style of json information!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		if (!strcmp(json_object_get_string(mode), "exit"))
		{
			std::cout << "Session ended!\n";
			close(client_deskriptor);
			return 0;
		}

		if (!strcmp(json_object_get_string(mode), "creating"))
		{
			int16_t number = 0;
			FILE* stream;
			char temp[100], read_name[100], database_name[100], aux[10];


			if (!(json_object_object_get_ex(parsed_json, "name", &name)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			strcpy(database_name, json_object_get_string(name));
			strcpy(aux_buffer, "no");
			do
			{
				number++;
				strcpy(temp, "../databases/");
				sprintf(aux, "%d", number);
				strcat(temp, aux);
				strcat(temp, "_info");
				if ((stream = fopen(temp, "r")) == NULL)
					break;
				else
				{	
					fscanf(stream, "%s", read_name);
					if (!strcmp(read_name, database_name))
					{
						strcpy(aux_buffer, "exist");
						fclose(stream);
						break;
					}
					fclose(stream);
				}
			} while(true);

			if (strcmp(aux_buffer, "exist"))
			{
				strcpy(temp, "../databases/");
				sprintf(aux, "%d", number);
				strcat(temp, aux);
				strcat(temp, "_info");
				if ((stream = fopen(temp, "w")) == NULL)
				{
					Red();
					std::cout << "Undefined error!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				fprintf(stream, "%s\n", database_name);

				if (!(json_object_object_get_ex(parsed_json, "int", &int_number)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				fprintf(stream, "%d\n", json_object_get_int(int_number));

				if (!(json_object_object_get_ex(parsed_json, "double", &double_number)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				fprintf(stream, "%d\n", json_object_get_int(double_number));

				if (!(json_object_object_get_ex(parsed_json, "string", &string_number)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				fprintf(stream, "%d\n", json_object_get_int(string_number));

				if (!(json_object_object_get_ex(parsed_json, "ints", &array)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				length = json_object_array_length(array);

				for (size_t i = 0; i < length; ++i)
				{
					element = json_object_array_get_idx(array, i);
					fprintf(stream, "%s\n", json_object_get_string(element));
				}

				if (!(json_object_object_get_ex(parsed_json, "doubles", &array)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				length = json_object_array_length(array);

				for (size_t i = 0; i < length; ++i)
				{
					element = json_object_array_get_idx(array, i);
					fprintf(stream, "%s\n", json_object_get_string(element));
				}

				if (!(json_object_object_get_ex(parsed_json, "strings", &array)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				length = json_object_array_length(array);

				for (size_t i = 0; i < length; ++i)
				{
					element = json_object_array_get_idx(array, i);
					fprintf(stream, "%s\n", json_object_get_string(element));
				}

				fclose(stream);
				strcpy(aux_buffer, "ok");
			}
			

			strcpy(buffer, "{ \"mode\":\"");
			strcat(buffer, aux_buffer);
			strcat(buffer, "\" }");

			if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
			{
				Red();
				std::cout << "Unfortunately server could not send message.\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}

			std::cout << buffer << std::endl;
		}

		if (!strcmp(json_object_get_string(mode), "working")) // WORKING//////////////////////////////////////////////////////////////////////////////////////
		{
			int16_t number = 0;
			FILE* stream;
			char temp[100], read_name[100], database_name[100], aux[10];

			if (!(json_object_object_get_ex(parsed_json, "name", &name)))
			{
				Red();
				std::cout << "Incorrect style of json information!\n";
				close(client_deskriptor);
				exit(EXIT_FAILURE);
			}
			strcpy(database_name, json_object_get_string(name));
			strcpy(aux_buffer, "no");

			do
			{
				number++;
				strcpy(temp, "../databases/");
				sprintf(aux, "%d", number);
				strcat(temp, aux);
				strcat(temp, "_info");
				if ((stream = fopen(temp, "r")) == NULL)
					break;
				else
				{	
					fscanf(stream, "%s", read_name);
					if (!strcmp(read_name, database_name))
					{
						strcpy(aux_buffer, "ok");
						fclose(stream);
						break;
					}
					fclose(stream);
				}
			} while(true);

			int16_t int_number, double_number, string_number;
			if (strcmp(aux_buffer, "ok"))
			{
				strcpy(buffer, "{ \"mode\":\"no\" }");
				if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
				{
					Red();
					std::cout << "Unfortunately server could not send message.\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				std::cout << buffer << std::endl;
				continue;

			}
			else
			{
				strcpy(temp, "../databases/");
				sprintf(aux, "%d", number);
				strcat(temp, aux);
				strcat(temp, "_info");
				if ((stream = fopen(temp, "r")) == NULL)
				{
					Red();
					std::cout << "Undefined error!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				char database_name[100], title[100];

				fscanf(stream, "%s", database_name);
				fgetc(stream);
				fscanf(stream, "%" SCNd16, &int_number);
				fgetc(stream);
				fscanf(stream, "%" SCNd16, &double_number);
				fgetc(stream);
				fscanf(stream, "%" SCNd16, &string_number);
				fgetc(stream);

				strcpy(buffer, "{ \"mode\":\"ok\", \"int\":");
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
					fscanf(stream, "%s", title);
					fgetc(stream);
					strcat(buffer, title);
					if (i == int_number - 1)
						strcat(buffer, "\"], ");
					else
						strcat(buffer, "\", ");
				}

				strcat(buffer, "\"doubles\":[");
				for (size_t i = 0; i < double_number; ++i)
				{
					strcat(buffer, "\"");
					fscanf(stream, "%s", title);
					fgetc(stream);
					strcat(buffer, title);
					if (i == double_number - 1)
						strcat(buffer, "\"], ");
					else
						strcat(buffer, "\", ");
				}	

				strcat(buffer, "\"strings\":[");
				for (size_t i = 0; i < string_number; ++i)
				{
					strcat(buffer, "\"");
					fscanf(stream, "%s", title);
					fgetc(stream);
					strcat(buffer, title);
					if (i == string_number - 1)
						strcat(buffer, "\"] }");
					else
						strcat(buffer, "\", ");
				}

				fclose(stream);

				std::cout << "Buffer > " << buffer << std::endl;

				if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
				{
					Red();
					std::cout << "Unfortunately client could not send message.\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				std::cout << buffer << std::endl;
			}
			
			while (true) // NEW MENU
			{
				if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
				{
					Red();
					std::cout << "Unfortunately server did not get message from client!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}
				std::cout << buffer << std::endl;

				struct json_object *parsed_json, *mode;

				parsed_json = json_tokener_parse(buffer);
				if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
				{
					Red();
					std::cout << "Incorrect style of json information!\n";
					close(client_deskriptor);
					exit(EXIT_FAILURE);
				}

				if (!strcmp(json_object_get_string(mode), "back"))
					break;

				if (!strcmp(json_object_get_string(mode), "delete")) // Deleting//////////////////////////////////////////////////////////////////////
				{
					int32_t* integer_values = NULL;
					double* double_values = NULL;
					char** string_values = NULL;
					size_t int_n, double_n, string_n;

					strcpy(temp, "../databases/");
					sprintf(aux, "%d", number);
					strcat(temp, aux);
					if ((stream = fopen(temp, "r")) == NULL)
					{
						strcpy(buffer, "{ \"mode\":\"empty\" }");
						if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
						{
							Red();
							std::cout << "Unfortunately server could not send message.\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						continue;
					}

					fseek(stream, 0, SEEK_SET);
					size_t check_start = ftell(stream);
					fseek(stream, 0, SEEK_END);
					size_t check_end = ftell(stream);

					if (check_start == check_end)
					{
						strcpy(buffer, "{ \"mode\":\"empty\" }");
						if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
						{
							Red();
							std::cout << "Unfortunately server could not send message.\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						continue;
					}

					fseek(stream, 0, SEEK_SET);

					if (!(json_object_object_get_ex(parsed_json, "ints", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					int_n = json_object_array_length(array);

					if ((integer_values = new int32_t[int_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					for (size_t i = 0; i < int_n; ++i)
					{
						element = json_object_array_get_idx(array, i);
						integer_values[i] = json_object_get_int(element);
					}

					if (!(json_object_object_get_ex(parsed_json, "doubles", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					double_n = json_object_array_length(array);

					if ((double_values = new double[double_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					for (size_t i = 0; i < double_n; ++i)
					{
						element = json_object_array_get_idx(array, i);
						double_values[i] = json_object_get_double(element);
					}

					if (!(json_object_object_get_ex(parsed_json, "strings", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					string_n = json_object_array_length(array);

					if ((string_values = new char*[string_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}
					for (size_t i = 0; i < string_n; ++i)
						if ((string_values[i] = new char[100]) == NULL)
						{
							Red();
							std::cout << "Memory was not alloced!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}

					for (size_t i = 0; i < string_n; ++i)
					{
						element = json_object_array_get_idx(array, i);
						strcpy(string_values[i], json_object_get_string(element));
					}

					int32_t* integer_read = NULL;
					double* double_read = NULL;
					char** string_read = NULL;

					if ((integer_read = new int32_t[int_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}
					
					if ((double_read = new double[double_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					if ((string_read = new char*[string_n]) == NULL)
					{
						Red();
						std::cout << "Memory was not alloced!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}
					for (size_t i = 0; i < string_n; ++i)
						if ((string_read[i] = new char[100]) == NULL)
						{
							Red();
							std::cout << "Memory was not alloced!\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}

					FILE* new_stream = NULL;
					strcpy(temp, "../databases/temp");

					if ((new_stream = fopen(temp, "w")) == NULL)
					{
						Red();
						std::cout << "Undefined error!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					fseek(stream, 0, SEEK_SET);
					bool is = false;
					bool* flag = NULL;

					if ((flag = new bool[int_n + double_n + string_n]) == NULL)
					{
						Red();
						std::cout << "Undefined error!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					while (!feof(stream))
					{
						size_t k = 0;
						for (size_t i = 0; i < int_n; ++i, k++)
						{
							fscanf(stream, "%" SCNd32, &integer_read[i]);
							if (integer_read[i] == integer_values[i])
								flag[k] = true;
							else
								flag[k] = false;
						}

						for (size_t i = 0; i < double_n; ++i, k++)
						{
							fscanf(stream, "%lf", &double_read[i]);
							if (double_read[i] == double_values[i])
								flag[k] = true;
							else
								flag[k] = false;
						}

						for (size_t i = 0; i < string_n; ++i, k++)
						{
							fscanf(stream, "%s", string_read[i]);
							if (!strcmp(string_read[i], string_values[i]))
								flag[k] = true;
							else
								flag[k] = false;
						}

						if (feof(stream))
							break;

						bool new_flag = true;
						for (k = 0; k < int_n + double_n + string_n; k++)
							if (flag[k] == false)
							{
								new_flag = false;
								break;
							}

						if (!new_flag)
						{
							for (size_t i = 0; i < int_n; ++i)
								fprintf(new_stream, "%" PRId32 "\n", integer_read[i]);

							for (size_t i = 0; i < double_n; ++i)
								fprintf(new_stream, "%lf\n", double_read[i]);

							for (size_t i = 0; i < string_n; ++i)
								fprintf(new_stream, "%s\n", string_read[i]);

							fprintf(new_stream, "\n");
						}
						else
							is = true;

					}

					fclose(stream);
					strcpy(temp, "../databases/");
					sprintf(aux, "%d", number);
					strcat(temp, aux);

					remove(temp);
					rename("../databases/temp", temp);
					fclose(new_stream);

					
					delete[] integer_values;
					delete[] double_values;
					delete[] integer_read;
					delete[] double_read;
					for (size_t i = 0; i < string_n; ++i)
					{
						delete[] string_values[i];
						delete[] string_read[i];
					}
					delete[] string_values;
					delete[] string_read;
					delete[] flag;

					if (is)
						strcpy(buffer, "{ \"mode\":\"ok\" }");
					else
						strcpy(buffer, "{ \"mode\":\"no\" }");

					if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
					{
						Red();
						std::cout << "Unfortunately server could not send message.\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}
					std::cout << buffer << std::endl;

				}////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				if (!strcmp(json_object_get_string(mode), "show")) // Showing
				{
					strcpy(temp, "../databases/");
					sprintf(aux, "%d", number);
					strcat(temp, aux);
					if ((stream = fopen(temp, "r")) == NULL)
					{
						strcpy(buffer, "{ \"mode\":\"empty\" }");
						if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
						{
							Red();
							std::cout << "Unfortunately server could not send message.\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						std::cout << buffer << std::endl;
						continue;
					}

					else
					{
						fseek(stream, 0, SEEK_SET);
						size_t check_start = ftell(stream);
						fseek(stream, 0, SEEK_END);
						size_t check_end = ftell(stream);

						if (check_start == check_end)
						{
							strcpy(buffer, "{ \"mode\":\"empty\" }");
							if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
							{
								Red();
								std::cout << "Unfortunately server could not send message.\n";
								close(client_deskriptor);
								exit(EXIT_FAILURE);
							}
							std::cout << buffer << std::endl;
							continue;
						}

						fseek(stream, 0, SEEK_SET);

						char string_v[100];
						int32_t int_v;
						double double_v;
						while (!feof(stream))
						{
							strcpy(buffer, "{ \"mode\":\"ok\", \"ints\":[");
							for (size_t j = 0; j < int_number; j++)
							{
								fscanf(stream, "%" SCNd32, &int_v);
								sprintf(string_v, "%" PRId32, int_v);
								strcat(buffer, string_v);
								if (j == int_number - 1)
									strcat(buffer, "], \"");
								else
									strcat(buffer, ", ");
							}

							strcat(buffer, "doubles\":[");
							for (size_t j = 0; j < double_number; j++)
							{
								fscanf(stream, "%lf", &double_v);
								sprintf(string_v, "%lf", double_v);
								strcat(buffer, string_v);
								if (j == double_number - 1)
									strcat(buffer, "], \"");
								else
									strcat(buffer, ", ");
							}

							strcat(buffer, "strings\":[");
							for (size_t j = 0; j < string_number; j++)
							{
								strcat(buffer, "\"");
								fscanf(stream, "%s", string_v);
								fgetc(stream);
								strcat(buffer, string_v);
								if (j == string_number - 1)
									strcat(buffer, "\"] }");
								else
									strcat(buffer, "\", ");
							}
							fgetc(stream);

							if (feof(stream))
								break;
							std::cout << "Here buffer: " << buffer << std::endl;

							if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
							{
								Red();
								std::cout << "Unfortunately server could not send message.\n";
								close(client_deskriptor);
								exit(EXIT_FAILURE);
							}
						}
						fclose(stream);
						strcpy(buffer, "{ \"mode\":\"done\" }");
						if ((send(client_deskriptor, buffer, BUFFER_SIZE, 0) < 0))
						{
							Red();
							std::cout << "Unfortunately server could not send message.\n";
							close(client_deskriptor);
							exit(EXIT_FAILURE);
						}
						std::cout << buffer << std::endl;

					}

				}

				if (!strcmp(json_object_get_string(mode), "add")) // Add new element
				{
					strcpy(temp, "../databases/");
					sprintf(aux, "%d", number);
					strcat(temp, aux);
					if ((stream = fopen(temp, "a")) == NULL)
					{
						Red();
						std::cout << "Undefined error!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					fseek(stream, 0, SEEK_END);

					if (!(json_object_object_get_ex(parsed_json, "ints", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					length = json_object_array_length(array);

					for (size_t i = 0; i < length; ++i)
					{
						element = json_object_array_get_idx(array, i);
						fprintf(stream, "%d\n", json_object_get_int(element));
					}

					if (!(json_object_object_get_ex(parsed_json, "doubles", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					length = json_object_array_length(array);

					for (size_t i = 0; i < length; ++i)
					{
						element = json_object_array_get_idx(array, i);
						fprintf(stream, "%lf\n", json_object_get_double(element));
					}

					if (!(json_object_object_get_ex(parsed_json, "strings", &array)))
					{
						Red();
						std::cout << "Incorrect style of json information!\n";
						close(client_deskriptor);
						exit(EXIT_FAILURE);
					}

					length = json_object_array_length(array);

					for (size_t i = 0; i < length; ++i)
					{
						element = json_object_array_get_idx(array, i);
						fprintf(stream, "%s\n", json_object_get_string(element));
					}

					fprintf(stream, "\n");
					fclose(stream);

				}
		    } // End new

		} // End working
	}
}



char* Check_autorization(int32_t& client_deskriptor, char* save_login)
{
	if ((recv(client_deskriptor, buffer, BUFFER_SIZE, 0)) < 0)
		{
			Red();
			std::cout << "Unfortunately server did not get message from client!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}
		std::cout << buffer << std::endl;
		struct json_object *parsed_json, *mode, *login, *password;

		parsed_json = json_tokener_parse(buffer);
		if (!(json_object_object_get_ex(parsed_json, "mode", &mode)))
		{
			Red();
			std::cout << "Incorrect style of json information!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		if (!(json_object_object_get_ex(parsed_json, "login", &login)))
		{
			Red();
			std::cout << "Incorrect style of json information!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		if (!(json_object_object_get_ex(parsed_json, "password", &password)))
		{
			Red();
			std::cout << "Incorrect style of json information!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		char* aux_buffer = NULL;
		if ((aux_buffer = new char[50]) == NULL)
		{
			Red();
			std::cout << "There are not free memory!\n";
			close(client_deskriptor);
			exit(EXIT_FAILURE);
		}

		FILE* stream;
		if (!strcmp(json_object_get_string(mode), "registration")) // If registration
		{
			if ((stream = fopen("../accounts/accounts", "r+")) == NULL) // If file is not exist
			{
				if ((stream = fopen("../accounts/accounts", "w")) == NULL)
				{
					Red();
					std::cout << "Undefined error!\n";
					exit(EXIT_FAILURE);
				}

				fprintf(stream, "%s\n", json_object_get_string(login));
				fprintf(stream, "%s\n\n", json_object_get_string(password));
				strcpy(save_login, json_object_get_string(login));
				fclose(stream);
				strcpy(aux_buffer, "ok");
				return aux_buffer;
			}

			//If file is exist already
			char login_s[100], password_s[100];
			bool colision = false;
			fseek(stream, 0, SEEK_SET);

			while (!feof(stream))
			{
				fscanf(stream, "%s", login_s);
				fscanf(stream, "%s", password_s);
				fgetc(stream);
				fgetc(stream);

				if (!strcmp(login_s, json_object_get_string(login)))
				{
					colision = true;
					break;
				}
			}

			if (colision)
			{
				fclose(stream);
				strcpy(aux_buffer, "login colision");
				return aux_buffer;
			}

			fseek(stream, 0, SEEK_END);
			fprintf(stream, "%s\n", json_object_get_string(login));
			fprintf(stream, "%s\n\n", json_object_get_string(password));
			strcpy(save_login, json_object_get_string(login));
			fclose(stream);
			strcpy(aux_buffer, "ok");
			return aux_buffer;
		}

		else // If sing in
		{
			if ((stream = fopen("../accounts/accounts", "r+")) == NULL) // If file is not exist
			{
				strcpy(aux_buffer, "no file");
				return aux_buffer;
			}

			fseek(stream, 0, SEEK_SET);

			char login_s[100], password_s[100];
			bool colision = false;
			while (!feof(stream))
			{
				fscanf(stream, "%s", login_s);
				fscanf(stream, "%s", password_s);
				fgetc(stream);
				fgetc(stream);

				if (!strcmp(login_s, json_object_get_string(login)) && !strcmp(password_s, json_object_get_string(password)))
				{
					colision = true;
					break;
				}
			}

			fclose(stream);
			if (colision)
			{
				strcpy(aux_buffer, "ok");
				strcpy(save_login, json_object_get_string(login));
				return aux_buffer;
			}
			else
			{
				strcpy(aux_buffer, "no login");
				return aux_buffer;
			}
		}
	}