CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2

TARGET = interactive-shell

SRCS = interactive_shell.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.log

.PHONY:
