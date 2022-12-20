#include <array>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

class Piece;

// |.......| <= height
// |..###..|
// |..###..| <= 0
// +-------+
//  ^     ^
//  0     6
class Chamber {
public:
    std::vector<std::array<bool, 7>> spaces;
    int height;

    Chamber() : spaces(std::vector<std::array<bool, 7>>()), height(0) {}

    bool is_filled(int y, int x) {
        if (x < 0 || x > 6 || y < 0) {
            return true;
        }
        if (y >= height) {
            return false;
        }
        return spaces[y][x];
    }

    void place(Piece& piece);

private:
    void ensure_height(int min_height) {
        for (int i = spaces.size(); i < min_height; i++) {
            std::array<bool, 7> row = {false};
            spaces.push_back(row);
        }
    }
};

// .#.
// ###
// .#. <= bottom
// ^
// left
class Piece {
public:
    int type;
    int bottom;
    int left;

    void maybe_move_left(Chamber& chamber) {
        if (left == 0) {
            return;
        }

        if (bottom < chamber.height) {
            switch (type) {
            case 0: // square
                if (chamber.is_filled(bottom, left - 1) || chamber.is_filled(bottom + 1, left - 1)) {
                    return;
                }
                break;
            case 1: // horizontal line
                if (chamber.is_filled(bottom, left - 1)) {
                    return;
                }
                break;
            case 2: // plus
                if (chamber.is_filled(bottom, left) || chamber.is_filled(bottom + 1, left - 1) || chamber.is_filled(bottom + 2, left)) {
                    return;
                }
                break;
            case 3: // right angle
                if (chamber.is_filled(bottom, left - 1) || chamber.is_filled(bottom + 1, left + 1) || chamber.is_filled(bottom + 2, left + 1)) {
                    return;
                }
                break;
            case 4: // vertical line
                if (chamber.is_filled(bottom, left - 1) || chamber.is_filled(bottom + 1, left - 1) || chamber.is_filled(bottom + 2, left - 1) || chamber.is_filled(bottom + 3, left - 1)) {
                    return;
                }
                break;
            }
        }
        left -= 1;
    }

    void maybe_move_right(Chamber& chamber) {
        switch (type) {
        case 0: // square
            if (chamber.is_filled(bottom, left + 2) || chamber.is_filled(bottom + 1, left + 2)) {
                return;
            }
            break;
        case 1: // horizontal line
            if (chamber.is_filled(bottom, left + 4)) {
                return;
            }
            break;
        case 2: // plus
            if (chamber.is_filled(bottom, left + 2) || chamber.is_filled(bottom + 1, left + 3) || chamber.is_filled(bottom + 2, left + 2)) {
                return;
            }
            break;
        case 3: // right angle
            if (chamber.is_filled(bottom, left + 3) || chamber.is_filled(bottom + 1, left + 3) || chamber.is_filled(bottom + 2, left + 3)) {
                return;
            }
            break;
        case 4: // vertical line
            if (chamber.is_filled(bottom, left + 1) || chamber.is_filled(bottom + 1, left + 1) || chamber.is_filled(bottom + 2, left + 1) || chamber.is_filled(bottom + 3, left + 1)) {
                return;
            }
            break;
        }
        left += 1;
    }

    bool maybe_move_down(Chamber& chamber) {
        if (bottom <= chamber.height) {
            switch (type) {
            case 0: // square
                if (chamber.is_filled(bottom - 1, left) || chamber.is_filled(bottom - 1, left + 1)) {
                    return false;
                }
                break;
            case 1: // horizontal line
                if (chamber.is_filled(bottom - 1, left) || chamber.is_filled(bottom - 1, left + 1) || chamber.is_filled(bottom - 1, left + 2) || chamber.is_filled(bottom - 1, left + 3)) {
                    return false;
                }
                break;
            case 2: // plus
                if (chamber.is_filled(bottom, left) || chamber.is_filled(bottom - 1, left + 1) || chamber.is_filled(bottom, left + 2)) {
                    return false;
                }
                break;
            case 3: // right angle
                if (chamber.is_filled(bottom - 1, left) || chamber.is_filled(bottom - 1, left + 1) || chamber.is_filled(bottom - 1, left + 2)) {
                    return false;
                }
                break;
            case 4: // vertical line
                if (chamber.is_filled(bottom - 1, left)) {
                    return false;
                }
                break;
            }
        }
        bottom -= 1;
        return true;
    }
};


