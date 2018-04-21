CC=g++

all:sim04

sim04: main.cpp MetaDataCode.o Config.o Process.o Log.o OperatingSystem.o Scheduler.o
	g++ -pthread -std=c++11 main.cpp MetaDataCode.o Config.o Process.o Log.o OperatingSystem.o Scheduler.o -o sim04

MetaDataCode.o: MetaDataCode.cpp
	g++ -pthread -c -std=c++11 MetaDataCode.cpp

Config.o: Config.cpp
	g++ -pthread -c -std=c++11 Config.cpp

Process.o: Process.cpp
	g++ -pthread -c -std=c++11 Process.cpp

Log.o: Log.cpp
	g++ -pthread -c -std=c++11 Log.cpp

OperatingSystem.o: OperatingSystem.cpp
	g++ -pthread -c -std=c++11 OperatingSystem.cpp

Scheduler.o: Scheduler.cpp
	g++ -pthread -c -std=c++11 Scheduler.cpp

clean: rm -rf sim04 Config.o MetaDataCode.o Process.o Log.o OperatingSystem.o Scheduler.o