#pragma once

#include <string>

namespace auth {

// Path to the file persisting registered users (email/password pairs).
extern const std::string kUserDatabasePath;

// Creates the user database file if it does not yet exist.
void ensureDatabaseFileExists();

// Loads existing users from disk into the in-memory map.
void loadUsersFromFile();

// Drives the login/register loop until the user is authenticated.
void authenticationPage();

}  // namespace auth
