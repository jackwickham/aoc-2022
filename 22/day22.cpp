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

#define FACE_SIZE 50

struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int>& pair) const {
        return pair.first * 63 + pair.second;
    }
};

enum Tile {
    OPEN, WALL, ABSENT,
};

struct Position {
    int x;
    int y;
    unsigned char dir; // [0..3]

    Position() : x(0), y(0), dir(0) {}

    Position(int x, int y, unsigned char dir) : x(x), y(y), dir(dir) {}
};

class WrappingStrategy {
public:
    virtual Position wrap(Position pos, std::vector<std::vector<Tile>>& map) = 0;
};

class Part1WrappingStrategy : public WrappingStrategy {
public:
    virtual Position wrap(Position pos, std::vector<std::vector<Tile>>& map) override {
        bool positive = (pos.dir & 2) == 0;
        bool update_x = (pos.dir & 1) == 0;
        if (update_x) {
            pos.x = positive ? 0 : (map[pos.y].size() - 1);
        } else {
            pos.y = positive ? 0 : (map.size() - 1);
        }
        while (map[pos.y].size() <= pos.x || map[pos.y][pos.x] == Tile::ABSENT) {
            (update_x ? pos.x : pos.y) += positive ? 1 : -1;
        }
        return pos;
    }
};

class State {
public:
    std::vector<std::vector<Tile>> map;
    Position pos;
    WrappingStrategy* wrapping_strategy;

    State(std::vector<std::vector<Tile>> map, WrappingStrategy* wrapping_strategy) : map(map), pos(Position()), wrapping_strategy(wrapping_strategy) {
        while (map[pos.y][pos.x] == Tile::ABSENT) {
            ++pos.x;
        }
    }

    void move(int amount) {
        while (amount > 0 && maybe_step()) {
            amount--;
        }
    }

    void rotate(bool clockwise) {
        if (clockwise) {
            pos.dir = (pos.dir + 1) % 4;
        } else {
            pos.dir = (pos.dir + 3) % 4;
        }
    }

private:
    bool maybe_step() {
        bool positive = (pos.dir & 2) == 0;
        bool update_x = (pos.dir & 1) == 0;

        Position updated = Position(pos);
        (update_x ? updated.x : updated.y) += positive ? 1 : -1;
        if (updated.x < 0 || updated.y < 0 || updated.y >= map.size() || updated.x >= map[updated.y].size() || map[updated.y][updated.x] == Tile::ABSENT) {
            updated = wrapping_strategy->wrap(pos, map);
        }

        if (map[updated.y][updated.x] == Tile::OPEN) {
            pos = updated;
            return true;
        } else {
            return false;
        }
    }
};

std::vector<std::vector<Tile>> parse(std::ifstream& input);

void part1(std::ifstream& input) {
    Part1WrappingStrategy strat = Part1WrappingStrategy{};
    State state = State(parse(input), &strat);
    while (input) {
        int amount;
        input >> amount;
        state.move(amount);

        if (input) {
            char rotate = 0;
            input >> rotate;
            if (rotate == 'R') {
                state.rotate(true);
            } else if (rotate == 'L') {
                state.rotate(false);
            }
        }
    }

    std::cout << 1000L * (state.pos.y + 1) + 4 * (state.pos.x + 1) + state.pos.dir << std::endl;
}

struct NetFace {
    NetFace* neighbours[4];
    int x;
    int y;

    NetFace(int x, int y) : neighbours{nullptr, nullptr, nullptr, nullptr}, x(x), y(y) {}

    std::size_t find_neighbour(NetFace* neighbour) {
        for (int i = 0; i < 4; i++) {
            if (neighbours[i] == neighbour) {
                return i;
            }
        }
        std::cerr << "Neighbour not found" << std::endl;
        exit(1);
    }
};

struct Part2WrappingStrategy : public WrappingStrategy {
    std::unordered_map<std::pair<int, int>, NetFace*, pair_hash> faces;

    Part2WrappingStrategy(std::unordered_map<std::pair<int, int>, NetFace*, pair_hash> faces) : faces(faces) {}

    virtual Position wrap(Position pos, std::vector<std::vector<Tile>>& map) override {
        NetFace* current_face = faces.find({pos.x / FACE_SIZE, pos.y / FACE_SIZE})->second;
        NetFace* target_face = current_face->neighbours[pos.dir];
        int edge = target_face->find_neighbour(current_face);
        unsigned char new_dir = (edge + 2) % 4;

        Position relative_pos = Position(pos.x % FACE_SIZE, pos.y % FACE_SIZE, pos.dir);
        if (relative_pos.dir == 0) {
            relative_pos.x = 0;
        } else if (relative_pos.dir == 1) {
            relative_pos.y = 0;
        } else if (relative_pos.dir == 2) {
            relative_pos.x = FACE_SIZE - 1;
        } else {
            relative_pos.y = FACE_SIZE - 1;
        }
        while (relative_pos.dir != new_dir) {
            relative_pos = Position(FACE_SIZE - 1 - relative_pos.y, relative_pos.x, (relative_pos.dir + 1) % 4);
        }
        return Position(target_face->x + relative_pos.x, target_face->y + relative_pos.y, relative_pos.dir);
    }
};

