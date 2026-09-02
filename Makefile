CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra
LIBS = -lnotcurses -lnotcurses-core

SRC = $(wildcard src/*.cpp)
TEST_SRC = tests/test_chrono_engine.cpp

TARGET = GabaryApp
TEST_TARGET = run_tests

all: $(TARGET)

$(TARGET): chrono_main.cpp
$(CXX) $(CXXFLAGS) chrono_main.cpp $(filter-out src/main.cpp, $(SRC)) $(LIBS) -o $(TARGET)

test: $(TEST_SRC)
$(CXX) -std=c++17 $(TEST_SRC) -o $(TEST_TARGET)
./$(TEST_TARGET)

clean:
rm -f $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
