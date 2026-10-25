#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <iostream>

// Abstract Base Class for polymorphism
class User {
protected:
    std::string username;
    std::string role;

public:
    User(std::string uname, std::string r) : username(std::move(uname)), role(std::move(r)) {}
    virtual ~User() = default;

    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }

    // Pure virtual function enforcing role-specific menu behavior
    virtual void showMenu() const = 0;
};

class Admin : public User {
public:
    Admin(std::string uname) : User(std::move(uname), "Admin") {}
    
    void showMenu() const override {
        std::cout << "\n=== Admin Menu (" << username << ") ===\n";
        std::cout << "1. Manage System Users\n";
        std::cout << "2. View System Audit Logs\n";
        std::cout << "3. System Settings\n";
        std::cout << "4. Logout\n";
    }
};

class Moderator : public User {
public:
    Moderator(std::string uname) : User(std::move(uname), "Moderator") {}
    
    void showMenu() const override {
        std::cout << "\n=== Moderator Menu (" << username << ") ===\n";
        std::cout << "1. Review Flagged Content\n";
        std::cout << "2. Moderate Forums/Discussions\n";
        std::cout << "3. Logout\n";
    }
};

class Instructor : public User {
public:
    Instructor(std::string uname) : User(std::move(uname), "Instructor") {}
    
    void showMenu() const override {
        std::cout << "\n=== Instructor Menu (" << username << ") ===\n";
        std::cout << "1. Create/Manage Courses\n";
        std::cout << "2. Grade Student Submissions\n";
        std::cout << "3. View Enrolled Students\n";
        std::cout << "4. Logout\n";
    }
};

class Student : public User {
public:
    Student(std::string uname) : User(std::move(uname), "Student") {}
    
    void showMenu() const override {
        std::cout << "\n=== Student Menu (" << username << ") ===\n";
        std::cout << "1. Browse Courses\n";
        std::cout << "2. View My Grades\n";
        std::cout << "3. Submit Assignments\n";
        std::cout << "4. Logout\n";
    }
};

#endif