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

struct Blueprint {
    int ore_per_ore_robot;
    int ore_per_clay_robot;
    int ore_per_obsidian_robot;
    int clay_per_obsidian_robot;
    int ore_per_geode_robot;
    int obsidian_per_geode_robot;
};

struct State {
    int end_of_time;
    int ore;
    int clay;
    int obsidian;
    int geodes;
    int ore_robots;
    int clay_robots;
    int obsidian_robots;
    int geode_robots;

    State tick() {
        return State{
            .end_of_time = end_of_time + 1,
            .ore = ore + ore_robots,
            .clay = clay + clay_robots,
            .obsidian = obsidian + obsidian_robots,
            .geodes = geodes + geode_robots,
            .ore_robots = ore_robots,
            .clay_robots = clay_robots,
            .obsidian_robots = obsidian_robots,
            .geode_robots = geode_robots,
        };
    }
};

struct StoredState {
    State state;
    int lower_bound;
    int upper_bound;
};

struct state_ub_comparator {
    bool operator() (StoredState& left, StoredState& right) {
        return left.upper_bound < right.upper_bound;
    }
};

std::vector<Blueprint> parse(std::ifstream& input);

int calculate_ub(State state, Blueprint& blueprint, int threshold) {
    while (state.end_of_time < threshold) {
        State next = state.tick();
        next.clay_robots++;
        if (state.clay >= blueprint.clay_per_obsidian_robot) {
            next.obsidian_robots++;
            next.clay -= blueprint.clay_per_obsidian_robot;
        }
        if (state.obsidian >= blueprint.obsidian_per_geode_robot) {
            next.geode_robots++;
            next.obsidian -= blueprint.obsidian_per_geode_robot;
        }
        state = next;
    }
    return state.geodes;
}

int calculate_lb(State state, Blueprint& blueprint, int threshold) {
    while (state.end_of_time < threshold) {
        State next = state.tick();
        if (state.obsidian >= blueprint.obsidian_per_geode_robot && state.ore >= blueprint.ore_per_geode_robot) {
            next.geode_robots++;
            next.ore -= blueprint.ore_per_geode_robot;
            next.obsidian -= blueprint.obsidian_per_geode_robot;
        } else if (state.clay >= blueprint.clay_per_obsidian_robot && state.ore >= blueprint.ore_per_obsidian_robot) {
            next.obsidian_robots++;
            next.ore -= blueprint.ore_per_obsidian_robot;
            next.clay -= blueprint.clay_per_obsidian_robot;
        }
        state = next;
    }
    return state.geodes;
}

void maybe_push(
        std::priority_queue<StoredState, std::vector<StoredState>, state_ub_comparator>& queue,
        State& state,
        Blueprint blueprint,
        int max_score,
        int threshold) {
    StoredState stored_state = StoredState{
        .state = state,
        .lower_bound = calculate_lb(state, blueprint, threshold),
        .upper_bound = calculate_ub(state, blueprint, threshold),
    };
    if (stored_state.upper_bound > max_score && (queue.empty() || stored_state.upper_bound > queue.top().lower_bound)) {
        queue.push(stored_state);
    }
}

void part1(std::ifstream& input) {
    std::vector<Blueprint> blueprints = parse(input);
    int quality_level = 0;
    for (int i = 0; i < blueprints.size(); i++) {
        Blueprint blueprint = blueprints[i];
        int max_required_ore_robots = std::max({
            blueprint.ore_per_ore_robot,
            blueprint.ore_per_clay_robot,
            blueprint.ore_per_obsidian_robot,
            blueprint.ore_per_geode_robot
        });
        std::priority_queue<StoredState, std::vector<StoredState>, state_ub_comparator> queue;
        queue.push(StoredState{
            .state = State{
                .end_of_time = 0,
                .ore = 0,
                .clay = 0,
                .obsidian = 0,
                .geodes = 0,
                .ore_robots = 1,
                .clay_robots = 0,
                .obsidian_robots = 0,
                .geode_robots = 0,
            },
            .lower_bound = 0,
            .upper_bound = INT16_MAX,
        });
        int max_score = 0;
        while (!queue.empty() && queue.top().upper_bound > max_score) {
            StoredState stored_state = queue.top();
            queue.pop();

            State initial = stored_state.state;

            bool should_make_ore_robot = initial.ore_robots < max_required_ore_robots;
            bool should_make_clay_robot = initial.clay_robots < blueprint.clay_per_obsidian_robot;
            bool should_make_obsidian_robot = initial.clay_robots > 0 && initial.obsidian_robots < blueprint.obsidian_per_geode_robot;
            bool should_make_geode_robot = initial.obsidian_robots > 0;

            while (initial.end_of_time < 23) { // Doesn't make sense to build something at t=24
                if (!should_make_ore_robot && !should_make_clay_robot && !should_make_obsidian_robot && !should_make_geode_robot) {
                    // We've done some work this cycle and there's nothing left to do
                    break;
                }

                if (should_make_ore_robot && initial.ore >= blueprint.ore_per_ore_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_ore_robot;
                    updated.ore_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 24);
                    should_make_ore_robot = false;
                }
                if (should_make_clay_robot && initial.ore >= blueprint.ore_per_clay_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_clay_robot;
                    updated.clay_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 24);
                    should_make_clay_robot = false;
                }
                if (should_make_obsidian_robot
                        && initial.ore >= blueprint.ore_per_obsidian_robot
                        && initial.clay >= blueprint.clay_per_obsidian_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_obsidian_robot;
                    updated.clay -= blueprint.clay_per_obsidian_robot;
                    updated.obsidian_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 24);
                    should_make_obsidian_robot = false;
                }
                if (should_make_geode_robot
                        && initial.ore >= blueprint.ore_per_geode_robot
                        && initial.obsidian >= blueprint.obsidian_per_geode_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_geode_robot;
                    updated.obsidian -= blueprint.obsidian_per_geode_robot;
                    updated.geode_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 24);
                    should_make_geode_robot = false;
                }
                initial = initial.tick();
            }

            if (initial.end_of_time == 23) {
                State end = initial.tick();
                if (end.geodes > max_score) {
                    max_score = end.geodes;
                }
            }
        }

        std::cout << "max=" << max_score << std::endl;
        quality_level += max_score * (i + 1);
    }
    std::cout << quality_level << std::endl;
}

