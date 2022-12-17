#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

#define DEFAULT_DISTANCE INT16_MAX

class Node {
public:
    std::vector<Node*> neighbours;
    char height;
    int distance;
    bool end;

    int weight1() {
        return -(this->distance + 'z' - this->height);
    }

    int weight2() {
        return -(this->distance + this->height - 'a');
    }
};

struct NodeComparator {
    bool operator() (Node* x, Node* y) {
        return x->weight1() < y->weight1();
    }
};

struct NodeComparator2 {
    bool operator() (Node* x, Node* y) {
        return x->weight2() < y->weight2();
    }
};

Node* parse_part1(std::ifstream& input);
Node* parse_part2(std::ifstream& input);

void part1(std::ifstream& input) {
    Node* start = parse_part1(input);

    std::priority_queue<Node*, std::vector<Node*>, NodeComparator> queue;
    queue.push(start);
    while(!queue.empty()) {
        Node* node = queue.top();
        queue.pop();
        for (Node* neighbour : node->neighbours) {
            if (neighbour->distance == DEFAULT_DISTANCE) {
                neighbour->distance = node->distance + 1;
                queue.push(neighbour);

                if (neighbour->end) {
                    std::cout << neighbour->distance << std::endl;
                    return;
                }
            }
        }
    }
    std::cerr << "Unexpectedly reached end of queue" << std::endl;
}

void part2(std::ifstream& input) {
    Node* end = parse_part2(input);

    std::priority_queue<Node*, std::vector<Node*>, NodeComparator2> queue;
    queue.push(end);
    while(!queue.empty()) {
        Node* node = queue.top();
        queue.pop();
        for (Node* neighbour : node->neighbours) {
            if (neighbour->distance == DEFAULT_DISTANCE) {
                neighbour->distance = node->distance + 1;
                queue.push(neighbour);

                if (neighbour->height == 'a') {
                    std::cout << neighbour->distance << std::endl;
                    return;
                }
            }
        }
    }
    std::cerr << "Unexpectedly reached end of queue" << std::endl;
}

Node* parse_part1(std::ifstream& input) {
    Node* start;

    std::vector<Node*> prev_row;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<Node*> row;
        Node* prev = nullptr;
        int idx = 0;
        for (char c : line) {
            Node* node = new Node{
                .neighbours = std::vector<Node*>(),
                .height = c,
                .distance = DEFAULT_DISTANCE,
                .end = false,
            };
            if (c == 'S') {
                node->height = 'a';
                node->distance = 0;
                start = node;
            } else if (c == 'E') {
                node->height = 'z';
                node->end = true;
            }

            if (prev != nullptr) {
                if (prev->height <= node->height + 1) {
                    node->neighbours.push_back(prev);
                }
                if (node->height <= prev->height + 1) {
                    prev->neighbours.push_back(node);
                }
            }
            if (!prev_row.empty()) {
                Node* above = prev_row[idx];
                if (above->height <= node->height + 1) {
                    node->neighbours.push_back(above);
                }
                if (node->height <= above->height + 1) {
                    above->neighbours.push_back(node);
                }
            }

            row.push_back(node);
            prev = node;
            idx++;
        }
        prev_row = row;
    }

    return start;
}

Node* parse_part2(std::ifstream& input) {
    Node* end;

    std::vector<Node*> prev_row;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<Node*> row;
        Node* prev = nullptr;
        int idx = 0;
        for (char c : line) {
            Node* node = new Node{
                .neighbours = std::vector<Node*>(),
                .height = c,
                .distance = DEFAULT_DISTANCE,
                .end = false,
            };
            if (c == 'S') {
                node->height = 'a';
            } else if (c == 'E') {
                node->height = 'z';
                node->end = true;
                node->distance = 0;
                end = node;
            }

            if (prev != nullptr) {
                if (prev->height <= node->height + 1) {
                    prev->neighbours.push_back(node);
                }
                if (node->height <= prev->height + 1) {
                    node->neighbours.push_back(prev);
                }
            }
            if (!prev_row.empty()) {
                Node* above = prev_row[idx];
                if (above->height <= node->height + 1) {
                    above->neighbours.push_back(node);
                }
                if (node->height <= above->height + 1) {
                    node->neighbours.push_back(above);
                }
            }

            row.push_back(node);
            prev = node;
            idx++;
        }
        prev_row = row;
    }

    return end;
}
