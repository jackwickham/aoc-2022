#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../shared/base.hpp"

std::vector<std::vector<uint8_t>> parse(std::ifstream& input);
std::vector<std::vector<bool>> make_result_grid(std::vector<std::vector<uint8_t>>& grid);

void part1(std::ifstream& input) {
    std::vector<std::vector<uint8_t>> grid = parse(input);
    int visible = 0;
    for (int r = 0; r < grid.size(); r++) {
        for (int c = 0; c < grid[r].size(); c++) {
            bool visible_up = true;
            for (int y = 0; y < r; y++) {
                if (grid[y][c] >= grid[r][c]) {
                    visible_up = false;
                    break;
                }
            }
            bool visible_down = true;
            for (int y = r + 1; y < grid.size(); y++) {
                if (grid[y][c] >= grid[r][c]) {
                    visible_down = false;
                    break;
                }
            }

            bool visible_left = true;
            for (int x = 0; x < c; x++) {
                if (grid[r][x] >= grid[r][c]) {
                    visible_left = false;
                    break;
                }
            }
            bool visible_right = true;
            for (int x = c + 1; x < grid[r].size(); x++) {
                if (grid[r][x] >= grid[r][c]) {
                    visible_right = false;
                    break;
                }
            }

            if (visible_up || visible_down || visible_left || visible_right) {
                visible++;
            }
        }
    }

    std::cout << visible << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<std::vector<uint8_t>> grid = parse(input);
    long max_scenic_score = 0;
    for (int r = 1; r < grid.size() - 1; r++) {
        for (int c = 1; c < grid[r].size() - 1; c++) {
            long distance_up = 0;
            for (int y = r - 1; y >= 0; y--) {
                distance_up++;
                if (grid[y][c] >= grid[r][c]) {
                    break;
                }
            }
            long distance_down = 0;
            for (int y = r + 1; y < grid.size(); y++) {
                distance_down++;
                if (grid[y][c] >= grid[r][c]) {
                    break;
                }
            }

            long distance_left = 0;
            for (int x = c - 1; x >= 0; x--) {
                distance_left++;
                if (grid[r][x] >= grid[r][c]) {
                    break;
                }
            }
            long distance_right = 0;
            for (int x = c + 1; x < grid[r].size(); x++) {
                distance_right++;
                if (grid[r][x] >= grid[r][c]) {
                    break;
                }
            }

            long score = distance_up * distance_down * distance_left * distance_right;
            if (score > max_scenic_score) {
                max_scenic_score = score;
            }
        }
    }

    std::cout << max_scenic_score << std::endl;
}

std::vector<std::vector<uint8_t>> parse(std::ifstream& input) {
    std::vector<std::vector<uint8_t>> result;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<uint8_t> row;
        for (char c : line) {
            row.push_back(c - '0');
        }
        result.push_back(row);
    }
    return result;
}
