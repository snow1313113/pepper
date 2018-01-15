.PHONY: clean all

all : sample

main.o : main.cpp
	g++ -c -std=c++11 -Wall $<
sample : main.o
	g++ -std=c++11 -Wall main.o -o sample
clean :
	rm -rf sample main.o
