CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -I./lib -I./src/headers

all: tmp/farb.o

bin/test: tmp/perimeter-constraints.o tmp/tigr.o tmp/data.o tmp/test.o tmp/irrational-spirals.o
	g++ -o bin/test tmp/tigr.o tmp/data.o tmp/perimeter-constraints.o tmp/irrational-spirals.o tmp/test.o -framework OpenGL -framework Cocoa

tmp/farb.o: lib/tigr.c
	$(CXX) $(CXXFLAGS) -c -o tmp/test.o src/test.cpp

tmp/irrational-spirals.o: src/code/irrational-spirals.cpp
	$(CXX) $(CXXFLAGS) -c -o tmp/irrational-spirals.o src/code/irrational-spirals.cpp

tmp/perimeter-constraints.o: src/code/perimeter-constraints.cpp
	g++ -c -I./lib -I./src/headers -o tmp/perimeter-constraints.o src/code/perimeter-constraints.cpp

tmp/data.o: lib/data.cpp
	g++ -c -o tmp/data.o lib/data.cpp -std=c++11

tmp/tigr.o: lib/tigr/tigr.c
	gcc -c -o tmp/tigr.o lib/tigr/tigr.c

clean:
	rm tmp/* bin/*
