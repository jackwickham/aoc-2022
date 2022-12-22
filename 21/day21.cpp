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

class DescendantMonkey;

class Monkey {
public:
    std::string name;
    std::vector<DescendantMonkey*> descendants;
    bool is_root;

    Monkey(std::string name) : name(name), descendants({}), is_root(false) {}

    virtual std::optional<long> value() = 0;

    virtual std::optional<std::pair<Monkey*, long>> invert(long output) = 0;
};

class ConstantMonkey : public Monkey {
public:
    long val;

    ConstantMonkey(std::string name, long value) : Monkey(name), val(value) {}

    virtual std::optional<long> value() override {
        return val;
    }

    virtual std::optional<std::pair<Monkey*, long>> invert(long output) override {
        return std::optional<std::pair<Monkey*, long>>();
    }
};

class DescendantMonkey : public Monkey {
public:
    Monkey* left;
    Monkey* right;

    DescendantMonkey(std::string name) : Monkey(name), left(nullptr), right(nullptr) {}
};

class AlgebraMonkey : public DescendantMonkey {
public:
    std::optional<long> val;
    std::string op;

    AlgebraMonkey(std::string name, std::string op) : DescendantMonkey(name), op(op) {}

    virtual std::optional<long> value() override {
        if (val.has_value()) {
            return val;
        }
        std::optional<long> l = left->value();
        std::optional<long> r = right->value();
        if (l.has_value() && r.has_value()) {
            std::function<long (long, long)> fn;
            if (op == "+") {
                fn = std::plus<long>();
            } else if (op == "-") {
                fn = std::minus<long>();
            } else if (op == "*") {
                fn = std::multiplies<long>();
            } else if (op == "/") {
                fn = std::divides<long>();
            } else {
                std::cerr << "Unexpected operator " << op << std::endl;
                exit(1);
            }
            val = fn(*l, *r);
            return val;
        }
        return std::optional<long>();
    }

    virtual std::optional<std::pair<Monkey*, long>> invert(long output) override {
        std::optional<long> l = left->value();
        std::optional<long> r = right->value();
        if (l.has_value() == r.has_value()) {
            return std::optional<std::pair<Monkey*, long>>();
        }

        long result;
        if (op == "+") {
            result = output - l.value_or(*r);
        } else if (op == "-") {
            if (l.has_value()) {
                result = *l - output;
            } else {
                result = *r + output;
            }
        } else if (op == "*") {
            result = output / l.value_or(*r);
        } else if (op == "/") {
            if (l.has_value()) {
                result = *l / output;
            } else {
                result = *r * output;
            }
        } else {
            std::cerr << "Unexpected operator " << op << std::endl;
            exit(1);
        }
        if (l.has_value()) {
            return std::pair<Monkey*, long>(right, result);
        } else {
            return std::pair<Monkey*, long>(left, result);
        }
    }
};

class RootMonkey : public DescendantMonkey {
public:
    RootMonkey(std::string name) : DescendantMonkey(name) {
        is_root = true;
    }

    virtual std::optional<long> value() override {
        std::optional<long> l = left->value();
        if (l.has_value()) {
            return l;
        }
        return right->value();
    }

    virtual std::optional<std::pair<Monkey*, long>> invert(long _output) override {
        std::optional<long> l = left->value();
        std::optional<long> r = right->value();
        if (l.has_value() == r.has_value()) {
            return std::optional<std::pair<Monkey*, long>>();
        }
        if (l.has_value()) {
            return std::pair(right, *l);
        } else {
            return std::pair(left, *r);
        }
    }
};

class HumanMonkey : public Monkey {
public:
    HumanMonkey(std::string name) : Monkey(name) {}

    virtual std::optional<long> value() override {
        return std::optional<long>();
    }

    virtual std::optional<std::pair<Monkey*, long>> invert(long output) override {
        return std::pair(nullptr, output);
    }
};

std::vector<ConstantMonkey*> parse(std::ifstream& input);
std::vector<ConstantMonkey*> parse2(std::ifstream& input);

void part1(std::ifstream& input) {
    std::vector<ConstantMonkey*> ready_monkeys = parse(input);
    std::queue<Monkey*> ready_queue;
    for (ConstantMonkey* monkey : ready_monkeys) {
        ready_queue.push(monkey);
    }

    while (!ready_queue.empty()) {
        Monkey* monkey = ready_queue.front();
        ready_queue.pop();

        if (monkey->is_root) {
            std::cout << *monkey->value() << std::endl;
            return;
        }

        for (Monkey* descendant : monkey->descendants) {
            if (descendant->value().has_value()) {
                ready_queue.push(descendant);
            }
        }
    }
}

