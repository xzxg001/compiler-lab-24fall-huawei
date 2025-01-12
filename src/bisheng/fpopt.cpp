#include <iostream>

int main() {
    double x = 1.0;
    for (int i = 0; i < 1000000; ++i) {
        x += 0.0000001 * x;
    }
    std::cout.precision(15);
    std::cout << "Result: " << x << std::endl;
    return 0;
}
