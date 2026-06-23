#include "AthleteStore.h"
#include "Auth.h"
#include "CSVStore.h"
#include "Menu.h"
#include "Platform.h"

int main() {
    csv::loadFromCSV();

    platform::animatedText(
        "------ Welcome to the Olympic Athlete Tracking System -------\n\n", 50);

    auth::ensureDatabaseFileExists();
    auth::loadUsersFromFile();

    platform::animatedText("! Welcome to the Authentication System !\n", 10);
    auth::authenticationPage();

    platform::animatedText("Proceeding to the main system...\n", 10);
    menu::run();

    store::clear();
    return 0;
}
