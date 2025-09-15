#include <iostream>
#include <cstdlib>

int myMax(int a, int b, int c) {
    int maxVal = a;
    if (b > maxVal) maxVal = b;
    if (c > maxVal) maxVal = c;
    return maxVal;
}

int myMin(int a, int b, int c) {
    int minVal = a;
    if (b < minVal) minVal = b;
    if (c < minVal) minVal = c;
    return minVal;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "use three nums \n";
    }

    int a = std::atoi(argv[1]);
    int b = std::atoi(argv[2]);
    int c = std::atoi(argv[3]);

    std::cout << "min - " << myMin(a, b, c) << "\n";
    std::cout << "max - " << myMax(a, b, c) << "\n";
}
