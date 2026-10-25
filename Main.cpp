#include <iostream>
#include <memory>
#include <string>
#include "DatabaseManager.hpp"
#include "SecurityUtils.hpp"
#include "User.hpp"


std::unique_ptr<User> createUserInstance(const std::string& username, const std::string& role) {
    if (role == "Admin") return std::make_unique<Admin>(username);
    if (role == "Moderator") return std::make_unique<Moderator>(username);
    if (role == "Instructor") return std::make_unique<Instructor>(username);
    if (role == "Student") return std::make_unique<Student>(username);
    return nullptr;
}


void registerUser(DatabaseManager& dbManager) {
    std::string username, password, role;
    std::cout << "\n--- User Registration ---\n";
    std::cout << "Enter username: ";
    std::cin >> username;
    
    std::cout << "Enter password (min 8 chars, uppercase, lowercase, digit, special char): ";
    std::cin >> password;

    if (!SecurityUtils::validatePasswordStrength(password)) {
        std::cout << "[!] Error: Password does not meet complexity requirements.\n";
        return;
    }

    std::cout << "Select Role (1: Admin, 2: Moderator, 3: Instructor, 4: Student): ";
    int roleChoice;
    std::cin >> roleChoice;

    switch (roleChoice) {
        case 1: role = "Admin"; break;
        case 2: role = "Moderator"; break;
        case 3: role = "Instructor"; break;
        case 4: role = "Student"; break;
        default:
            std::cout << "[!] Invalid role selection. Defaulting to Student.\n";
            role = "Student";
            break;
    }

    std::string salt = SecurityUtils::generateSalt();
    std::string passwordHash = SecurityUtils::hashPassword(password, salt);

   
    std::string sql = "INSERT INTO users (username, password_hash, salt, role) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3* db = dbManager.getConnection();

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "[!] Database error during preparation.\n";
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "[+] Registration successful for " << username << " as " << role << "!\n";
        dbManager.logAction(username, "USER_REGISTERED");
    } else {
        std::cout << "[!] Registration failed. Username may already exist.\n";
    }

    sqlite3_finalize(stmt);
}

std::unique_ptr<User> loginUser(DatabaseManager& dbManager) {
    std::string username, password;
    std::cout << "\n--- User Login ---\n";
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::string sql = "SELECT password_hash, salt, role FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    sqlite3* db = dbManager.getConnection();

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "[!] Database error during login preparation.\n";
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    std::unique_ptr<User> loggedInUser = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        std::string computedHash = SecurityUtils::hashPassword(password, salt);

        if (computedHash == storedHash) {
            std::cout << "[+] Login successful! Welcome back, " << username << ".\n";
            dbManager.logAction(username, "USER_LOGIN_SUCCESS");
            loggedInUser = createUserInstance(username, role);
        } else {
            std::cout << "[!] Invalid password.\n";
            dbManager.logAction(username, "USER_LOGIN_FAILED_INVALID_PASSWORD");
        }
    } else {
        std::cout << "[!] User not found.\n";
    }

    sqlite3_finalize(stmt);
    return loggedInUser;
}

int main() {
    try {
        DatabaseManager dbManager("lms.db");
        std::cout << "========================================\n";
        std::cout << "   Moderated Learning Management System \n";
        std::cout << "========================================\n";

        bool running = true;
        while (running) {
            std::cout << "\n1. Register\n2. Login\n3. Exit\nChoice: ";
            int choice;
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(32767, '\n');
                std::cout << "[!] Invalid input. Please enter a number.\n";
                continue;
            }

            if (choice == 1) {
                registerUser(dbManager);
            } else if (choice == 2) {
                auto currentUser = loginUser(dbManager);
                if (currentUser) {
                    bool sessionActive = true;
                    while (sessionActive) {
                        currentUser->showMenu();
                        std::cout << "Select option: ";
                        int menuChoice;
                        std::cin >> menuChoice;

                        if (menuChoice == 4 && currentUser->getRole() == "Admin") {
                            dbManager.logAction(currentUser->getUsername(), "USER_LOGOUT");
                            sessionActive = false;
                        } else if (menuChoice == 3 && currentUser->getRole() != "Admin") {
                            dbManager.logAction(currentUser->getUsername(), "USER_LOGOUT");
                            sessionActive = false;
                        } else {
                            std::cout << "[*] Feature placeholder or simulated action executed.\n";
                            dbManager.logAction(currentUser->getUsername(), "MENU_ACTION_" + std::to_string(menuChoice));
                        }
                    }
                }
            } else if (choice == 3) {
                running = false;
                std::cout << "Exiting application. Goodbye!\n";
            } else {
                std::cout << "[!] Invalid choice. Try again.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Critical Exception]: " << e.what() << '\n';
        return 1;
    }

    return 0;
}