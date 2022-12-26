#include <iostream>
#include <fstream>
#include <optional>
#include <regex>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

long from_snafu(std::string snafu);
std::string to_snafu(long i);

void part1(std::ifstream& input) {
    long total = 0;
    std::string line;
    while (std::getline(input, line)) {
        total += from_snafu(line);
    }
    std::cout << to_snafu(total) << " (" << total << ")" << std::endl;
}

void part2(std::ifstream& input) {
}

long from_snafu(std::string snafu) {
    long result = 0;
    long multiplier = 1;
    auto it = snafu.end();
    do {
        --it;
        if (*it == '=') {
            result -= 2 * multiplier;
        } else if (*it == '-') {
            result -= 1 * multiplier;
        } else {
            result += (*it - '0') * multiplier;
        }
        multiplier *= 5;
    } while (it != snafu.begin());
    return result;
}

std::string to_snafu(long i) {
    std::string snafu;
    while (i > 0) {
        int carry = 0;
        switch (i % 5) {
        case 0:
            snafu.insert(0, "0");
            carry = 0;
            break;
        case 1:
            snafu.insert(0, "1");
            carry = 0;
            break;
        case 2:
            snafu.insert(0, "2");
            carry = 0;
            break;
        case 3:
            snafu.insert(0, "=");
            carry = 1;
            break;
        case 4:
            snafu.insert(0, "-");
            carry = 1;
            break;
        }
        i = i / 5 + carry;
    }
    return snafu;
}
