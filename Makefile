# Build with:  make          -> ./price_filter
# Tests:       make test
# Benchmark:   make benchmark
# Clean up:    make clean

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
INCLUDES  = -Isrc

CORE = src/Utils.cpp src/DataGenerator.cpp src/DataLoader.cpp \
       src/MergeSort.cpp src/QuickSort.cpp src/Benchmark.cpp \
       src/ProductFilter.cpp src/Menu.cpp
HEADERS = $(wildcard src/*.h)

all: price_filter

price_filter: $(CORE) src/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(CORE) src/main.cpp

run_tests: $(CORE) tests/tests.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(CORE) tests/tests.cpp

test: run_tests
	./run_tests

benchmark: price_filter
	./price_filter --benchmark

clean:
	rm -f price_filter run_tests price_filter.exe run_tests.exe

.PHONY: all test benchmark clean