void part2(std::ifstream& input) {
    std::vector<std::vector<Tile>> map = parse(input);

    std::vector<std::vector<NetFace*>> faces_matrix;
    std::unordered_map<std::pair<int, int>, NetFace*, pair_hash> faces_map;
    for (int y = 0; y * FACE_SIZE < map.size(); y++) {
        std::vector<NetFace*> row;
        for (int x = 0; x * FACE_SIZE < map[y * FACE_SIZE].size(); x++) {
            if (map[y * FACE_SIZE][x * FACE_SIZE] != Tile::ABSENT) {
                NetFace* face = new NetFace(x * FACE_SIZE, y * FACE_SIZE);
                if (x > 0 && row[x - 1] != nullptr) {
                    face->neighbours[2] = row[x - 1];
                    row[x - 1]->neighbours[0] = face;
                }
                if (y > 0 && faces_matrix[y - 1].size() > x && faces_matrix[y - 1][x] != nullptr) {
                    face->neighbours[3] = faces_matrix[y - 1][x];
                    faces_matrix[y - 1][x]->neighbours[1] = face;
                }
                row.push_back(face);
                faces_map.insert({{x, y}, face});
            } else {
                row.push_back(nullptr);
            }
        }
        faces_matrix.push_back(row);
    }

    bool updated_any = true;
    while (updated_any) {
        updated_any = false;
        for (auto entry : faces_map) {
            for (int i = 0; i < 4; i++) {
                if (entry.second->neighbours[i] == nullptr) {
                    NetFace* common_neighbour = entry.second->neighbours[(i + 1) % 4];
                    if (common_neighbour != nullptr) {
                        NetFace* target = common_neighbour->neighbours[(common_neighbour->find_neighbour(entry.second) + 1) % 4];
                        if (target != nullptr) {
                            entry.second->neighbours[i] = target;
                            int target_index = (target->find_neighbour(common_neighbour) + 1) % 4;
                            target->neighbours[target_index] = entry.second;
                            updated_any = true;
                            continue;
                        }
                    }
                    common_neighbour = entry.second->neighbours[(i + 3) % 4];
                    if (common_neighbour != nullptr) {
                        NetFace* target = common_neighbour->neighbours[(common_neighbour->find_neighbour(entry.second) + 3) % 4];
                        if (target != nullptr) {
                            entry.second->neighbours[i] = target;
                            int target_index = (target->find_neighbour(common_neighbour) + 3) % 4;
                            target->neighbours[target_index] = entry.second;
                            updated_any = true;
                            continue;
                        }
                    }
                }
            }
        }
    }

    for (auto entry : faces_map) {
        for (int i = 0; i < 4; i++) {
            if (entry.second->neighbours[i] == nullptr) {
                std::cerr << "(" << entry.first.first << "," << entry.first.second << ") is missing neighbour on " << i << std::endl;
            }
        }
    }

    Part2WrappingStrategy strat = Part2WrappingStrategy(faces_map);
    State state = State(map, &strat);
    while (input) {
        int amount;
        input >> amount;
        state.move(amount);

        if (input) {
            char rotate = 0;
            input >> rotate;
            if (rotate == 'R') {
                state.rotate(true);
            } else if (rotate == 'L') {
                state.rotate(false);
            }
        }
        
        // for (int y = 0; y < state.map.size(); y++) {
        //     for (int x = 0; x < state.map[y].size(); x++) {
        //         if (state.pos.x == x && state.pos.y == y) {
        //             if (state.pos.dir == 0) {
        //                 std::cout << ">";
        //             } else if (state.pos.dir == 1) {
        //                 std::cout << "v";
        //             } else if (state.pos.dir == 2) {
        //                 std::cout << "<";
        //             } else {
        //                 std::cout << "^";
        //             }
        //         } else if (state.map[y][x] == Tile::OPEN) {
        //             std::cout << ".";
        //         } else if (state.map[y][x] == Tile::WALL) {
        //             std::cout << "#";
        //         } else {
        //             std::cout << " ";
        //         }
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << std::endl << std::endl;
    }

    std::cout << 1000L * (state.pos.y + 1) + 4 * (state.pos.x + 1) + state.pos.dir << std::endl;
}

std::vector<std::vector<Tile>> parse(std::ifstream& input) {
    std::vector<std::vector<Tile>> result;
    std::string line;
    while (true) {
        std::getline(input, line);
        if (line == "") {
            break;
        }
        std::vector<Tile> row;
        for (char c : line) {
            if (c == '.') {
                row.push_back(Tile::OPEN);
            } else if (c == '#') {
                row.push_back(Tile::WALL);
            } else {
                row.push_back(Tile::ABSENT);
            }
        }
        result.push_back(row);
    }
    return result;
}
