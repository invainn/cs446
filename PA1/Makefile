CC=g++

all:main.o

main.o: main.cpp MetaDataCode.o Config.o
	g++ -std=c++11 main.cpp MetaDataCode.o Config.o -o main.o

MetaDataCode.o: MetaDataCode.cpp
	g++ -c -std=c++11 MetaDataCode.cpp

Config.o: Config.cpp
	g++ -c -std=c++11 Config.cpp

clean: rm -rf *.o
