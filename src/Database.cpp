#include "Database.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

Database::Database(const std::string& databasePath)
    : dbPath(databasePath), db(nullptr) {
    try {
        db = std::make_unique<SQLiteWrapper>(databasePath);
        initializeSchema();
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error creating database: " + std::string(e.what()));
    }
}

Database::~Database() {
    try {
        disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Error in Database destructor: " << e.what() << std::endl;
    }
}

bool Database::connect() {
    try {
        if (!db) {
            db = std::make_unique<SQLiteWrapper>(dbPath);
        }
        return db->isOpen();
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error connecting to database: " + std::string(e.what()));
    }
}

void Database::disconnect() {
    try {
        if (db) {
            db->close();
            db = nullptr;
        }
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error disconnecting from database: " + std::string(e.what()));
    }
}

bool Database::isConnected() const {
    return db && db->isOpen();
}

void Database::createTables() {
    try {
        // Tabella dei giocatori
        std::string playersTable = R"(
            CREATE TABLE IF NOT EXISTS players (
                player_id INTEGER PRIMARY KEY,
                player_name TEXT NOT NULL UNIQUE,
                games_won INTEGER DEFAULT 0,
                games_lost INTEGER DEFAULT 0,
                total_games INTEGER DEFAULT 0,
                win_rate REAL DEFAULT 0.0,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        )";
        db->execute(playersTable);
        
        // Tabella delle partite
        std::string gamesTable = R"(
            CREATE TABLE IF NOT EXISTS games (
                game_id INTEGER PRIMARY KEY AUTOINCREMENT,
                winner TEXT NOT NULL,
                players TEXT NOT NULL,
                game_duration INTEGER,
                played_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY(winner) REFERENCES players(player_name)
            );
        )";
        db->execute(gamesTable);
        
        // Tabella dei log di gioco
        std::string gameLogsTable = R"(
            CREATE TABLE IF NOT EXISTS game_logs (
                log_id INTEGER PRIMARY KEY AUTOINCREMENT,
                game_id INTEGER NOT NULL,
                player_id INTEGER NOT NULL,
                action TEXT NOT NULL,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY(game_id) REFERENCES games(game_id),
                FOREIGN KEY(player_id) REFERENCES players(player_id)
            );
        )";
        db->execute(gameLogsTable);
        
        std::cout << "Database tables created successfully" << std::endl;
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error creating tables: " + std::string(e.what()));
    }
}

void Database::initializeSchema() {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (!isConnected()) {
            connect();
        }
        
        createTables();
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error initializing schema: " + std::string(e.what()));
    }
}

bool Database::addPlayer(int playerId, const std::string& playerName) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "INSERT OR IGNORE INTO players (player_id, player_name, games_won, games_lost, total_games) "
                         "VALUES (" + std::to_string(playerId) + ", '" + playerName + "', 0, 0, 0);";
        
        int changes = 0;
        db->executeUpdate(sql, changes);
        return changes > 0;
    } catch (const DatabaseException& e) {
        std::cerr << "Error adding player: " << e.what() << std::endl;
        return false;
    }
}

bool Database::updatePlayerStats(int playerId, int wins, int losses) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        int totalGames = wins + losses;
        double winRate = totalGames > 0 ? (double)wins / totalGames * 100.0 : 0.0;
        
        std::stringstream sql;
        sql << std::fixed << std::setprecision(2);
        sql << "UPDATE players SET games_won = " << wins 
            << ", games_lost = " << losses 
            << ", total_games = " << totalGames 
            << ", win_rate = " << winRate 
            << ", updated_at = CURRENT_TIMESTAMP "
            << "WHERE player_id = " << playerId << ";";
        
        int changes = 0;
        db->executeUpdate(sql.str(), changes);
        return changes > 0;
    } catch (const DatabaseException& e) {
        std::cerr << "Error updating player stats: " << e.what() << std::endl;
        return false;
    }
}

