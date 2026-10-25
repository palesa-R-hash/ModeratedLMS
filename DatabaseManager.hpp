#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include "sqlite3.h"
#include <string>
#include <iostream>
#include <stdexcept>

class DatabaseManager {
private:
    sqlite3* db;

public:
    DatabaseManager(const std::string& dbName = "lms.db") {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
        }
        initializeTables();
    }

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
        }
    }

    // Initialize core schema for Users, Courses, and Audit Logs
    void initializeTables() {
        const char* sql = 
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT UNIQUE NOT NULL, "
            "password_hash TEXT NOT NULL, "
            "salt TEXT NOT NULL, "
            "role TEXT NOT NULL);"

            "CREATE TABLE IF NOT EXISTS audit_logs ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT NOT NULL, "
            "action TEXT NOT NULL, "
            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);"
        ;

        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
            std::string error = "SQL error: " + std::string(errMsg);
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }
    }

    // Safe execution of parameterized audit logging
    void logAction(const std::string& username, const std::string& action) {
        std::string sql = "INSERT INTO audit_logs (username, action) VALUES (?, ?);";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare audit log statement.");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to execute audit log insertion.");
        }

        sqlite3_finalize(stmt);
    }

    sqlite3* getConnection() { return db; }
};

#endif 