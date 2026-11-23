CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall
INC = -Iinclude -Isrc/structA -Isrc/structB -Ioracle -Iadversary

UNIT_SRC = $(wildcard tests/structA/test_*.cpp) $(wildcard tests/structB/test_*.cpp) $(wildcard tests/adversary/test_*.cpp) $(wildcard app/test_*.cpp)
ORACLE_SRC = $(wildcard tests/*.cpp)

UNIT_BIN = $(patsubst %.cpp,build/%,$(UNIT_SRC))
ORACLE_BIN = $(patsubst %.cpp,build/%,$(ORACLE_SRC))

all: bin/app bin/max_benchmark bin/gcd_benchmark

bin/app: app/gcd_segment_tree.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

bin/max_benchmark: bench/max_benchmark.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INC) -DSTRUCTB_DISABLE_COUNTERS $< -o $@

bin/max_counts: bench/max_benchmark.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INC) -DSTRUCTA_ENABLE_COUNTERS $< -o $@

bin/gcd_benchmark: bench/gcd_benchmark.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

bin/gcd_counts: bench/gcd_benchmark.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INC) -DSTRUCTA_ENABLE_COUNTERS $< -o $@

build/%: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

build/tests/structA/test_counters: CXXFLAGS += -DSTRUCTA_ENABLE_COUNTERS

test: bin/app $(UNIT_BIN) $(ORACLE_BIN)
	@set -e; \
	for t in $(UNIT_BIN) $(ORACLE_BIN); do \
		echo "== $$t"; ./$$t > /dev/null; \
	done; \
	for b in a b-hybrid; do \
		echo "== app --backend=$$b sample"; ./bin/app --backend=$$b < app/sample.in | diff - app/sample.out; \
		echo "== app --backend=$$b --test"; ./bin/app --backend=$$b --test > /dev/null; \
	done; \
	echo "ALL TESTS PASSED"

clean:
	rm -rf bin build

.PHONY: all test clean