void part2(std::ifstream& input) {
    std::vector<Blueprint> blueprints = parse(input);
    int result = 1;
    for (int i = 0; i < std::min((int) blueprints.size(), 3); i++) {
        Blueprint blueprint = blueprints[i];
        int max_required_ore_robots = std::max({
            blueprint.ore_per_ore_robot,
            blueprint.ore_per_clay_robot,
            blueprint.ore_per_obsidian_robot,
            blueprint.ore_per_geode_robot
        });
        std::priority_queue<StoredState, std::vector<StoredState>, state_ub_comparator> queue;
        queue.push(StoredState{
            .state = State{
                .end_of_time = 0,
                .ore = 0,
                .clay = 0,
                .obsidian = 0,
                .geodes = 0,
                .ore_robots = 1,
                .clay_robots = 0,
                .obsidian_robots = 0,
                .geode_robots = 0,
            },
            .lower_bound = 0,
            .upper_bound = INT16_MAX,
        });
        int max_score = 0;
        while (!queue.empty() && queue.top().upper_bound > max_score) {
            StoredState stored_state = queue.top();
            queue.pop();

            State initial = stored_state.state;

            bool should_make_ore_robot = initial.ore_robots < max_required_ore_robots;
            bool should_make_clay_robot = initial.clay_robots < blueprint.clay_per_obsidian_robot;
            bool should_make_obsidian_robot = initial.clay_robots > 0 && initial.obsidian_robots < blueprint.obsidian_per_geode_robot;
            bool should_make_geode_robot = initial.obsidian_robots > 0;

            while (initial.end_of_time < 31) { // Doesn't make sense to build something at t=32
                if (!should_make_ore_robot && !should_make_clay_robot && !should_make_obsidian_robot && !should_make_geode_robot) {
                    // We've done some work this cycle and there's nothing left to do
                    break;
                }

                if (should_make_ore_robot && initial.ore >= blueprint.ore_per_ore_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_ore_robot;
                    updated.ore_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 32);
                    should_make_ore_robot = false;
                }
                if (should_make_clay_robot && initial.ore >= blueprint.ore_per_clay_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_clay_robot;
                    updated.clay_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 32);
                    should_make_clay_robot = false;
                }
                if (should_make_obsidian_robot
                        && initial.ore >= blueprint.ore_per_obsidian_robot
                        && initial.clay >= blueprint.clay_per_obsidian_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_obsidian_robot;
                    updated.clay -= blueprint.clay_per_obsidian_robot;
                    updated.obsidian_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 32);
                    should_make_obsidian_robot = false;
                }
                if (should_make_geode_robot
                        && initial.ore >= blueprint.ore_per_geode_robot
                        && initial.obsidian >= blueprint.obsidian_per_geode_robot) {
                    State updated = initial.tick();
                    updated.ore -= blueprint.ore_per_geode_robot;
                    updated.obsidian -= blueprint.obsidian_per_geode_robot;
                    updated.geode_robots += 1;
                    maybe_push(queue, updated, blueprint, max_score, 32);
                    should_make_geode_robot = false;
                }
                initial = initial.tick();
            }

            if (initial.end_of_time == 31) {
                State end = initial.tick();
                if (end.geodes > max_score) {
                    max_score = end.geodes;
                }
            }
        }

        std::cout << "max=" << max_score << std::endl;
        result *= max_score;
    }
    std::cout << result << std::endl;
}

std::vector<Blueprint> parse(std::ifstream& input) {
    std::regex re ("Blueprint \\d+: Each ore robot costs (\\d+) ore. Each clay robot costs (\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each geode robot costs (\\d+) ore and (\\d+) obsidian.");
    std::string line;
    std::vector<Blueprint> blueprints;
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex_match(line, match, re);
        blueprints.push_back(Blueprint{
            .ore_per_ore_robot = std::stoi(match[1]),
            .ore_per_clay_robot = std::stoi(match[2]),
            .ore_per_obsidian_robot = std::stoi(match[3]),
            .clay_per_obsidian_robot = std::stoi(match[4]),
            .ore_per_geode_robot = std::stoi(match[5]),
            .obsidian_per_geode_robot = std::stoi(match[6]),
        });
    }
    return blueprints;
}
