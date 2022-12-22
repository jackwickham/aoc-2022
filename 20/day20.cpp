#include <iostream>
#include <fstream>
#include <list>
#include <optional>
#include <regex>
#include <string>
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

struct Entry {
    int num;
    bool moved;
};

std::list<Entry> parse(std::ifstream& input);

void part1(std::ifstream& input) {
    std::list<Entry> nums = parse(input);
    std::list<Entry>::iterator it = nums.begin();
    while (it != nums.end()) {
        Entry e = *it;
        if (e.moved) {
            it++;
            continue;
        }
        it = nums.erase(it);
        auto move_it = std::list<Entry>::iterator(it);
        if (e.num > 0) {
            if (move_it == nums.end()) {
                move_it = nums.begin();
            }
            for (int i = 0; i < e.num; i++) {
                ++move_it;
                if (move_it == nums.end()) {
                    move_it = nums.begin();
                }
            }
            nums.insert(move_it, Entry{
                .num = e.num,
                .moved = true,
            });
        } else if (e.num < 0) {
            if (move_it == nums.begin()) {
                move_it = nums.end();
            }
            for (int i = 0; i > e.num; i--) {
                --move_it;
                if (move_it == nums.begin()) {
                    move_it = nums.end();
                }
            }
            nums.insert(move_it, Entry{
                .num = e.num,
                .moved = true,
            });
        } else {
            nums.insert(move_it, Entry{
                .num = e.num,
                .moved = true,
            });
        }
    }
    
    std::list<Entry>::iterator read_it = nums.begin();
    while ((*read_it).num != 0) {
        ++read_it;
    }

    int result = 0;

    if (read_it == nums.end()) {
        read_it = nums.begin();
    }

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 1000; i++) {
            ++read_it;
            if (read_it == nums.end()) {
                read_it = nums.begin();
            }
        }
        result += (*read_it).num;
    }
    std::cout << result << std::endl;
}

std::list<Entry> parse(std::ifstream& input) {
    std::string line;
    std::list<Entry> result;
    int index = 0;
    while (std::getline(input, line)) {
        result.push_back(Entry{
            .num = std::stoi(line),
            .moved = false,
        });
    }
    return result;
}
