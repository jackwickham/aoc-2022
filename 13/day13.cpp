#include <algorithm>
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>
#include "../shared/base.hpp"

class ListVal;

class AbstractVal {
public:
    virtual bool is_int() = 0;

    virtual void print_string() = 0;
};

class ListVal : public AbstractVal {
public:
    std::vector<AbstractVal*> val;

    ListVal(std::vector<AbstractVal*> list) : val(list) {}

    virtual bool is_int() {
        return false;
    }

    virtual void print_string() {
        std::cout << "[";
        for (AbstractVal* v : val) {
            v->print_string();
            std::cout << ",";
        }
        std::cout << "]";
    }
};

class IntVal : public AbstractVal {
public:
    int val;

    IntVal(int i) : val(i) {}

    virtual bool is_int() {
        return true;
    }

    virtual void print_string() {
        std::cout << val;
    }

    ListVal as_list() {
        return ListVal(std::vector<AbstractVal*>({this}));
    }
};

enum ComparisonResult {
    LESS,
    EQUAL,
    GREATER,
};

AbstractVal* parse(std::string line);
ComparisonResult compare(AbstractVal* left, AbstractVal* right);

void part1(std::ifstream& input) {
    std::string line;
    int index = 1;
    int index_sum = 0;
    do {
        std::getline(input, line);
        AbstractVal* left = parse(line);

        std::getline(input, line);
        AbstractVal* right = parse(line);

        ComparisonResult result = compare(left, right);

        if (result == ComparisonResult::LESS) {
            index_sum += index;
        }

        index++;
    } while (std::getline(input, line));

    std::cout << index_sum << std::endl;
}

void part2(std::ifstream& input) {
    std::string line;
    std::vector<AbstractVal*> values;
    while (std::getline(input, line)) {
        if (line != "") {
            values.push_back(parse(line));
        }
    }
    AbstractVal* div_2 = parse("[[2]]");
    AbstractVal* div_6 = parse("[[6]]");
    values.push_back(div_2);
    values.push_back(div_6);

    std::sort(values.begin(), values.end(), [=](AbstractVal* left, AbstractVal* right) { return compare(left, right) == ComparisonResult::LESS; });

    int result = 1;

    for (int i = 0; i < values.size(); i++) {
        if (compare(values[i], div_2) == ComparisonResult::EQUAL || compare(values[i], div_6) == ComparisonResult::EQUAL) {
            result *= i + 1;
        }
    }
    std::cout << result << std::endl;
}

AbstractVal* parse(std::string line) {
    std::stack<std::vector<AbstractVal*>> stack;
    stack.push(std::vector<AbstractVal*>());

    for (int i = 1; i < line.length() - 1; i++) {
        if (line[i] == '[') {
            stack.push(std::vector<AbstractVal*>());
        } else if (line[i] == ']') {
            ListVal* list = new ListVal(stack.top());
            stack.pop();
            stack.top().push_back(list);
        } else if (line[i] != ',') {
            size_t len;
            int val = std::stoi(line.c_str() + i, &len, 10);
            stack.top().push_back(new IntVal(val));
            i += len - 1;
        }
    }
    
    if (stack.size() != 1) {
        std::cerr << "Unmatched brackets" << std::endl;
    }
    return new ListVal(stack.top());
}

ComparisonResult compare(AbstractVal* left, AbstractVal* right) {
    if (left->is_int()) {
        if (right->is_int()) {
            int diff = ((IntVal*) left)->val - ((IntVal*) right)->val;
            if (diff == 0) {
                return ComparisonResult::EQUAL;
            } else if (diff < 0) {
                return ComparisonResult::LESS;
            } else {
                return ComparisonResult::GREATER;
            }
        } else {
            ListVal left_list = ((IntVal*) left)->as_list();
            return compare(&left_list, right);
        }
    } else {
        if (right->is_int()) {
            ListVal right_list = ((IntVal*) right)->as_list();
            return compare(left, &right_list);
        } else {
            std::vector<AbstractVal*> l = ((ListVal*) left)->val;
            std::vector<AbstractVal*> r = ((ListVal*) right)->val;

            for (int i = 0; ; i++) {
                if (i >= l.size()) {
                    if (i >= r.size()) {
                        return ComparisonResult::EQUAL;
                    } else {
                        return ComparisonResult::LESS;
                    }
                } else {
                    if (i >= r.size()) {
                        return ComparisonResult::GREATER;
                    } else {
                        ComparisonResult res = compare(l[i], r[i]);
                        if (res != ComparisonResult::EQUAL) {
                            return res;
                        }
                    }
                }
            }
        }
    }
}