void part2(std::ifstream& input) {
    std::vector<ConstantMonkey*> ready_monkeys = parse2(input);
    std::queue<Monkey*> ready_queue;
    for (ConstantMonkey* monkey : ready_monkeys) {
        ready_queue.push(monkey);
    }

    Monkey* root = nullptr;

    while (!ready_queue.empty()) {
        Monkey* monkey = ready_queue.front();
        ready_queue.pop();

        if (monkey->is_root) {
            root = monkey;
        }

        for (Monkey* descendant : monkey->descendants) {
            if (descendant->value().has_value()) {
                ready_queue.push(descendant);
            }
        }
    }

    if (root == nullptr) {
        std::cerr << "Root was not ready" << std::endl;
        return;
    }

    long output = 0;
    while (root != nullptr) {
        std::optional<std::pair<Monkey*, long>> result = root->invert(output);
        if (!result.has_value()) {
            std::cerr << "Failed to invert" << std::endl;
            return;
        }
        output = result->second;
        root = result->first;
    }

    std::cout << output << std::endl;
}

std::vector<ConstantMonkey*> parse(std::ifstream& input) {
    std::unordered_map<std::string, Monkey*> monkeys;
    std::unordered_multimap<std::string, std::pair<AlgebraMonkey*, bool>> monkeys_needing_inputs;
    std::vector<ConstantMonkey*> constant_monkeys;

    std::string line;
    std::regex re ("([a-z]*): (?:(-?\\d+)|([a-z]*) ([+\\-*/]) ([a-z]*))");
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex_match(line, match, re);

        std::string name = match[1];
        Monkey* monkey;
        if (match[2].matched) {
            ConstantMonkey* cm = new ConstantMonkey(name, std::stol(match[2]));
            constant_monkeys.push_back(cm);
            monkey = cm;
        } else {
            AlgebraMonkey* am = new AlgebraMonkey(name, match[4]);
            if (monkeys.contains(match[3])) {
                am->left = monkeys[match[3]];
                monkeys[match[3]]->descendants.push_back(am);
            } else {
                monkeys_needing_inputs.insert({match[3], {am, false}});
            }
            if (monkeys.contains(match[5])) {
                am->right = monkeys[match[5]];
                monkeys[match[5]]->descendants.push_back(am);
            } else {
                monkeys_needing_inputs.insert({match[5], {am, true}});
            }
            monkey = am;
        }
        monkeys[name] = monkey;
        auto it = monkeys_needing_inputs.find(name);
        while (it != monkeys_needing_inputs.end() && it->first == name) {
            AlgebraMonkey* descendant = it->second.first;
            if (it->second.second) {
                descendant->right = monkey;
            } else {
                descendant->left = monkey;
            }
            monkey->descendants.push_back(descendant);
            it = monkeys_needing_inputs.erase(it);
        }

        if (name == "root") {
            monkey->is_root = true;
        }
    }
    return constant_monkeys;
}

std::vector<ConstantMonkey*> parse2(std::ifstream& input) {
    std::unordered_map<std::string, Monkey*> monkeys;
    std::unordered_multimap<std::string, std::pair<DescendantMonkey*, bool>> monkeys_needing_inputs;
    std::vector<ConstantMonkey*> constant_monkeys;

    std::string line;
    std::regex re ("([a-z]*): (?:(-?\\d+)|([a-z]*) ([+\\-*/]) ([a-z]*))");
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex_match(line, match, re);

        std::string name = match[1];
        Monkey* monkey;
        if (name == "humn") {
            monkey = new HumanMonkey(name);
        } else if (match[2].matched) {
            ConstantMonkey* cm = new ConstantMonkey(name, std::stol(match[2]));
            constant_monkeys.push_back(cm);
            monkey = cm;
        } else {
            DescendantMonkey* am;
            if (name == "root") {
                am = new RootMonkey(name);
            } else {
                am = new AlgebraMonkey(name, match[4]);
            }
            if (monkeys.contains(match[3])) {
                am->left = monkeys[match[3]];
                monkeys[match[3]]->descendants.push_back(am);
            } else {
                monkeys_needing_inputs.insert({match[3], {am, false}});
            }
            if (monkeys.contains(match[5])) {
                am->right = monkeys[match[5]];
                monkeys[match[5]]->descendants.push_back(am);
            } else {
                monkeys_needing_inputs.insert({match[5], {am, true}});
            }
            monkey = am;
        }
        monkeys[name] = monkey;
        auto it = monkeys_needing_inputs.find(name);
        while (it != monkeys_needing_inputs.end() && it->first == name) {
            DescendantMonkey* descendant = it->second.first;
            if (it->second.second) {
                descendant->right = monkey;
            } else {
                descendant->left = monkey;
            }
            monkey->descendants.push_back(descendant);
            it = monkeys_needing_inputs.erase(it);
        }
    }
    return constant_monkeys;
}
