#include <iostream>
#include <fstream>
#include <string>
#include "../shared/base.hpp"

void part1(std::ifstream& input) {
    int max_calories = 0;
    int current_calories = 0;

    while (input.good()) {
        std::string line;
        std::getline(input, line);

        if (line.length() == 0) {
            if (current_calories > max_calories) {
                max_calories = current_calories;
            }
            current_calories = 0;
        } else {
            current_calories += std::stoi(line);
        }
    }

    std::cout << max_calories << std::endl;
}

void part2(std::ifstream& input) {
    int current_calories = 0;
    int max_calories[3] = {0};

    while (input.good()) {
        std::string line;
        std::getline(input, line);

        if (line.length() == 0) {
            for (int i = 2; i >= 0; i--) {
                if (current_calories > max_calories[i]) {
                    for (int j = 0; j < i; j++) {
                        max_calories[j] = max_calories[j+1];
                    }
                    max_calories[i] = current_calories;
                    break;
                }
            }
            current_calories = 0;
        } else {
            current_calories += std::stoi(line);
        }
    }

    std::cout << max_calories[0] + max_calories[1] + max_calories[2] << std::endl;
}
