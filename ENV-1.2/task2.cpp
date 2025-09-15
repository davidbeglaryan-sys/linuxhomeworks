#include <iostream>

int main(int argc, char* argv[]) {

    for (int i = argc - 1; i >= 0; --i) {
        std::cout << argv[i] << std::endl;
    }

    return 0;
}
