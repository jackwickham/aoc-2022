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
        // https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

enum Direction {
    UP, DOWN, LEFT, RIGHT,
};

struct Valley {
    Point start;
    Point end;
    std::unordered_multimap<Point, Direction> blizzards;
    int width;
    int height;

    void step() {
        std::unordered_multimap<Point, Direction> updated = std::unordered_multimap<Point, Direction>(blizzards.size());
        for (auto entry : blizzards) {
            Point next (entry.first);
            if (entry.second == Direction::UP) {
                next.y -= 1;
            } else if (entry.second == Direction::RIGHT) {
                next.x += 1;
            } else if (entry.second == Direction::DOWN) {
                next.y += 1;
            } else {
                next.x -= 1;
            }
            next = Point((next.x + width) % width, (next.y + height) % height);
            updated.insert({next, entry.second});
        }
        blizzards = updated;
    }
};

Valley parse(std::ifstream& input);

void update_possible_locations(Point current, std::unordered_set<Point>& possible_locations, Valley& valley) {
    for (Point d : {Point(0, -1), Point(-1, 0), Point(0, 0), Point(1, 0), Point(0, 1)}) {
        Point p = Point(current.x + d.x, current.y + d.y);
        if ((p.x >= 0 && p.y >= 0 && p.x < valley.width && p.y < valley.height && !valley.blizzards.contains(p)) || p == valley.start || p == valley.end) {
            possible_locations.insert(p);
        }
    }
}

void part1(std::ifstream& input) {
    Valley valley = parse(input);
    std::unordered_set<Point> possible_locations = {valley.start};
    for (int i = 1; ; i++) {
        valley.step();
        std::unordered_set<Point> updated_locations (possible_locations.size() * 3 / 2);
        for (Point location : possible_locations) {
            if (Point(location.x, location.y + 1) == valley.end) {
                std::cout << i << std::endl;
                return;
            }
            update_possible_locations(location, updated_locations, valley);
        }
        possible_locations = updated_locations;
    }
}

void part2(std::ifstream& input) {
    Valley valley = parse(input);
    std::unordered_set<Point> possible_locations = {valley.start};
    int i = 0;
    while (!possible_locations.contains(valley.end)) {
        ++i;
        valley.step();
        std::unordered_set<Point> updated_locations (possible_locations.size() * 3 / 2);
        for (Point location : possible_locations) {
            update_possible_locations(location, updated_locations, valley);
        }
        possible_locations = updated_locations;
    }

    possible_locations = {valley.end};
    while (!possible_locations.contains(valley.start)) {
        ++i;
        valley.step();
        std::unordered_set<Point> updated_locations (possible_locations.size() * 3 / 2);
        for (Point location : possible_locations) {
            update_possible_locations(location, updated_locations, valley);
        }
        possible_locations = updated_locations;
    }

    possible_locations = {valley.start};
    while (!possible_locations.contains(valley.end)) {
        ++i;
        valley.step();
        std::unordered_set<Point> updated_locations (possible_locations.size() * 3 / 2);
        for (Point location : possible_locations) {
            update_possible_locations(location, updated_locations, valley);
        }
        possible_locations = updated_locations;
    }

    std::cout << i << std::endl;
}

Valley parse(std::ifstream& input) {
    std::string line;
    std::getline(input, line);
    Valley valley = {
        .start = Point(line.find('.') - 1, -1),
        .end = Point(0, 0),
        .blizzards = {},
        .width = static_cast<int>(line.size()) - 2,
        .height = 0,
    };
    int y = 0;
    while (std::getline(input, line)) {
        if (line[1] == '#') {
            valley.end = Point(line.find('.') - 1, y);
            valley.height = y;
        } else {
            for (int i = 1; i < line.size() - 1; i++) {
                if (line[i] == '^') {
                    valley.blizzards.insert({Point(i-1, y), Direction::UP});
                } else if (line[i] == '>') {
                    valley.blizzards.insert({Point(i-1, y), Direction::RIGHT});
                } else if (line[i] == 'v') {
                    valley.blizzards.insert({Point(i-1, y), Direction::DOWN});
                } else if (line[i] == '<') {
                    valley.blizzards.insert({Point(i-1, y), Direction::LEFT});
                }
            }
        }
        y++;
    }
    return valley;
}
