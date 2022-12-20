#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

class Valve {
public:
    std::unordered_set<std::string> neighbours;
    int flow_rate;
};

struct State {
    std::vector<std::string> remaining;
    std::string pos;
    int score;
    int t;
    int lower_bound;
    int upper_bound;
    std::vector<std::string> history;
};

struct state_ub_comparator {
    bool operator() (State& left, State& right) {
        return left.upper_bound < right.upper_bound;
    }
};

struct State2 {
    std::vector<std::string> remaining;
    std::string pos;
    std::string el_pos;
    int score;
    int t;
    int el_t;
    int lower_bound;
    int upper_bound;
};

struct state2_ub_comparator {
    bool operator() (State2& left, State2& right) {
        return left.upper_bound < right.upper_bound;
    }
};

std::unordered_map<std::string, Valve> parse(std::ifstream& input);
std::unordered_map<std::string, std::unordered_map<std::string, int>> compute_shortest_paths(std::unordered_map<std::string, Valve> layout);
std::vector<std::string> relevant_keys(std::unordered_map<std::string, Valve> layout);
bool compute_part2(
        std::priority_queue<State2, std::vector<State2>, state2_ub_comparator>& search_space,
        std::unordered_map<std::string, std::unordered_map<std::string, int>>& distances,
        std::unordered_map<std::string, Valve>& layout,
        bool elephant,
        State2& current);

void part1(std::ifstream& input) {
    std::unordered_map<std::string, Valve> layout = parse(input);
    auto distances = compute_shortest_paths(layout);

    std::priority_queue<State, std::vector<State>, state_ub_comparator> search_space;
    std::vector<std::string> keys = relevant_keys(layout);
    std::sort(keys.begin(), keys.end(), [&](std::string left, std::string right) { return layout[left].flow_rate > layout[right].flow_rate; });
    search_space.emplace(State{
        .remaining = keys,
        .pos = "AA",
        .score = 0,
        .t = 30,
        .lower_bound = 0,
        .upper_bound = INT16_MAX,
        .history = {},
    });

    while (true) {
        State current = search_space.top();
        search_space.pop();

        if (current.lower_bound == current.upper_bound) {
            std::cout << current.lower_bound << std::endl;
            for (std::string node : current.history) {
                std::cout << node << ", ";
            }
            std::cout << std::endl;
            break;
        }

        for (std::string next : current.remaining) {
            int t = current.t - distances[current.pos][next] - 1;
            if (t <= 0) {
                continue;
            }
            int score = current.score + layout[next].flow_rate * t;
            std::vector<std::string> remaining;
            for (std::string other : current.remaining) {
                if (other != next) {
                    remaining.push_back(other);
                }
            }

            int upper_bound = score;
            for (std::string other : remaining) {
                int dist = distances[next][other];
                if (dist + 1 >= t) {
                    continue;
                }
                upper_bound += (t - dist - 1) * layout[other].flow_rate;
            }

            if (!search_space.empty() && upper_bound < search_space.top().lower_bound) {
                continue;
            }
            
            int lower_bound = score;
            int t_lb = t;
            for (std::string other : remaining) {
                int dist = distances[next][other];
                if (dist + 1 >= t_lb) {
                    continue;
                }
                t_lb -= dist + 1;
                lower_bound += t_lb * layout[other].flow_rate;
                t_lb -= dist;
                if (t_lb <= 0) {
                    break;
                }
            }

            std::vector<std::string> history = std::vector(current.history);
            history.push_back(next);

            search_space.emplace(State{
                .remaining = remaining,
                .pos = next,
                .score = score,
                .t = t,
                .lower_bound = lower_bound,
                .upper_bound = upper_bound,
                .history = history,
            });
        }
    }
}

void part2(std::ifstream& input) {
    std::unordered_map<std::string, Valve> layout = parse(input);
    auto distances = compute_shortest_paths(layout);

    std::priority_queue<State2, std::vector<State2>, state2_ub_comparator> search_space;
    std::vector<std::string> keys = relevant_keys(layout);
    std::sort(keys.begin(), keys.end(), [&](std::string left, std::string right) { return layout[left].flow_rate > layout[right].flow_rate; });
    search_space.emplace(State2{
        .remaining = keys,
        .pos = "AA",
        .el_pos = "AA",
        .score = 0,
        .t = 26,
        .el_t = 26,
        .lower_bound = 0,
        .upper_bound = INT16_MAX,
    });

    while (true) {
        State2 current = search_space.top();
        search_space.pop();

        // std::cout << "Checking node " << current.pos << "/" << current.el_pos << " score=" << current.score << " t=" << current.t << " el_t=" << current.el_t << " lb=" << current.lower_bound << " ub=" << current.upper_bound << " remaining=" << current.remaining.size() << " queue_sz=" << search_space.size() << std::endl;

        if (current.lower_bound == current.upper_bound) {
            std::cout << current.lower_bound << std::endl;
            break;
        }

        bool elephant = current.el_t > current.t;
        if (!compute_part2(search_space, distances, layout, elephant, current)) {
            compute_part2(search_space, distances, layout, !elephant, current);
        }
    }
}

