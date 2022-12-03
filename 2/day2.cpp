#include <iostream>
#include <fstream>
#include <string>
#include "../shared/base.hpp"

void part1(std::ifstream& input) {
    long score = 0;

    while (input.good()) {
        char theirs;
        char ours;
        
        input >> theirs;
        input >> ours;

        if (!input.good()) {
            break;
        }

        switch (ours) {
        case 'X': // Rock
            score += 1;
            switch (theirs) {
            case 'A': // Rock - draw
                score += 3;
                break;
            case 'B': // Paper - they win
                break;
            case 'C': // Scisors - we win
                score += 6;
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;

        case 'Y': // Paper
            score += 2;
            switch (theirs) {
            case 'A': // Rock - we win
                score += 6;
                break;
            case 'B': // Paper - draw
                score += 3;
                break;
            case 'C': // Scisors - they win
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;

        case 'Z': // Sciscors
            score += 3;
            switch (theirs) {
            case 'A': // Rock - they win
                break;
            case 'B': // Paper - we win
                score += 6;
                break;
            case 'C': // Scisors - draw
                score += 3;
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;
        
        default:
            std::cout << "Unexpected ours " << ours << std::endl;
            return;
        }
    }

    std::cout << score << std::endl;
}

void part2(std::ifstream& input) {
    long score = 0;

    while (input.good()) {
        char theirs;
        char ours;
        
        input >> theirs;
        input >> ours;

        if (!input.good()) {
            break;
        }

        switch (ours) {
        case 'X': // Lose
            switch (theirs) {
            case 'A': // Rock => Scissors
                score += 3;
                break;
            case 'B': // Paper => Rock
                score += 1;
                break;
            case 'C': // Scissors => Paper
                score += 2;
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;

        case 'Y': // Draw
            score += 3;
            switch (theirs) {
            case 'A': // Rock
                score += 1;
                break;
            case 'B': // Paper
                score += 2;
                break;
            case 'C': // Scissors
                score += 3;
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;

        case 'Z': // Win
            score += 6;
            switch (theirs) {
            case 'A': // Rock => Paper
                score += 2;
                break;
            case 'B': // Paper => Scissors
                score += 3;
                break;
            case 'C': // Scisors => Rock
                score += 1;
                break;
            default:
                std::cout << "Unexpected theirs " << theirs << std::endl;
                return;
            }
            break;
        
        default:
            std::cout << "Unexpected ours " << ours << std::endl;
            return;
        }
    }

    std::cout << score << std::endl;
}
