.PHONY: clean all

all : sample

main.o : main.cpp
	g++ -O2 -c -std=c++14 -Wall $<
sample : main.o
	g++ -std=c++14 -Wall main.o -O2 -o sample
clean :
	rm -rf sample main.o
