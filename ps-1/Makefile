CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = do-command
SRCS = do-command.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
	
.PHONY: all clean
