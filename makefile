CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wfatal-errors -Wno-gnu-statement-expression -Wno-unused-function
#-Wgnu-statement-expression
#CXXFLAGS=-std=c++17 -Wall -pedantic -I./lib -I./src/headers

#all: tmp/farb.o

#bin/test: tmp/perimeter-constraints.o tmp/tigr.o tmp/data.o tmp/test.o tmp/irrational-spirals.o
#	g++ -o bin/test tmp/tigr.o tmp/data.o tmp/perimeter-constraints.o tmp/irrational-spirals.o tmp/test.o -framework OpenGL -framework Cocoa

#tmp/farb.o: lib/tigr.c
#	$(CXX) $(CXXFLAGS) -c -o tmp/test.o src/test.cpp

#tmp/irrational-spirals.o: src/code/irrational-spirals.cpp
#	$(CXX) $(CXXFLAGS) -c -o tmp/irrational-spirals.o src/code/irrational-spirals.cpp

#tmp/perimeter-constraints.o: src/code/perimeter-constraints.cpp
#	g++ -c -I./lib -I./src/headers -o tmp/perimeter-constraints.o src/code/perimeter-constraints.cpp

#tmp/data.o: lib/data.cpp
#	g++ -c -o tmp/data.o lib/data.cpp -std=c++11

#tmp/tigr.o: lib/tigr/tigr.c
#	gcc -c -o tmp/tigr.o lib/tigr/tigr.c

VPATH = tests/reflection tests/serialization tests/ src/core src/reflection src/serialization src/utils src/interface

TEST_DEPENDENCY_FILES = $(wildcard **/*.hpp)

LIB_FILES = $(wildcard lib/**/*.*)

SOURCE_DISTINCT_FILES = $(wildcard src/**/*.cpp)

SOURCE_DEPENDENCY_FILES = $(wildcard src/**/*.hpp)

all: build/bin/test

test: build/bin/test
	./build/bin/test

build/tmp/tigr.o: lib/tigr/tigr.c
	gcc -c -o build/tmp/tigr.o lib/tigr/tigr.c

build/tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/bin/test: $(TEST_DEPENDENCY_FILES) build/tmp/RunTests.o build/tmp/Deserialization.o build/tmp/InterfaceReflectionDefinitions.o build/tmp/ReflectionBasics.o
	$(CXX) $(CXXFLAGS) -o ./build/bin/test build/tmp/RunTests.o build/tmp/Deserialization.o build/tmp/InterfaceReflectionDefinitions.o build/tmp/ReflectionBasics.o

clean:
	rm build/tmp/* build/bin/*

# g++ -std=c++17 -Wall -pedantic -Wfatal-errors -o ./build/bin/test tests/RunTests.cpp src/serialization/Deserialization.cpp
