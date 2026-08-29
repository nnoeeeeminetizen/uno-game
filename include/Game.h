#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Deck.h"
#include "AI.h"
#include <stdexcept>

// Exception per il gioco
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message) : std::runtime_error(message) {}
};

class Game {
private:
    Player** players;          // Array dinamico di puntatori a giocatori
    int playersCount;
    Deck* deck;                // Puntatore al mazzo
    int currentPlayerIndex;
    bool gameActive;
    bool reverseDirection;
    std::string savedGamePath;
    
    // Gestione turni
    void nextTurn();
    void previousTurn();
    void skipTurn();
    
    // Gestione carte speciali
    void handleSkip();
    void handleReverse();
    void handleDrawTwo();
    void handleWild(Player* player);
    void handleWildDrawFour(Player* player);
    
    // Utility
    void displayGameState() const;
    void distributeInitialCards();
    Player* getCurrentPlayer() const;
    Player* getNextPlayer() const;
    int getNextPlayerIndex() const;
    
public:
    Game();
    ~Game();
    
    // Inizializzazione e gestione partita
    void initializeGame(int numAIPlayers = 3);
    void playRound();
    void playGame();
    
    // Input giocatore umano
    bool humanPlayerTurn();
    
    // Turno IA
    void aiPlayerTurn();
    
    // Controllo stato
    bool isGameActive() const;
    Player* getWinner() const;
    
    // Persistenza
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
    
    // Debug e statistiche
    void printGameState() const;
    void printPlayerStats() const;
    void resetScores();
};

#endif // GAME_H
