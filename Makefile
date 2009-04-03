CXX=g++
CXXFLAGS := $(CXXFLAGS) -W -Wall -ansi

all: lambda lambda-timing

debug:
	$(MAKE) all "CXXFLAGS=-g -DEBUG"

lambda: test.cpp
	$(CXX) -o lambda $(CXXFLAGS) -O3 test.cpp

lambda-timing: test-w-time.cpp
	$(CXX) -o lambda-timing-O0 $(CXXFLAGS) -O0 test-w-time.cpp
	$(CXX) -o lambda-timing-O1 $(CXXFLAGS) -O1 test-w-time.cpp
	$(CXX) -o lambda-timing-O2 $(CXXFLAGS) -O2 test-w-time.cpp
	$(CXX) -o lambda-timing-O3 $(CXXFLAGS) -O3 test-w-time.cpp

clean:
	rm -f *.o lambda lambda-timing-O*
