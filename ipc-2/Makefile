CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC = prime_calculator.cpp
EXEC = prime-calculator

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(EXEC)
