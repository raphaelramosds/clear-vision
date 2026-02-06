#include <iostream>
#include "cvision.hpp"

int main() {
    cvision::bootstrap("cvision.conf");
    std::cout << "Cvision bootstrap completed successfully." << std::endl;
    return 0;
}