bool Database::recordPlayerWin(int playerId) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "UPDATE players SET games_won = games_won + 1, "
                         "total_games = total_games + 1, "
                         "win_rate = CAST(games_won AS FLOAT) / CAST(total_games AS FLOAT) * 100.0, "
                         "updated_at = CURRENT_TIMESTAMP "
                         "WHERE player_id = " + std::to_string(playerId) + ";";
        
        int changes = 0;
        db->executeUpdate(sql, changes);
        return changes > 0;
    } catch (const DatabaseException& e) {
        std::cerr << "Error recording player win: " << e.what() << std::endl;
        return false;
    }
}

bool Database::recordPlayerLoss(int playerId) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "UPDATE players SET games_lost = games_lost + 1, "
                         "total_games = total_games + 1, "
                         "win_rate = CAST(games_won AS FLOAT) / CAST(total_games AS FLOAT) * 100.0, "
                         "updated_at = CURRENT_TIMESTAMP "
                         "WHERE player_id = " + std::to_string(playerId) + ";";
        
        int changes = 0;
        db->executeUpdate(sql, changes);
        return changes > 0;
    } catch (const DatabaseException& e) {
        std::cerr << "Error recording player loss: " << e.what() << std::endl;
        return false;
    }
}

PlayerStats Database::getPlayerStats(int playerId) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT player_id, player_name, games_won, games_lost, total_games, win_rate "
                         "FROM players WHERE player_id = " + std::to_string(playerId) + ";";
        
        auto results = db->query(sql);
        if (results.empty()) {
            return PlayerStats();
        }
        
        PlayerStats stats;
        const auto& row = results[0].data;
        stats.playerId = playerId;
        stats.playerName = row.at("player_name");
        stats.gamesWon = std::stoi(row.at("games_won"));
        stats.gamesLost = std::stoi(row.at("games_lost"));
        stats.totalGames = std::stoi(row.at("total_games"));
        stats.winRate = std::stod(row.at("win_rate"));
        
        return stats;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting player stats: " << e.what() << std::endl;
        return PlayerStats();
    }
}

PlayerStats Database::getPlayerStatsByName(const std::string& playerName) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT player_id, player_name, games_won, games_lost, total_games, win_rate "
                         "FROM players WHERE player_name = '" + playerName + "';";
        
        auto results = db->query(sql);
        if (results.empty()) {
            return PlayerStats();
        }
        
        PlayerStats stats;
        const auto& row = results[0].data;
        stats.playerId = std::stoi(row.at("player_id"));
        stats.playerName = playerName;
        stats.gamesWon = std::stoi(row.at("games_won"));
        stats.gamesLost = std::stoi(row.at("games_lost"));
        stats.totalGames = std::stoi(row.at("total_games"));
        stats.winRate = std::stod(row.at("win_rate"));
        
        return stats;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting player stats by name: " << e.what() << std::endl;
        return PlayerStats();
    }
}

bool Database::recordGame(const std::string& winner, const std::vector<std::string>& players) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        // Crea una stringa con i nomi dei giocatori
        std::string playersList;
        for (size_t i = 0; i < players.size(); i++) {
            playersList += players[i];
            if (i < players.size() - 1) {
                playersList += ", ";
            }
        }
        
        std::string sql = "INSERT INTO games (winner, players) VALUES ('" + winner + "', '" + playersList + "');";
        
        int changes = 0;
        db->executeUpdate(sql, changes);
        return changes > 0;
    } catch (const DatabaseException& e) {
        std::cerr << "Error recording game: " << e.what() << std::endl;
        return false;
    }
}

GameRecord Database::getGameRecord(int gameId) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT game_id, winner, players, played_at FROM games WHERE game_id = " + std::to_string(gameId) + ";";
        
        auto results = db->query(sql);
        if (results.empty()) {
            return GameRecord();
        }
        
        GameRecord record;
        const auto& row = results[0].data;
        record.gameId = gameId;
        record.winner = row.at("winner");
        record.players = row.at("players");
        record.timestamp = row.at("played_at");
        
        return record;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting game record: " << e.what() << std::endl;
        return GameRecord();
    }
}

