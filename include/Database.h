#ifndef DATABASE_H
#define DATABASE_H

#include "SQLiteWrapper.h"
#include <mutex>
#include <memory>

struct PlayerStats {
    int playerId;
    std::string playerName;
    int gamesWon;
    int gamesLost;
    int totalGames;
    double winRate;
    
    PlayerStats() : playerId(-1), gamesWon(0), gamesLost(0), totalGames(0), winRate(0.0) {}
};

struct GameRecord {
    int gameId;
    std::string winner;
    std::string players;
    std::string timestamp;
    
    GameRecord() : gameId(-1) {}
};

class Database {
private:
    std::unique_ptr<SQLiteWrapper> db;
    std::mutex dbMutex;
    std::string dbPath;
    
    // Inizializzazione schema
    void createTables();
    void initializeSchema();
    
public:
    Database(const std::string& databasePath = "uno_game.db");
    ~Database();
    
    // Connessione
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    // Player statistics
    bool addPlayer(int playerId, const std::string& playerName);
    bool updatePlayerStats(int playerId, int wins, int losses);
    bool recordPlayerWin(int playerId);
    bool recordPlayerLoss(int playerId);
    PlayerStats getPlayerStats(int playerId);
    PlayerStats getPlayerStatsByName(const std::string& playerName);
    
    // Game records
    bool recordGame(const std::string& winner, const std::vector<std::string>& players);
    GameRecord getGameRecord(int gameId);
    std::vector<GameRecord> getAllGameRecords();
    int getTotalGamesPlayed();
    
    // Leaderboard
    std::vector<PlayerStats> getLeaderboard(int limit = 10);
    std::vector<PlayerStats> getAllPlayers();
    
    // Utility
    void resetDatabase();
    void backupDatabase(const std::string& backupPath);
    
    // Thread-safe operations
    void executeThreadSafe(const std::string& sql);
    std::vector<SQLiteWrapper::Row> queryThreadSafe(const std::string& sql);
};

#endif // DATABASE_H
