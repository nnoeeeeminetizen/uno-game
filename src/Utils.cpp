#include "Utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <limits>

#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

int Utils::getIntInput(int min, int max) {
    int input;
    
    while (true) {
        std::cin >> input;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
            continue;
        }
        
        if (input >= min && input <= max) {
            return input;
        }
        
        std::cout << "Input out of range. Please enter a number between " << min << " and " << max << ": ";
    }
}

std::string Utils::getStringInput() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void Utils::clearScreen() {
    system(CLEAR_SCREEN);
}

void Utils::pauseExecution() {
    std::cout << "\nPress ENTER to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Utils::colorToString(CardColor color) {
    switch (color) {
        case CardColor::RED: return "RED";
        case CardColor::YELLOW: return "YELLOW";
        case CardColor::GREEN: return "GREEN";
        case CardColor::BLUE: return "BLUE";
        case CardColor::WILD: return "WILD";
        default: return "UNKNOWN";
    }
}

std::string Utils::typeToString(CardType type) {
    switch (type) {
        case CardType::NUMBER_0: return "0";
        case CardType::NUMBER_1: return "1";
        case CardType::NUMBER_2: return "2";
        case CardType::NUMBER_3: return "3";
        case CardType::NUMBER_4: return "4";
        case CardType::NUMBER_5: return "5";
        case CardType::NUMBER_6: return "6";
        case CardType::NUMBER_7: return "7";
        case CardType::NUMBER_8: return "8";
        case CardType::NUMBER_9: return "9";
        case CardType::SKIP: return "SKIP";
        case CardType::REVERSE: return "REVERSE";
        case CardType::DRAW_TWO: return "DRAW_TWO";
        case CardType::WILD: return "WILD";
        case CardType::WILD_DRAW_FOUR: return "WILD_DRAW_FOUR";
        default: return "UNKNOWN";
    }
}

CardColor Utils::stringToColor(const std::string& str) {
    if (str == "RED") return CardColor::RED;
    if (str == "YELLOW") return CardColor::YELLOW;
    if (str == "GREEN") return CardColor::GREEN;
    if (str == "BLUE") return CardColor::BLUE;
    return CardColor::RED;  // Default
}

bool Utils::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool Utils::deleteFile(const std::string& filename) {
    return std::remove(filename.c_str()) == 0;
}

int Utils::getRandomInt(int min, int max) {
    static std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

bool Utils::isValidCardPlay(const std::shared_ptr<Card>& playedCard, 
                            const std::shared_ptr<Card>& topCard) {
    if (!playedCard || !topCard) {
        return false;
    }
    return playedCard->canPlayOn(*topCard);
}
