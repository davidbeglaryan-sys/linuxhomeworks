CXX = g++
CXXFLAGS = -std=c++11 -pthread -O2
TARGET = array-summary
SOURCES = array_summary.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
