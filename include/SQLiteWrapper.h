#ifndef SQLITEWRAPPER_H
#define SQLITEWRAPPER_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>

// Exception per errori di database
class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message) : std::runtime_error(message) {}
};

class SQLiteWrapper {
private:
    sqlite3* db;
    std::string dbPath;
    
    // Utility
    void checkError(int returnCode, const std::string& operation);
    
public:
    SQLiteWrapper(const std::string& databasePath);
    ~SQLiteWrapper();
    
    // Apertura/Chiusura
    void open();
    void close();
    bool isOpen() const;
    
    // Query execution
    void execute(const std::string& sql);
    void executeUpdate(const std::string& sql, int& changes);
    
    // Query con risultati
    struct Row {
        std::map<std::string, std::string> data;
    };
    
    std::vector<Row> query(const std::string& sql);
    std::string queryScalar(const std::string& sql);
    
    // Transactions
    void beginTransaction();
    void commit();
    void rollback();
    
    // Database info
    std::string getDatabasePath() const { return dbPath; }
    sqlite3* getHandle() const { return db; }
};

#endif // SQLITEWRAPPER_H
