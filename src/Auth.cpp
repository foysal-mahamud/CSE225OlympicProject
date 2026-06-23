#include "Auth.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>

#include "Platform.h"

namespace auth {

const std::string kUserDatabasePath = "users.dat";

namespace {
std::unordered_map<std::string, std::string> g_users;

void saveUserToFile(const std::string& email, const std::string& password) {
    std::ofstream file(kUserDatabasePath, std::ios::app);
    if (!file.is_open()) {
        platform::animatedText("Error: Failed to save user data.\n", 15);
        return;
    }
    file << email << "\n" << password << "\n";
}

void registerUser() {
    std::string email;
    while (true) {
        std::cout << "Enter your Email: ";
        std::cin >> email;
        if (g_users.find(email) != g_users.end()) {
            platform::animatedText("Email already registered. Try logging in.\n", 10);
        } else {
            break;
        }
    }
    std::string password = platform::readPassword();
    g_users[email] = password;
    saveUserToFile(email, password);
    platform::animatedText("Registration completed. You may now log in.\n", 10);
}

bool loginUser() {
    std::string email;
    std::cout << "Enter your Email: ";
    std::cin >> email;
    std::string password = platform::readPassword();
    auto it = g_users.find(email);
    if (it != g_users.end() && it->second == password) {
        platform::animatedText("Login successful!\n", 10);
        return true;
    }
    platform::animatedText("Invalid email or password.\n", 10);
    return false;
}
}  // namespace

void ensureDatabaseFileExists() {
    std::ifstream file(kUserDatabasePath);
    if (!file) {
        std::ofstream newFile(kUserDatabasePath);
        if (!newFile.is_open()) {
            std::cerr << "Error: Could not create database file at " << kUserDatabasePath
                      << "\n";
            std::exit(1);
        }
    }
}

void loadUsersFromFile() {
    std::ifstream file(kUserDatabasePath);
    if (!file.is_open()) {
        platform::animatedText("No existing user database found. Initializing.\n", 10);
        return;
    }
    std::string email, password;
    while (std::getline(file, email) && std::getline(file, password)) {
        g_users[email] = password;
    }
}

void authenticationPage() {
    while (true) {
        int choice = 0;
        std::cout << "1. Login\n2. Register\nChoose an option: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            platform::animatedText("Invalid option. Please enter 1 or 2.\n", 10);
            continue;
        }
        switch (choice) {
            case 1:
                if (loginUser()) return;
                break;
            case 2:
                registerUser();
                break;
            default:
                platform::animatedText("Invalid option. Try again.\n", 10);
        }
    }
}

}  // namespace auth