std::vector<GameRecord> Database::getAllGameRecords() {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT game_id, winner, players, played_at FROM games ORDER BY played_at DESC;";
        
        auto results = db->query(sql);
        std::vector<GameRecord> records;
        
        for (const auto& row : results) {
            GameRecord record;
            record.gameId = std::stoi(row.data.at("game_id"));
            record.winner = row.data.at("winner");
            record.players = row.data.at("players");
            record.timestamp = row.data.at("played_at");
            records.push_back(record);
        }
        
        return records;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting all game records: " << e.what() << std::endl;
        return std::vector<GameRecord>();
    }
}

int Database::getTotalGamesPlayed() {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT COUNT(*) as total FROM games;";
        std::string result = db->queryScalar(sql);
        
        return result.empty() ? 0 : std::stoi(result);
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting total games: " << e.what() << std::endl;
        return 0;
    }
}

std::vector<PlayerStats> Database::getLeaderboard(int limit) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT player_id, player_name, games_won, games_lost, total_games, win_rate "
                         "FROM players ORDER BY games_won DESC, win_rate DESC LIMIT " + std::to_string(limit) + ";";
        
        auto results = db->query(sql);
        std::vector<PlayerStats> leaderboard;
        
        for (const auto& row : results) {
            PlayerStats stats;
            stats.playerId = std::stoi(row.data.at("player_id"));
            stats.playerName = row.data.at("player_name");
            stats.gamesWon = std::stoi(row.data.at("games_won"));
            stats.gamesLost = std::stoi(row.data.at("games_lost"));
            stats.totalGames = std::stoi(row.data.at("total_games"));
            stats.winRate = std::stod(row.data.at("win_rate"));
            leaderboard.push_back(stats);
        }
        
        return leaderboard;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting leaderboard: " << e.what() << std::endl;
        return std::vector<PlayerStats>();
    }
}

std::vector<PlayerStats> Database::getAllPlayers() {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "SELECT player_id, player_name, games_won, games_lost, total_games, win_rate "
                         "FROM players ORDER BY player_name ASC;";
        
        auto results = db->query(sql);
        std::vector<PlayerStats> players;
        
        for (const auto& row : results) {
            PlayerStats stats;
            stats.playerId = std::stoi(row.data.at("player_id"));
            stats.playerName = row.data.at("player_name");
            stats.gamesWon = std::stoi(row.data.at("games_won"));
            stats.gamesLost = std::stoi(row.data.at("games_lost"));
            stats.totalGames = std::stoi(row.data.at("total_games"));
            stats.winRate = std::stod(row.data.at("win_rate"));
            players.push_back(stats);
        }
        
        return players;
    } catch (const DatabaseException& e) {
        std::cerr << "Error getting all players: " << e.what() << std::endl;
        return std::vector<PlayerStats>();
    }
}

void Database::resetDatabase() {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        db->execute("DROP TABLE IF EXISTS game_logs;");
        db->execute("DROP TABLE IF EXISTS games;");
        db->execute("DROP TABLE IF EXISTS players;");
        
        createTables();
        std::cout << "Database reset successfully" << std::endl;
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error resetting database: " + std::string(e.what()));
    }
}

void Database::backupDatabase(const std::string& backupPath) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::string sql = "VACUUM INTO '" + backupPath + "';";
        db->execute(sql);
        std::cout << "Database backed up to: " << backupPath << std::endl;
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error backing up database: " + std::string(e.what()));
    }
}

void Database::executeThreadSafe(const std::string& sql) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        db->execute(sql);
    } catch (const DatabaseException& e) {
        throw;
    }
}

std::vector<SQLiteWrapper::Row> Database::queryThreadSafe(const std::string& sql) {
    try {
        std::lock_guard<std::mutex> lock(dbMutex);
        return db->query(sql);
    } catch (const DatabaseException& e) {
        throw;
    }
}
