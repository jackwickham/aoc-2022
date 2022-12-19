#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <stack>
#include <optional>
#include <unordered_set>
#include <vector>
#include "../shared/base.hpp"

class Point {
public:
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
struct point_hash {
    inline std::size_t operator()(const Point& point) const {
        return point.x * 63 + point.y;
    }
};

struct Setup {
    std::unordered_set<Point, point_hash> points;
    int max_y;
};

Setup parse(std::ifstream& input);
std::optional<Point> compute_drop(Point point, std::unordered_set<Point, point_hash>& filled);

void part1(std::ifstream& input) {
    Setup setup = parse(input);
    std::unordered_set<Point, point_hash> filled = setup.points;
    
    std::stack<Point> path;
    path.push(Point(500, 0));
    for (int i = 0; ; i++) {
        Point prev = path.top();
        path.pop();
        for (std::optional<Point> next = compute_drop(prev, filled); next.has_value(); next = compute_drop(prev, filled)) {
            path.push(prev);
            prev = *next;

            if (prev.y > setup.max_y) {
                std::cout << i << std::endl;
                return;
            }
        }
        filled.insert(prev);
    }
}

void part2(std::ifstream& input) {
    Setup setup = parse(input);
    std::unordered_set<Point, point_hash> filled = setup.points;
    for (int x = -10; x < 1010; x++) {
        filled.insert(Point(x, setup.max_y + 2));
    }
    
    std::stack<Point> path;
    path.push(Point(500, 0));
    for (int i = 1; ; i++) {
        Point prev = path.top();
        path.pop();
        for (std::optional<Point> next = compute_drop(prev, filled); next.has_value(); next = compute_drop(prev, filled)) {
            path.push(prev);
            prev = *next;
        }
        if (prev == Point(500, 0)) {
            std::cout << i << std::endl;
            return;
        }
        filled.insert(prev);
    }
}

Setup parse(std::ifstream& input) {
    std::unordered_set<Point, point_hash> filled;
    int max_y = 0;

    std::regex re ("(\\d+),(\\d+)( -> )?");
    std::string line;
    while (std::getline(input, line)) {
        std::optional<Point> prev_point;
        for (std::sregex_iterator it (line.begin(), line.end(), re); it != std::sregex_iterator(); it++) {
            std::smatch match = *it;
            Point point (std::stoi(match[1]), std::stoi(match[2]));
            if (prev_point.has_value()) {
                if (prev_point->x == point.x) {
                    int y1 = std::min(prev_point->y, point.y);
                    int y2 = std::max(prev_point->y, point.y);
                    for (int y = y1; y <= y2; y++) {
                        filled.insert(Point(point.x, y));
                    }
                } else {
                    int x1 = std::min(prev_point->x, point.x);
                    int x2 = std::max(prev_point->x, point.x);
                    for (int x = x1; x <= x2; x++) {
                        filled.insert(Point(x, point.y));
                    }
                }
            }
            if (point.y > max_y) {
                max_y = point.y;
            }
            prev_point = point;
        }
    }
    return Setup{
        .points = filled,
        .max_y = max_y,
    };
}

std::optional<Point> compute_drop(Point point, std::unordered_set<Point, point_hash>& filled) {
    for (int d : {0, -1, 1}) {
        Point p = Point(point.x + d, point.y + 1);
        if (!filled.contains(p)) {
            return p;
        }
    }
    return std::optional<Point>();
}
