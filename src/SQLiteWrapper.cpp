#include "SQLiteWrapper.h"
#include <iostream>
#include <sstream>

SQLiteWrapper::SQLiteWrapper(const std::string& databasePath)
    : db(nullptr), dbPath(databasePath) {
    try {
        open();
    } catch (const DatabaseException& e) {
        throw;
    }
}

SQLiteWrapper::~SQLiteWrapper() {
    try {
        close();
    } catch (const std::exception& e) {
        std::cerr << "Error in SQLiteWrapper destructor: " << e.what() << std::endl;
    }
}

void SQLiteWrapper::open() {
    try {
        int returnCode = sqlite3_open(dbPath.c_str(), &db);
        checkError(returnCode, "open database");
        
        // Abilita foreign keys
        std::string pragmaSQL = "PRAGMA foreign_keys = ON;";
        sqlite3_stmt* stmt = nullptr;
        returnCode = sqlite3_prepare_v2(db, pragmaSQL.c_str(), -1, &stmt, nullptr);
        checkError(returnCode, "prepare pragma statement");
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        std::cout << "Database opened: " << dbPath << std::endl;
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error opening database: " + std::string(e.what()));
    }
}

void SQLiteWrapper::close() {
    try {
        if (db != nullptr) {
            int returnCode = sqlite3_close(db);
            checkError(returnCode, "close database");
            db = nullptr;
            std::cout << "Database closed" << std::endl;
        }
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error closing database: " + std::string(e.what()));
    }
}

bool SQLiteWrapper::isOpen() const {
    return db != nullptr;
}

void SQLiteWrapper::execute(const std::string& sql) {
    try {
        if (!isOpen()) {
            throw DatabaseException("Database is not open");
        }
        
        char* errMsg = nullptr;
        int returnCode = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        
        if (returnCode != SQLITE_OK) {
            std::string error = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            throw DatabaseException("SQL execution failed: " + error);
        }
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error executing SQL: " + std::string(e.what()));
    }
}

void SQLiteWrapper::executeUpdate(const std::string& sql, int& changes) {
    try {
        execute(sql);
        changes = sqlite3_changes(db);
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error executing update: " + std::string(e.what()));
    }
}

std::vector<SQLiteWrapper::Row> SQLiteWrapper::query(const std::string& sql) {
    try {
        if (!isOpen()) {
            throw DatabaseException("Database is not open");
        }
        
        std::vector<Row> results;
        sqlite3_stmt* stmt = nullptr;
        
        int returnCode = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        checkError(returnCode, "prepare query");
        
        int columnCount = sqlite3_column_count(stmt);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Row row;
            
            for (int i = 0; i < columnCount; i++) {
                const char* columnName = sqlite3_column_name(stmt, i);
                const char* columnValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                
                row.data[columnName] = columnValue ? columnValue : "";
            }
            
            results.push_back(row);
        }
        
        sqlite3_finalize(stmt);
        return results;
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error executing query: " + std::string(e.what()));
    }
}

std::string SQLiteWrapper::queryScalar(const std::string& sql) {
    try {
        auto results = query(sql);
        if (results.empty()) {
            return "";
        }
        
        // Restituisci il primo valore della prima riga
        for (const auto& [key, value] : results[0].data) {
            return value;
        }
        
        return "";
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DatabaseException("Error executing scalar query: " + std::string(e.what()));
    }
}

void SQLiteWrapper::beginTransaction() {
    try {
        execute("BEGIN TRANSACTION;");
    } catch (const DatabaseException& e) {
        throw;
    }
}

void SQLiteWrapper::commit() {
    try {
        execute("COMMIT;");
    } catch (const DatabaseException& e) {
        throw;
    }
}

void SQLiteWrapper::rollback() {
    try {
        execute("ROLLBACK;");
    } catch (const DatabaseException& e) {
        throw;
    }
}

void SQLiteWrapper::checkError(int returnCode, const std::string& operation) {
    if (returnCode != SQLITE_OK && returnCode != SQLITE_DONE && returnCode != SQLITE_ROW) {
        std::string errorMsg = db ? sqlite3_errmsg(db) : "Unknown error";
        throw DatabaseException("Error during " + operation + ": " + errorMsg);
    }
}
