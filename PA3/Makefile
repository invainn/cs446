CC=g++

all:sim03

sim03: main.cpp MetaDataCode.o Config.o
	g++ -pthread -std=c++11 main.cpp MetaDataCode.o Config.o -o sim03

MetaDataCode.o: MetaDataCode.cpp
	g++ -pthread -c -std=c++11 MetaDataCode.cpp

Config.o: Config.cpp
	g++ -pthread -c -std=c++11 Config.cpp

clean: rm -rf sim03 Config.o MetaDataCode.o
