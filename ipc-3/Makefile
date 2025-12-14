CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDLIBS = -lrt -pthread

FIRST_SRC = first.cpp
SECOND_SRC = second.cpp
FIRST_EXEC = first
SECOND_EXEC = second
HEADER = shared_array.hpp

ARRAY_NAME = ipc3_test_array
CLEANUP_SRC = cleanup_temp.cpp

.PHONY: all clean clean_resources

all: $(FIRST_EXEC) $(SECOND_EXEC)

$(FIRST_EXEC): $(FIRST_SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

$(SECOND_EXEC): $(SECOND_SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

$(CLEANUP_SRC): $(HEADER)
	@echo '#include "shared_array.hpp"' > $@
	@echo '#include <iostream>' >> $@
	@echo 'const std::string ARRAY_NAME = "$(ARRAY_NAME)";' >> $@
	@echo 'int main() {' >> $@
	@echo '    std::cout << "Unlinking " << ARRAY_NAME << " resources..." << std::endl;' >> $@
	@echo '    SharedArray::unlink_resources(ARRAY_NAME);' >> $@
	@echo '    return 0;' >> $@
	@echo '}' >> $@

clean_resources: $(CLEANUP_SRC)
	$(CXX) $(CXXFLAGS) $(CLEANUP_SRC) -o cleanup_tool $(LDLIBS)
	./cleanup_tool
	rm -f cleanup_tool $(CLEANUP_SRC)
	@echo "IPC-3 resources fully unlinked."

clean: clean_resources
	rm -f $(FIRST_EXEC) $(SECOND_EXEC)