void Chamber::place(Piece& piece) {
    ensure_height(piece.bottom + 5);
    int top;
    switch (piece.type) {
    case 0: // square
        spaces[piece.bottom][piece.left] = true;
        spaces[piece.bottom][piece.left + 1] = true;
        spaces[piece.bottom + 1][piece.left] = true;
        spaces[piece.bottom + 1][piece.left + 1] = true;
        top = piece.bottom + 1;
        break;
    case 1: // horizontal line
        spaces[piece.bottom][piece.left] = true;
        spaces[piece.bottom][piece.left + 1] = true;
        spaces[piece.bottom][piece.left + 2] = true;
        spaces[piece.bottom][piece.left + 3] = true;
        top = piece.bottom;
        break;
    case 2: // plus
        spaces[piece.bottom][piece.left + 1] = true;
        spaces[piece.bottom + 1][piece.left] = true;
        spaces[piece.bottom + 1][piece.left + 1] = true;
        spaces[piece.bottom + 1][piece.left + 2] = true;
        spaces[piece.bottom + 2][piece.left + 1] = true;
        top = piece.bottom + 2;
        break;
    case 3: // right angle
        spaces[piece.bottom][piece.left] = true;
        spaces[piece.bottom][piece.left + 1] = true;
        spaces[piece.bottom][piece.left + 2] = true;
        spaces[piece.bottom + 1][piece.left + 2] = true;
        spaces[piece.bottom + 2][piece.left + 2] = true;
        top = piece.bottom + 2;
        break;
    case 4: // vertical line
        spaces[piece.bottom][piece.left] = true;
        spaces[piece.bottom + 1][piece.left] = true;
        spaces[piece.bottom + 2][piece.left] = true;
        spaces[piece.bottom + 3][piece.left] = true;
        top = piece.bottom + 3;
        break;
    }
    height = std::max(height, top + 1);
}

std::vector<bool> parse(std::ifstream& input);

void part1(std::ifstream& input) {
    std::vector<bool> gusts = parse(input);
    auto gusts_iter = gusts.begin();

    Chamber chamber;
    for (int i = 1; i <= 2022; i++) {
        Piece piece = Piece{
            .type = i % 5,
            .bottom = chamber.height + 3,
            .left = 2,
        };

        do {
            if (*gusts_iter) {
                piece.maybe_move_right(chamber);
            } else {
                piece.maybe_move_left(chamber);
            }

            gusts_iter++;
            if (gusts_iter == gusts.end()) {
                gusts_iter = gusts.begin();
            }
        } while (piece.maybe_move_down(chamber));

        chamber.place(piece);
    }

    std::cout << chamber.height << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<bool> gusts = parse(input);
    auto gusts_iter = gusts.begin();

    long bonus_height;

    Chamber chamber;
    for (long i = 1; i <= 1000000000000; i++) {
        Piece piece = Piece{
            .type = (int)(i % 5),
            .bottom = chamber.height + 3,
            .left = 2,
        };

        do {
            if (*gusts_iter) {
                piece.maybe_move_right(chamber);
            } else {
                piece.maybe_move_left(chamber);
            }

            gusts_iter++;
            if (gusts_iter == gusts.end()) {
                std::cout << "Gusts wrapping at i=" << i << ", type=" << piece.type << ", height=" << chamber.height << ", dh=" << piece.bottom - chamber.height << ", top=";
                for (int i = 0; i < 7; i++) {
                    std::cout << (chamber.spaces[chamber.height - 1][i] ? '#' : '.');
                }
                std::cout << std::endl;

                if (i < 100000) {
                    long multiplier = (1000000000000 / (5231 - 3486)) - 1;
                    i += multiplier * (5231 - 3486);
                    bonus_height = multiplier * (8275 - 5522);
                }
                
                gusts_iter = gusts.begin();
            }
        } while (piece.maybe_move_down(chamber));

        chamber.place(piece);
    }

    std::cout << chamber.height + bonus_height << std::endl;
}

std::vector<bool> parse(std::ifstream& input) {
    std::string line;
    std::getline(input, line);
    std::vector<bool> result;
    for (char c : line) {
        result.push_back(c == '>');
    }
    return result;
}
