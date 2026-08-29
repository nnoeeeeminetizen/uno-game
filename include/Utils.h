#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Card.h"

namespace Utils {
    // Input/Output
    int getIntInput(int min, int max);
    std::string getStringInput();
    void clearScreen();
    void pauseExecution();
    
    // Conversione stringhe
    std::string colorToString(CardColor color);
    std::string typeToString(CardType type);
    CardColor stringToColor(const std::string& str);
    
    // File
    bool fileExists(const std::string& filename);
    bool deleteFile(const std::string& filename);
    
    // Random
    int getRandomInt(int min, int max);
    
    // Validazione
    bool isValidCardPlay(const std::shared_ptr<Card>& playedCard, 
                        const std::shared_ptr<Card>& topCard);
}

#endif // UTILS_H
