TARGET = server

CXX = g++

CXXFLAGS = -std=c++17 -pthread

SRCS = main.cpp Socket.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
