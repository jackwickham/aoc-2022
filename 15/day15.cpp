#include <iostream>
#include <fstream>
#include <optional>
#include <regex>
#include <string>
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

struct Sensor {
    int x;
    int y;
    int range;
    int beacon_x;
    int beacon_y;
};

struct Range {
    /// @brief Inclusive lower bound
    int lb;
    /// @brief Exclusive upper bound
    int ub;

    Range combine(Range other) {
        return Range{
            .lb = std::min(this->lb, other.lb),
            .ub = std::max(this->ub, other.ub),
        };
    }

    int size() {
        return ub - lb;
    }
};

struct lb_compare {
    bool operator() (const Range& left, const Range& right) const {
        return left.lb < right.lb;
    }
};

class RangeSet {
private:
    std::set<Range, lb_compare> ranges;

    void erase(Range range) {
        ranges.erase(range);
    }

public:
    void insert(Range range) {
        auto it = ranges.lower_bound(range);
        if (it != ranges.end() && it->lb <= range.ub) {
            this->erase(*it);
            insert(range.combine(*it));
        } else if (it != ranges.begin() && (--it)->ub >= range.lb) {
            this->erase(*it);
            insert(range.combine(*it));
        } else {
            ranges.insert(range);
        }
    }

    std::set<Range>::iterator begin() {
        return this->ranges.begin();
    }

    std::set<Range>::iterator end() {
        return this->ranges.end();
    }

    std::optional<int> first_difference(Range range) {
        auto it = ranges.upper_bound(range);
        if (it == ranges.begin()) {
            return range.lb;
        }
        it--;
        if (it->ub < range.ub) {
            return it->ub;
        } else {
            return std::optional<int>();
        }
    }
};

std::vector<Sensor> parse(std::ifstream& input);

void part1(std::ifstream& input) {
    std::vector<Sensor> sensors = parse(input);
    RangeSet ranges;
    int row = 2000000;
    for (Sensor sensor : sensors) {
        int x_delta = sensor.range - std::abs(sensor.y - row);
        if (x_delta < 0) {
            continue;
        }
        Range range = Range{
            .lb = sensor.x - x_delta,
            .ub = sensor.x + x_delta + 1,
        };
        
        ranges.insert(range);
    }

    int total = 0;
    for (Range range : ranges) {
        std::cout << range.lb << "->" << range.ub << " with size " << range.size() << std::endl;
        total += range.size();
    }

    std::unordered_set<int> beacon_x;
    for (Sensor sensor : sensors) {
        if (sensor.beacon_y == row) {
            beacon_x.insert(sensor.beacon_x);
        }
    }
    total -= beacon_x.size();

    std::cout << total << std::endl;
}
 
void part2(std::ifstream& input) {
    int upper_range = 4000000;
    std::vector<Sensor> sensors = parse(input);
    Range region = Range{
        .lb = 0,
        .ub = upper_range + 1,
    };
    for (int y = 0; y <= upper_range; y++) {
        if (y % 100000 == 0) {
            std::cout << "processing " << y << " (" << y * 100 / upper_range << "% complete)" << std::endl;
        }
        RangeSet ranges;
        for (Sensor sensor : sensors) {
            int x_delta = sensor.range - std::abs(sensor.y - y);
            if (x_delta < 0) {
                continue;
            }
            Range range = Range{
                .lb = sensor.x - x_delta,
                .ub = sensor.x + x_delta + 1,
            };
            
            ranges.insert(range);
        }

        std::optional<int> maybe_result = ranges.first_difference(region);
        if (maybe_result.has_value()) {
            std::cout << 4000000L * *maybe_result + y << std::endl;
            return;
        }
    }
}

std::vector<Sensor> parse(std::ifstream& input) {
    std::regex re ("Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)");
    std::string line;
    std::vector<Sensor> sensors;
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex_match(line, match, re);
        int x = std::stoi(match[1]);
        int y = std::stoi(match[2]);
        int beacon_x = std::stoi(match[3]);
        int beacon_y = std::stoi(match[4]);
        int range = std::abs(x - beacon_x) + std::abs(y - beacon_y);
        sensors.emplace_back(Sensor{
            .x = x,
            .y = y,
            .range = range,
            .beacon_x = beacon_x,
            .beacon_y = beacon_y,
        });
    }
    return sensors;
}
