#include <iostream>
#include <fstream>
#include <chrono>
#include "base.hpp"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Expected 3 arguments, found " << argc << std::endl;
        return 1;
    }

    auto start = std::chrono::steady_clock::now();

    std::ifstream input (argv[2]);

    if (!input.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    int part = std::stoi(argv[1]);

    if (part == 1) {
        part1(input);
    } else if (part == 2) {
        part2(input);
    } else {
        std::cerr << "Unknown part " << argv[1] << std::endl;
        return 1;
    }

    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration) << std::endl;
}
