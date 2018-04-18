CC=g++

all:sim04

sim04: main.cpp MetaDataCode.o Config.o Process.o
	g++ -pthread -std=c++11 main.cpp MetaDataCode.o Config.o Process.o -o sim04

MetaDataCode.o: MetaDataCode.cpp
	g++ -pthread -c -std=c++11 MetaDataCode.cpp

Config.o: Config.cpp
	g++ -pthread -c -std=c++11 Config.cpp

Process.o: Process.cpp
	g++ -pthread -c -std=c++11 Process.cpp

clean: rm -rf sim04 Config.o MetaDataCode.o Process.o