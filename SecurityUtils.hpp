#ifndef SECURITY_UTILS_HPP
#define SECURITY_UTILS_HPP

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <regex>

class SecurityUtils {
public:
 
    static std::string generateSalt(size_t length = 16) {
        const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::string salt;
        salt.reserve(length);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

        for (size_t i = 0; i < length; ++i) {
            salt += alphanum[dis(gen)];
        }
        return salt;
    }

    static std::string hashPassword(const std::string& password, const std::string& salt) {
       
        std::hash<std::string> hasher;
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') 
            << hasher(password + salt + "LMS_SECURE_KEY");
        return oss.str();
    }

  
    static bool validatePasswordStrength(const std::string& password) {
       
        if (password.length() < 8) return false;
        
        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;

        for (char c : password) {
            if (std::isupper(c)) hasUpper = true;
            else if (std::islower(c)) hasLower = true;
            else if (std::isdigit(c)) hasDigit = true;
            else if (std::ispunct(c)) hasSpecial = true;
        }

        return hasUpper && hasLower && hasDigit && hasSpecial;
    }
};

#endif 