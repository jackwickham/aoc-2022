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

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

template<> struct std::hash<Point> {
    std::size_t operator()(Point const& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(p.x);
        std::size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};

std::unordered_set<Point> parse(std::ifstream& input);
std::unordered_set<Point> compute_move(std::unordered_set<Point>& elves, int move_num);

void part1(std::ifstream& input) {
    std::unordered_set<Point> elves = parse(input);

    for (int i = 0; i < 10; i++) {
        elves = compute_move(elves, i);
    }

    int min_x = 100;
    int min_y = 100;
    int max_x = 0;
    int max_y = 0;

    for (Point elf : elves) {
        min_x = std::min(min_x, elf.x);
        min_y = std::min(min_y, elf.y);
        max_x = std::max(max_x, elf.x);
        max_y = std::max(max_y, elf.y);
    }

    int empty_space = 0;
    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            if (!elves.contains(Point(x, y))) {
                ++empty_space;
            }
        }
    }
    std::cout << empty_space << std::endl;
}

void part2(std::ifstream& input) {
    std::unordered_set<Point> elves = parse(input);

    for (int i = 0;; i++) {
        std::unordered_set<Point> updated = compute_move(elves, i);
        if (updated == elves) {
            std::cout << i + 1 << std::endl;
            break;
        }
        elves = updated;
    }
}

std::unordered_set<Point> compute_move(std::unordered_set<Point>& elves, int move_num) {
    std::unordered_map<Point, Point> moves; // to, from
    std::unordered_set<Point> proposals;
    for (Point elf : elves) {
        bool could_move_north = !elves.contains(Point(elf.x - 1, elf.y - 1)) && !elves.contains(Point(elf.x, elf.y - 1)) && !elves.contains(Point(elf.x + 1, elf.y - 1));
        bool could_move_south = !elves.contains(Point(elf.x - 1, elf.y + 1)) && !elves.contains(Point(elf.x, elf.y + 1)) && !elves.contains(Point(elf.x + 1, elf.y + 1));
        bool could_move_west = !elves.contains(Point(elf.x - 1, elf.y - 1)) && !elves.contains(Point(elf.x - 1, elf.y)) && !elves.contains(Point(elf.x - 1, elf.y + 1));
        bool could_move_east = !elves.contains(Point(elf.x + 1, elf.y - 1)) && !elves.contains(Point(elf.x + 1, elf.y)) && !elves.contains(Point(elf.x + 1, elf.y + 1));
        Point destination = elf;
        if (!could_move_north || !could_move_south || !could_move_west || !could_move_east) {
            for (int i = move_num; i < move_num + 4; i++) {
                if (i % 4 == 0 && could_move_north) {
                    destination = Point(elf.x, elf.y - 1);
                    break;
                } else if (i % 4 == 1 && could_move_south) {
                    destination = Point(elf.x, elf.y + 1);
                    break;
                } else if (i % 4 == 2 && could_move_west) {
                    destination = Point(elf.x - 1, elf.y);
                    break;
                } else if (i % 4 == 3 && could_move_east) {
                    destination = Point(elf.x + 1, elf.y);
                    break;
                }
            }
        }
        if (proposals.contains(destination)) {
            // Multiple proposals for the same destination - nobody gets it
            moves.insert({elf, elf});
            auto conflicting_elf_it = moves.find(destination);
            if (conflicting_elf_it != moves.end() && conflicting_elf_it->first == destination) {
                Point conflicting_elf = conflicting_elf_it->second;
                moves.erase(conflicting_elf_it);
                moves.insert({conflicting_elf, conflicting_elf});
            }
        } else {
            proposals.insert(destination);
            moves.insert({destination, elf});
        }
    }
    std::unordered_set<Point> result;
    for (auto entry : moves) {
        result.insert(entry.first);
    }
    return result;
}

std::unordered_set<Point> parse(std::ifstream& input) {
    std::unordered_set<Point> elves;
    std::string line;
    int y = 0;
    while (std::getline(input, line)) {
        for (int x = 0; x < line.size(); x++) {
            if (line[x] == '#') {
                elves.insert(Point(x, y));
            }
        }
        y++;
    }
    return elves;
}
