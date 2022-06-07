./execatable/server: ./object_files/server.o
	g++ -o ./executable/server ./object_files/server.o -ljson-c
./object_files/server.o: ./source_files/server.cpp ./header_files/server.h
	g++ -o ./object_files/server.o -c ./source_files/server.cpp