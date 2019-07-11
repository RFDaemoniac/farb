CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wfatal-errors -Wno-gnu-statement-expression -Wno-unused-function

MODULES = core interface reflection serialization utils
VPATH = tests/ src/ $(addprefix tests/, $(MODULES)) $(addprefix src/, $(MODULES))

LIB_DEPENDENCIES = $(wildcard lib/*/*.h*)
LIB_FILES = $(wildcard lib/*/*.c*)
LIB_OBJECTS = $(addprefix build/tmp/, $(notdir $(LIB_FILES:%.c=%.o)))

SOURCE_DEPENDENCIES = $(wildcard src/*/*.h*)
SOURCE_FILES = $(wildcard src/*/*.cpp)
SOURCE_OBJECTS = $(addprefix build/tmp/, $(notdir $(SOURCE_FILES:%.cpp=%.o)))
# this excludes the final target Farb.o

TEST_DEPENDENCIES = $(wildcard tests/*/*.h*)
# there are no test files or objects other than RunTests, which is specified explicity

all: build/bin/test build/bin/farb.a

test: build/bin/test
	./build/bin/test

build/tmp/tigr.o: lib/tigr/tigr.c
	gcc -c -o build/tmp/tigr.o lib/tigr/tigr.c

build/tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/bin/test: build/tmp/RunTests.o $(TEST_DEPENDENCIES) $(SOURCE_DEPENDENCIES) $(SOURCE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o ./build/bin/test build/tmp/RunTests.o $(SOURCE_OBJECTS)

build/bin/farb.a: $(SOURCE_DEPENDENCIES) $(SOURCE_OBJECTS) $(LIB_DEPENDENCIES) $(LIB_OBJECTS)
	ar rvs $(SOURCE_OBJECTS) $(LIB_OBJECTS)

clean:
	rm build/tmp/* build/bin/*

# g++ -std=c++17 -Wall -pedantic -Wfatal-errors -o ./build/bin/test tests/RunTests.cpp src/serialization/Deserialization.cpp
