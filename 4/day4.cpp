#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include "../shared/base.hpp"

void part1(std::ifstream& input) {
    int result = 0;

    while (input.good()) {
        std::string buf;

        if (!std::getline(input, buf, '-')) {
            break;
        }
        int l1 = std::stoi(buf);

        std::getline(input, buf, ',');
        int u1 = std::stoi(buf);

        std::getline(input, buf, '-');
        int l2 = std::stoi(buf);

        std::getline(input, buf);
        int u2 = std::stoi(buf);

        if ((l1 >= l2 && u1 <= u2) || (l1 <= l2 && u1 >= u2)) {
            result++;
        }
    }

    std::cout << result << std::endl;
}

void part2(std::ifstream& input) {
    int result = 0;

    while (input.good()) {
        std::string buf;

        if (!std::getline(input, buf, '-')) {
            break;
        }
        int l1 = std::stoi(buf);

        std::getline(input, buf, ',');
        int u1 = std::stoi(buf);

        std::getline(input, buf, '-');
        int l2 = std::stoi(buf);

        std::getline(input, buf);
        int u2 = std::stoi(buf);

        if ((l1 >= l2 && l1 <= u2) || (l2 >= l1 && l2 <= u1) || (u1 <= u2 && u1 >= l2) || (u2 <= u1 && u2 >= l1)) {
            result++;
        }
    }

    std::cout << result << std::endl;
}
