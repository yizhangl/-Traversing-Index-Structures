CFLAGS = -Wall

all: test

test: catch.o lbnToPbn.o test.cpp
	g++ -g -o test catch.o test.cpp lbnToPbn.o

catch.o: catch.cpp catch.hpp
	g++ -g -o catch.o -c catch.cpp

lbnToPbn.o: lbnToPbn.c lbnToPbn.h
	gcc -g $(CFLAGS) -o lbnToPbn.o -c lbnToPbn.c

clean:
	rm -rf lbnToPbn.o test

tidy:
	rm -rf *.o test
