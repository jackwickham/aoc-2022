#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include "../shared/base.hpp"

// https://stackoverflow.com/questions/15160889/how-can-i-make-an-unordered-set-of-pairs-of-integers-in-c
struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int>& pair) const {
        return pair.first * 63 + pair.second;
    }
};

int sign(int v) {
    if (v == 0) {
        return 0;
    }
    return v > 0 ? 1 : -1;
}

void part1(std::ifstream& input) {
    std::pair<int, int> head (0, 0);
    std::pair<int, int> tail (0, 0);
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::string line;
    while (std::getline(input, line)) {
        char direction = line[0];
        int steps = std::stoi(line.substr(2));
        for (int i = 0; i < steps; i++) {
            if (direction == 'R') {
                head.first += 1;
            } else if (direction == 'L') {
                head.first -= 1;
            } else if (direction == 'U') {
                head.second += 1;
            } else if (direction == 'D') {
                head.second -= 1;
            } else {
                std::cerr << "Unexpected direction " << direction << std::endl;
                return;
            }

            int dx = head.first - tail.first;
            int dy = head.second - tail.second;
            int d = std::max(abs(dx), abs(dy));

            if (d > 1) {
                tail.first += sign(dx);
                tail.second += sign(dy);
            }

            visited.insert(tail);
        }
    }

    std::cout << visited.size() << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<std::pair<int, int>> knots (10, std::pair<int, int>(0, 0));
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::string line;
    while (std::getline(input, line)) {
        char direction = line[0];
        int steps = std::stoi(line.substr(2));
        for (int i = 0; i < steps; i++) {
            if (direction == 'R') {
                knots[0].first += 1;
            } else if (direction == 'L') {
                knots[0].first -= 1;
            } else if (direction == 'U') {
                knots[0].second += 1;
            } else if (direction == 'D') {
                knots[0].second -= 1;
            } else {
                std::cerr << "Unexpected direction " << direction << std::endl;
                return;
            }

            for (int i = 1; i < 10; i++) {
                int dx = knots[i-1].first - knots[i].first;
                int dy = knots[i-1].second - knots[i].second;
                int d = std::max(abs(dx), abs(dy));

                if (d > 1) {
                    knots[i].first += sign(dx);
                    knots[i].second += sign(dy);
                }
            }

            visited.insert(knots[9]);
        }
    }

    std::cout << visited.size() << std::endl;
}
