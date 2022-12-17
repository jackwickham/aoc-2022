#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>
#include "../shared/base.hpp"

struct Monkey {
    std::vector<long> items;
    std::function<long (long)> op;
    int modulus;
    int monkeyIfTrue;
    int monkeyIfFalse;
    long inspections;
};

std::vector<Monkey> parse_arrangement(std::ifstream& input);

void part1(std::ifstream& input) {
    std::vector<Monkey> monkeys = parse_arrangement(input);

    for (int i = 0; i < 20; i++) {
        for (int m = 0; m < monkeys.size(); m++) {
            for (int item : monkeys[m].items) {
                item = monkeys[m].op(item) / 3;
                if (item % monkeys[m].modulus == 0) {
                    monkeys[monkeys[m].monkeyIfTrue].items.push_back(item);
                } else {
                    monkeys[monkeys[m].monkeyIfFalse].items.push_back(item);
                }
                monkeys[m].inspections++;
            }
            monkeys[m].items.clear();
        }
    }

    int highest = 0, second = 0;
    for (Monkey monkey : monkeys) {
        if (monkey.inspections > highest) {
            second = highest;
            highest = monkey.inspections;
        } else if (monkey.inspections > second) {
            second = monkey.inspections;
        }
    }
    std::cout << highest * second << " (" << highest << ", " << second << ")" << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<Monkey> monkeys = parse_arrangement(input);

    long denominator = 1;
    for (Monkey monkey : monkeys) {
        denominator *= monkey.modulus;
    }

    for (int i = 0; i < 10000; i++) {
        for (int m = 0; m < monkeys.size(); m++) {
            for (long item : monkeys[m].items) {
                long new_item = monkeys[m].op(item) % denominator;
                if (new_item % monkeys[m].modulus == 0) {
                    monkeys[monkeys[m].monkeyIfTrue].items.push_back(new_item);
                } else {
                    monkeys[monkeys[m].monkeyIfFalse].items.push_back(new_item);
                }
                monkeys[m].inspections++;
            }
            monkeys[m].items.clear();
        }
    }

    long highest = 0, second = 0;
    for (Monkey monkey : monkeys) {
        std::cout << monkey.inspections << std::endl;
        if (monkey.inspections > highest) {
            second = highest;
            highest = monkey.inspections;
        } else if (monkey.inspections > second) {
            second = monkey.inspections;
        }
    }
    std::cout << highest * second << " (" << highest << ", " << second << ")" << std::endl;
}

std::vector<Monkey> parse_arrangement(std::ifstream& input) {
    std::vector<Monkey> monkeys;
    std::regex op_re ("\\s*Operation: new = old (.) (.*)$");
    std::regex throw_re (".*throw to monkey (\\d)$");
    
    while (true) {
        std::string line;

        std::getline(input, line); // Ignore monkey number

        std::vector<long> these_items;
        std::getline(input, line);
        line = line.substr(sizeof("  Starting items:"));
        for (int i = 0; i < line.length(); i += 4) {
            these_items.push_back(std::stol(line.substr(i, 2)));
        }

        std::getline(input, line);
        std::smatch op_matches;
        std::regex_match(line, op_matches, op_re);
        std::function<long (long)> op;
        if (op_matches[2] == "old") {
            if (op_matches[1] == "*") {
                op = [](long old) { return old * old; };
            } else {
                op = [](long old) { return old + old; };
            }
        } else {
            int other = std::stoi(op_matches[2]);
            if (op_matches[1] == "*") {
                op = [=](long old) { return old * other; };
            } else {
                op = [=](long old) { return old + other; };
            }
        }

        std::getline(input, line);
        line = line.substr(sizeof("  Test: divisible by"));
        int modulus = std::stoi(line);
        
        std::getline(input, line);
        std::smatch true_matches;
        std::regex_match(line, true_matches, throw_re);
        int monkey_true = std::stoi(true_matches[1]);

        std::getline(input, line);
        std::smatch false_matches;
        std::regex_match(line, false_matches, throw_re);
        int monkey_false = std::stoi(false_matches[1]);

        monkeys.push_back(Monkey{
            .items = these_items,
            .op = op,
            .modulus = modulus,
            .monkeyIfTrue = monkey_true,
            .monkeyIfFalse = monkey_false,
            .inspections = 0,
        });

        if (!std::getline(input, line)) {
            break;
        }
    }

    return monkeys;
}
