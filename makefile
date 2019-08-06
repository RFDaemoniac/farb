CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wfatal-errors -Wno-gnu-statement-expression -Wno-unused-function
TARGET_LINKS=-framework OpenGL -framework Cocoa

# MODULES = $(sort $(dir $(wildcard src/*/)))
MODULES = core interface reflection serialization utils
VPATH = tests/ src/ $(addprefix tests/, $(MODULES)) $(addprefix src/, $(MODULES))

LIB_HEADERS = $(wildcard lib/*/*.h*)
LIB_FILES = $(wildcard lib/*/*.c*)
LIB_OBJECTS = $(addprefix build/tmp/, $(notdir $(LIB_FILES:%.c=%.o)))

SOURCE_HEADERS = $(wildcard src/*/*.h*)
SOURCE_FILES = $(wildcard src/*/*.cpp)
SOURCE_OBJECTS = $(addprefix build/tmp/, $(notdir $(SOURCE_FILES:%.cpp=%.o)))
SOURCE_INCLUDES = $(addprefix -I src/, $(MODULES))
# this excludes the final target Farb.o

TEST_HEADERS = $(wildcard tests/*/*.h*)
# there are no test files or objects other than RunTests, which is specified explicity

all: build/bin/runtests build/link/farb.a

debug: CXXFLAGS += -DDebug -g
debug: build/bin/runtests

farb: build/link/farb.a

tests: build/bin/runtests
	./build/bin/runtests

lib: build/tmp/tigr.o

build/tmp/tigr.o: lib/tigr/tigr.c lib/tigr/tigr.h
	gcc -c -o build/tmp/tigr.o lib/tigr/tigr.c -Wno-deprecated-declarations

build/tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SOURCE_INCLUDES) -c $< -o $@
	printf "build/tmp/" > build/tmp/$*.make
	$(CXX) $(CXXFLAGS) $(SOURCE_INCLUDES) -MM $< >> build/tmp/$*.make

-include $(SOURCE_OBJECTS:.o=.make)
-include $(LIB_OBJECTS:.o=.make)

build/bin/runtests: build/tmp/RunTests.o $(LIB_HEADERS) $(SOURCE_OBJECTS) $(TEST_HEADERS) $(SOURCE_HEADERS) $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(SOURCE_INCLUDES) -o ./build/bin/runtests build/tmp/RunTests.o $(SOURCE_OBJECTS) $(LIB_OBJECTS) $(TARGET_LINKS)

build/link/farb.a: $(SOURCE_HEADERS) $(SOURCE_OBJECTS) $(LIB_HEADERS) $(LIB_OBJECTS)
	ar rvs build/link/farb.a $(SOURCE_OBJECTS) $(LIB_OBJECTS)

clean:
	rm build/tmp/* build/bin/* build/link/*

# g++ -std=c++17 -Wall -pedantic -Wfatal-errors -o ./build/bin/test tests/RunTests.cpp src/serialization/Deserialization.cpp
