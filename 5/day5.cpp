#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>
#include "../shared/base.hpp"

struct Move {
    int count;
    int src;
    int dest;
};

std::vector<std::vector<char>> parse_arrangement(std::ifstream& input);
std::vector<struct Move> parse_moves(std::ifstream& input);

void part1(std::ifstream& input) {
    std::vector<std::vector<char>> arrangement = parse_arrangement(input);
    std::vector<Move> moves = parse_moves(input);

    for (Move move : moves) {
        for (int i = 0; i < move.count; i++) {
            std::cout << ".";
            if (arrangement[move.src].size() == 0) {
                std::cerr << "source stack " << move.src << " is empty";
                return;
            }
            arrangement[move.dest].push_back(arrangement[move.src].back());
            arrangement[move.src].pop_back();
        }
        std::cout << std::endl;
    }

    for (auto stack : arrangement) {
        std::cout << stack.back();
    }
    std::cout << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<std::vector<char>> arrangement = parse_arrangement(input);
    std::vector<Move> moves = parse_moves(input);

    for (Move move : moves) {
        for (int i = move.count; i > 0; i--) {
            arrangement[move.dest].push_back(arrangement[move.src][arrangement[move.src].size() - i]);
        }
        for (int i = 0; i < move.count; i++) {
            arrangement[move.src].pop_back();
        }
    }

    for (auto stack : arrangement) {
        std::cout << stack.back();
    }
    std::cout << std::endl;
}

std::vector<std::vector<char>> parse_arrangement(std::ifstream& input) {
    std::vector<std::vector<char>> columns = std::vector<std::vector<char>>(9, std::vector<char>());
    
    while (true) {
        std::string line;
        std::getline(input, line);

        if (line == "") {
            break;
        }

        auto col_iter = columns.begin();
        std::string::iterator iter = line.begin();
        while (true) {
            if (col_iter == columns.end()) {
                columns.insert(col_iter, std::vector<char>());
            }

            char start = *(iter++);
            if (start == ' ') {
                iter += 2;
            } else if (start == '[') {
                char c = *(iter++);
                (*col_iter).insert((*col_iter).begin(), c);
                if (*(iter++) != ']') {
                    std::cerr << "Unexpected character " << *(iter - 1) << " (expected ])" << std::endl;
                    return columns;
                }
            } else {
                std::cerr << "Unexpected character " << *(iter - 1) << " (expected [ or space)" << std::endl;
                return columns;
            }

            if (iter == line.end()) {
                break;
            }

            if (*(iter++) != ' ') {
                std::cerr << "Unexpected character " << *(iter-1) << " (expected space)" << std::endl;
                return columns;
            }

            col_iter++;
        }
    }

    return columns;
}

std::vector<struct Move> parse_moves(std::ifstream& input) {
    std::regex re ("move (\\d+) from (\\d+) to (\\d+)$");

    std::vector<struct Move> results;

    std::string line;
    while (std::getline(input, line)) {
        std::smatch matches;
        std::regex_match(line, matches, re);
        std::string s = matches[1];

        if (matches.size() != 4) {
            std::cerr << "Unexpected number of matches " << matches.size() << " from line: " << line;
            continue;
        }

        results.push_back(Move{
            .count = std::stoi(matches[1]),
            .src = std::stoi(matches[2]) - 1,
            .dest = std::stoi(matches[3]) - 1,
        });
    }

    return results;
}
