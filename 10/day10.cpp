#include <iostream>
#include <fstream>
#include <string>
#include "../shared/base.hpp"

void step(int& i, int x, int& result) {
    i++;
    if ((i-20) % 40 == 0) {
        result += x * i;
    }
}

void part1(std::ifstream& input) {
    int x = 1;
    int i = 0;
    int result = 0;
    std::string line;
    while (std::getline(input, line)) {
        std::string cmd = line.substr(0, 4);
        if (cmd == "addx") {
            int arg = std::stoi(line.substr(5));
            step(i, x, result);
            step(i, x, result);
            x += arg;
        } else if (cmd == "noop") {
            step(i, x, result);
        } else {
            std::cerr << "Unexpected operation " << cmd << std::endl;
            return;
        }
    }
    std::cout << result << " " << i << std::endl;
}

void draw(int& pos, int x) {
    if (pos == x-1 || pos == x || pos == x+1) {
        std::cout << '#';
    } else {
        std::cout << ' ';
    }
    pos = (pos+1) % 40;
    if (pos % 40 == 0) {
        std::cout << std::endl;
    }
}

void part2(std::ifstream& input) {
    int x = 1;
    int pos = 0;
    std::string line;
    while (std::getline(input, line)) {
        std::string cmd = line.substr(0, 4);
        if (cmd == "addx") {
            int arg = std::stoi(line.substr(5));
            draw(pos, x);
            draw(pos, x);
            x += arg;
        } else if (cmd == "noop") {
            draw(pos, x);
        }
    }
}
