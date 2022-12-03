#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include "../shared/base.hpp"

void part1(std::ifstream& input) {
    long score = 0;

    while (input.good()) {
        std::string line;
        std::getline(input, line);

        std::unordered_set<char> first_compartment;

        int i = 0;
        for (; i < line.length() / 2; i++) {
            first_compartment.insert(line[i]);
        }

        for (; i < line.length(); i++) {
            if (first_compartment.count(line[i]) > 0) {
                if (line[i] >= 'a' && line[i] <= 'z') {
                    score += line[i] - 'a' + 1;
                } else {
                    score += line[i] - 'A' + 27;
                }
                break;
            }
        }
    }

    std::cout << score << std::endl;
}

void part2(std::ifstream& input) {
    long score = 0;

    while (input.good()) {
        std::string line1, line2, line3;
        std::getline(input, line1);
        std::getline(input, line2);
        std::getline(input, line3);

        std::unordered_set<char> first_items, second_items;

        for (char item : line1) {
            first_items.insert(item);
        }
        for (char item : line2) {
            second_items.insert(item);
        }

        for (char item : line3) {
            if (first_items.count(item) > 0 && second_items.count(item) > 0) {
                if (item >= 'a' && item <= 'z') {
                    score += item - 'a' + 1;
                } else {
                    score += item - 'A' + 27;
                }
                break;
            }
        }
    }

    std::cout << score << std::endl;
}
