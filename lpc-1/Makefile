CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -D_GNU_SOURCE

SRC = signal-echo.cpp
EXEC = signal-echo

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(EXEC)
