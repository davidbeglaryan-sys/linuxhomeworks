CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -fPIC
LDLIBS = -lpthread

SCHEDULER_SRC = parallel_scheduler.cpp
SCHEDULER_OBJ = parallel_scheduler.o
SCHEDULER_LIB = libparallel_scheduler.so # Важно: имя должно начинаться с 'lib'
DEMO_SRC = demo_application.cpp
DEMO_EXEC = demo-application

.PHONY: all clean

all: $(DEMO_EXEC)

$(SCHEDULER_LIB): $(SCHEDULER_OBJ)
	$(CXX) -shared $(SCHEDULER_OBJ) -o $@ $(LDLIBS)

$(SCHEDULER_OBJ): $(SCHEDULER_SRC) parallel_scheduler.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEMO_EXEC): $(DEMO_SRC) $(SCHEDULER_LIB)
	
	$(CXX) $(CXXFLAGS) $< -o $@ -L. -lparallel_scheduler $(LDLIBS)

clean:
	rm -f $(SCHEDULER_OBJ) $(SCHEDULER_LIB) $(DEMO_EXEC)
