#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include "../shared/base.hpp"

void part1(std::ifstream& input) {
    std::string str;
    std::getline(input, str);

    for (int i = 0; i + 4 < str.length(); i++) {
        std::unordered_set<char> set (4);
        set.insert(str[i]);
        if (set.insert(str[i+1]).second && set.insert(str[i+2]).second && set.insert(str[i+3]).second) {
            std::cout << i + 4 << std::endl;
            return;
        }
    }
}

void part2(std::ifstream& input) {
    std::string str;
    std::getline(input, str);

    for (int i = 0; i + 14 < str.length(); i++) {
        std::unordered_set<char> set (4);
        bool match = true;
        for (int j = 0; j < 14; j++) {
            if (!set.insert(str[i+j]).second) {
                match = false;
                break;
            }
        }
        if (match) {
            std::cout << i + 14 << std::endl;
            return;
        }
    }
}
