./execatable/client: ./object_files/client.o ./object_files/autorization.o
	g++ -o ./executable/client ./object_files/client.o ./object_files/autorization.o -ljson-c
./object_files/client.o: ./source_files/client.cpp ./header_files/client.h
	g++ -o ./object_files/client.o -c ./source_files/client.cpp
./object_files/autorization.o: ./source_files/autorization.cpp ./header_files/autorization.h
	g++ -o ./object_files/autorization.o -c ./source_files/autorization.cpp
