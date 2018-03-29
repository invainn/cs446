CC=g++

all:sim02

sim02: main.cpp MetaDataCode.o Config.o
	g++ -pthread -std=c++11 main.cpp MetaDataCode.o Config.o -o sim02

MetaDataCode.o: MetaDataCode.cpp
	g++ -pthread -c -std=c++11 MetaDataCode.cpp

Config.o: Config.cpp
	g++ -pthread -c -std=c++11 Config.cpp

clean: rm -rf sim02