bool compute_part2(
        std::priority_queue<State2, std::vector<State2>, state2_ub_comparator>& search_space,
        std::unordered_map<std::string, std::unordered_map<std::string, int>>& distances,
        std::unordered_map<std::string, Valve>& layout,
        bool elephant,
        State2& current) {
    bool any_move = false;
    for (std::string next : current.remaining) {
        int t = (elephant ? current.el_t : current.t) - distances[elephant ? current.el_pos : current.pos][next] - 1;
        if (t <= 0) {
            continue;
        }
        int score = current.score + layout[next].flow_rate * t;
        std::vector<std::string> remaining;
        for (std::string other : current.remaining) {
            if (other != next) {
                remaining.push_back(other);
            }
        }

        int upper_bound = score;
        for (std::string other : remaining) {
            int t_this = t - distances[next][other] - 1;
            int t_other = elephant ? (current.t - distances[current.pos][other] - 1) : (current.el_t - distances[current.el_pos][other] - 1);
            int t_best = std::max(t_this, t_other);
            if (t_best > 0) {
                upper_bound += t_best * layout[other].flow_rate;
            }
        }

        if (!search_space.empty() && upper_bound < search_space.top().lower_bound) {
            continue;
        }
        
        int lower_bound = score;
        int t_lb = t;
        for (std::string other : remaining) {
            int dist = distances[next][other];
            if (dist + 1 >= t_lb) {
                continue;
            }
            t_lb -= dist + 1;
            lower_bound += t_lb * layout[other].flow_rate;
            t_lb -= dist;
            if (t_lb <= 0) {
                break;
            }
        }

        any_move = true;

        // std::cout << "  Enqueuing node " << next << " score=" << score << " t=" << t << " lb=" << lower_bound << " ub=" << upper_bound << std::endl;

        search_space.emplace(State2{
            .remaining = remaining,
            .pos = elephant ? current.pos : next,
            .el_pos = elephant ? next : current.el_pos,
            .score = score,
            .t = elephant ? current.t : t,
            .el_t = elephant ? t : current.el_t,
            .lower_bound = lower_bound,
            .upper_bound = upper_bound,
        });
    }
    return any_move;
}

std::unordered_map<std::string, Valve> parse(std::ifstream& input) {
    std::unordered_map<std::string, Valve> map;
    std::regex re ("Valve (..) has flow rate=(\\d+); tunnels? leads? to valves? (.*)");
    std::string line;
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex_match(line, match, re);
        std::unordered_set<std::string> neighbours;
        for (int i = 0; i < match[3].str().size(); i += 4) {
            neighbours.insert(match[3].str().substr(i, 2));
        }
        map.insert({
            match[1].str(),
            Valve{
                .neighbours = neighbours,
                .flow_rate = std::stoi(match[2].str()),
            }
        });
    }
    return map;
}

std::unordered_map<std::string, std::unordered_map<std::string, int>> compute_shortest_paths(std::unordered_map<std::string, Valve> layout) {
    // https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
    std::unordered_map<std::string, std::unordered_map<std::string, int>> distances;
    std::vector<std::string> nodes;
    for (auto from : layout) {
        nodes.push_back(from.first);
        std::unordered_map<std::string, int> row;
        for (auto to : layout) {
            if (from.first == to.first) {
                row.insert({to.first, 0});
            } else if (from.second.neighbours.contains(to.first)) {
                row.insert({to.first, 1});
            } else {
                row.insert({to.first, INT16_MAX});
            }
        }
        distances.insert({from.first, row});
    }
    for (auto intermediate : nodes) {
        for (auto from : nodes) {
            for (auto to : nodes) {
                distances[from][to] = std::min(
                    distances[from][to],
                    distances[from][intermediate] + distances[intermediate][to]
                );
            }
        }
    }
    return distances;
}

std::vector<std::string> relevant_keys(std::unordered_map<std::string, Valve> layout) {
    std::vector<std::string> result;
    for (auto valve : layout) {
        if (valve.second.flow_rate > 0) {
            result.push_back(valve.first);
        }
    }
    return result;
}
