#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../shared/base.hpp"

struct node {
    node* parent;
    bool is_dir;
    long immediate_size;
    std::unordered_map<std::string, node*> children;
    std::string path;
};

struct dir_size {
    long size;
    long result;

    dir_size(long sz, long res) : size(sz), result(res) {};
};

node* parse(std::ifstream& input);
dir_size part1_recursive(node* node);
dir_size part2_recursive(node* node, long need_to_free);

void part1(std::ifstream& input) {
    node* root = parse(input);
    long result = part1_recursive(root).result;
    std::cout << result << std::endl;
}

dir_size part1_recursive(node* node) {
    dir_size result (node->immediate_size, 0);
    for (auto child : node->children) {
        dir_size res = part1_recursive(child.second);
        result.size += res.size;
        result.result += res.result;
    }
    if (node->is_dir && result.size <= 100000) {
        result.result += result.size;
    }
    return result;
}

void part2(std::ifstream& input) {
    node* root = parse(input);
    long need_to_free = part1_recursive(root).size - 40000000;
    long result = part2_recursive(root, need_to_free).result;

    std::cout << result << std::endl;
}


dir_size part2_recursive(node* node, long need_to_free) {
    dir_size result (node->immediate_size, __LONG_MAX__);
    for (auto child : node->children) {
        dir_size res = part2_recursive(child.second, need_to_free);
        result.size += res.size;
        if (res.result < result.result) {
            result.result = res.result;
        }
    }
    if (node->is_dir && result.size > need_to_free && result.size < result.result) {
        result.result = result.size;
    }
    return result;
}

node* parse(std::ifstream& input) {
    node* root = new node{
        .parent = nullptr,
        .is_dir = true,
        .immediate_size = 0,
        .children = std::unordered_map<std::string, node*>(),
        .path = "/",
    };
    node* current = root;

    std::string line;
    while (std::getline(input, line)) {
        if (line[0] == '$') {
            std::string op = line.substr(2, 2);
            if (op == "cd") {
                std::string name = line.substr(5);
                if (name == "/") {
                    current = root;
                } else if (name == "..") {
                    current = current->parent;
                } else {
                    current = current->children[name];
                }
            } else if (line.substr(2, 2) != "ls") {
                std::cerr << "Unexpected command in line " << line << std::endl;
                return root;
            }
        } else {
            int space = line.find(' ');
            std::string size_str = line.substr(0, space);
            std::string name = line.substr(space + 1);
            
            node* n;
            if (size_str == "dir") {
                n = new node{
                    .parent = current,
                    .is_dir = true,
                    .immediate_size = 0,
                    .children = std::unordered_map<std::string, node*>(),
                    .path = current->path + name + "/",
                };
            } else {
                n = new node{
                    .parent = current,
                    .is_dir = false,
                    .immediate_size = std::stoi(size_str),
                    .children = std::unordered_map<std::string, node*>(0),
                    .path = current->path + name,
                };
            }
            current->children[name] = n;
        }
    }

    return root;
}
