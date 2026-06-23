#include "Menu.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "AthleteStore.h"
#include "CSVStore.h"
#include "Platform.h"
#include "Stats.h"

namespace menu {

void displayAthlete(const Athlete& a) {
    std::cout << "-----------------------------------\n";
    std::cout << "Athlete ID      : " << a.athleteID << "\n"
              << "Name            : " << a.name << "\n"
              << "Gender          : " << a.gender << "\n"
              << "Type            : " << a.type << "\n"
              << "Country         : " << a.countryName << "\n"
              << "Native Language : " << a.nativeLanguage << "\n"
              << "Event Type      : " << a.eventType << "\n"
              << "Event Name      : " << a.eventName << "\n"
              << "Date of Birth   : " << a.dob << "\n"
              << "Medal ID        : " << a.medalID << "\n"
              << "Medal Type      : " << a.medalType << "\n"
              << "Medal Rank      : " << a.medalRank << "\n"
              << "Opponent Name   : " << a.opponentName << "\n"
              << "Game            : " << a.game << "\n"
              << "Event Start     : " << a.eventStartTime << "\n"
              << "Event End       : " << a.eventEndTime << "\n"
              << "Venue           : " << a.venue << "\n"
              << "Stadium         : " << a.stadium << "\n";
    std::cout << "-----------------------------------\n";
}

namespace {

void viewRecords() {
    if (!store::head()) {
        platform::animatedText("No records available.\n", 15);
        return;
    }
    while (true) {
        std::cout << "\n1. View All Records\n2. View Paginated (5 per page)\n"
                     "3. Show specific record by index\n4. Back to Main Menu\nChoose: ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            platform::animatedText("Invalid option (1-4).\n", 15);
            continue;
        }
        if (choice == 1) {
            int total = 0;
            for (Athlete* cur = store::head(); cur; cur = cur->next) {
                displayAthlete(*cur);
                ++total;
            }
            std::cout << "Total Records: " << total << "\n";
            return;
        }
        if (choice == 2) {
            std::vector<Athlete*> pageStarts;
            for (Athlete* cur = store::head(); cur;) {
                pageStarts.push_back(cur);
                for (int i = 0; i < 5 && cur; ++i) cur = cur->next;
            }
            int totalPages = static_cast<int>(pageStarts.size());
            int page = 0;
            while (true) {
                Athlete* cur = pageStarts[page];
                std::cout << "\nPage " << (page + 1) << " of " << totalPages << ":\n";
                for (int i = 0; i < 5 && cur; ++i) {
                    displayAthlete(*cur);
                    cur = cur->next;
                }
                std::cout << "\n[N] Next [P] Prev [Q] Quit [B] Back: ";
                char nav;
                std::cin >> nav;
                nav = static_cast<char>(std::tolower(nav));
                if (nav == 'q' || nav == 'b') return;
                if (nav == 'n') {
                    if (page + 1 < totalPages) ++page;
                    else platform::animatedText("Last page reached.\n", 10);
                } else if (nav == 'p') {
                    if (page > 0) --page;
                    else platform::animatedText("First page.\n", 10);
                } else {
                    platform::animatedText("Invalid input.\n", 10);
                }
            }
        } else if (choice == 3) {
            std::cout << "Enter the index of the record to view: ";
            int idx = 0;
            if (!(std::cin >> idx)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                platform::animatedText("Invalid input.\n", 15);
                continue;
            }
            Athlete* cur = store::head();
            int count = 1;
            while (cur && count < idx) {
                cur = cur->next;
                ++count;
            }
            if (cur) displayAthlete(*cur);
            else platform::animatedText("No record at that index.\n", 15);
        } else if (choice == 4) {
            platform::animatedText("Returning to main menu...\n", 10);
            return;
        } else {
            platform::animatedText("Invalid option (1-4).\n", 15);
        }
    }
}

void searchAthlete() {
    if (!store::head()) {
        platform::animatedText("No records available.\n", 15);
        return;
    }
    while (true) {
        std::cout << "\n1. Unique Search (Athlete ID, Name, Medal ID)\n"
                     "2. Common Search (Gender, Type, Country, etc.)\n"
                     "3. Back to Main Menu\nChoose: ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            platform::animatedText("Invalid choice (1-3).\n", 15);
            continue;
        }
        if (choice == 3) return;
        std::cout << "Please enter your search query: ";
        std::cin.ignore();
        std::string q;
        std::getline(std::cin, q);
        bool found = false;
        for (Athlete* cur = store::head(); cur; cur = cur->next) {
            bool match = false;
            if (choice == 1) {
                match = cur->athleteID == q || cur->name == q || cur->medalID == q;
            } else if (choice == 2) {
                match = cur->gender == q || cur->type == q || cur->countryName == q ||
                        cur->nativeLanguage == q || cur->eventType == q ||
                        cur->eventName == q || cur->dob == q || cur->medalType == q ||
                        cur->medalRank == q || cur->opponentName == q || cur->game == q ||
                        cur->eventStartTime == q || cur->eventEndTime == q ||
                        cur->venue == q || cur->stadium == q;
            }
            if (match) {
                displayAthlete(*cur);
                found = true;
            }
        }
        if (!found) platform::animatedText("No matching records found.\n", 15);
    }
}

void addAthlete() {
    Athlete a;
    auto prompt = [](const std::string& label, std::string& out) {
        std::cout << "Enter " << label << " (blank if N/A): ";
        std::getline(std::cin, out);
    };
    prompt("Athlete ID", a.athleteID);
    prompt("Name", a.name);
    prompt("Gender", a.gender);
    prompt("Type", a.type);
    prompt("Country Name", a.countryName);
    prompt("Native Language", a.nativeLanguage);
    prompt("Event Type", a.eventType);
    prompt("Event Name", a.eventName);
    prompt("Date of Birth", a.dob);
    prompt("Medal ID", a.medalID);
    prompt("Medal Type", a.medalType);
    prompt("Medal Rank", a.medalRank);
    prompt("Opponent Name", a.opponentName);
    prompt("Game", a.game);
    prompt("Event Start Time", a.eventStartTime);
    prompt("Event End Time", a.eventEndTime);
    prompt("Venue", a.venue);
    prompt("Stadium", a.stadium);

    store::insertAthlete(a);
    std::ofstream file(csv::kDataPath, std::ios::app);
    if (!file.is_open()) {
        platform::animatedText("Error: Unable to save the record to CSV file.\n", 10);
        return;
    }
    file << a.athleteID << "," << a.name << "," << a.gender << "," << a.type << ","
         << a.countryName << "," << a.nativeLanguage << "," << a.eventType << ","
         << a.eventName << "," << a.dob << "," << a.medalID << "," << a.medalType << ","
         << a.medalRank << "," << a.opponentName << "," << a.game << "," << a.eventStartTime
         << "," << a.eventEndTime << "," << a.venue << "," << a.stadium << "\n";
    platform::animatedText("New record added successfully.\n", 10);
}

void updateAthlete() {
    if (!store::head()) {
        platform::animatedText("No records available.\n", 10);
        return;
    }
    std::cout << "Enter Athlete ID to update: ";
    std::string id;
    std::cin >> id;
    std::cin.ignore();
    Athlete* a = store::findByID(id);
    if (!a) {
        platform::animatedText("No record found for the specified Athlete ID.\n", 25);
        return;
    }
    auto promptUpdate = [](const std::string& label, std::string& field) {
        std::cout << "Enter new " << label << " (current: " << field << "): ";
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) field = line;
    };
    promptUpdate("Name", a->name);
    promptUpdate("Gender", a->gender);
    promptUpdate("Type", a->type);
    promptUpdate("Country Name", a->countryName);
    promptUpdate("Native Language", a->nativeLanguage);
    promptUpdate("Event Type", a->eventType);
    promptUpdate("Event Name", a->eventName);
    promptUpdate("Date of Birth", a->dob);
    promptUpdate("Medal ID", a->medalID);
    promptUpdate("Medal Type", a->medalType);
    promptUpdate("Medal Rank", a->medalRank);
    promptUpdate("Opponent Name", a->opponentName);
    promptUpdate("Game", a->game);
    promptUpdate("Event Start Time", a->eventStartTime);
    promptUpdate("Event End Time", a->eventEndTime);
    promptUpdate("Venue", a->venue);
    promptUpdate("Stadium", a->stadium);

    csv::updateRecord(id, *a);
    csv::changedIndices().insert(csv::countLines());
    platform::animatedText("Record updated successfully and marked as changed.\n", 15);
}

void deleteAthlete() {
    if (!store::head()) {
        platform::animatedText("No records available.\n", 25);
        return;
    }
    std::cout << "Delete by:\n1. Athlete ID\n2. Index Number\nChoose: ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();

    std::string id;
    int index = 0;
    if (choice == 1) {
        std::cout << "Enter Athlete ID to delete: ";
        std::cin >> id;
        std::cin.ignore();
    } else if (choice == 2) {
        std::cout << "Enter index number to delete: ";
        std::cin >> index;
        std::cin.ignore();
    } else {
        platform::animatedText("Invalid choice.\n", 25);
        return;
    }

    std::ifstream in(csv::kDataPath);
    if (!in.is_open()) {
        platform::animatedText("Error: Unable to open CSV file.\n", 25);
        return;
    }
    std::vector<std::string> remaining;
    std::string line;
    int row = 1;
    bool found = false;
    std::string deletedLine;
    while (std::getline(in, line)) {
        bool isMatch =
            (choice == 1 && line.substr(0, line.find(',')) == id) ||
            (choice == 2 && row == index);
        if (isMatch) {
            found = true;
            deletedLine = line;
        } else {
            remaining.push_back(line);
        }
        ++row;
    }
    in.close();

    if (!found) {
        platform::animatedText("No matching record found.\n", 15);
        return;
    }
    std::ofstream out(csv::kDataPath, std::ios::trunc);
    if (!out.is_open()) {
        platform::animatedText("Error: Unable to write to CSV file.\n", 25);
        return;
    }
    for (const auto& r : remaining) out << r << "\n";

    Athlete* prev = nullptr;
    Athlete* cur = store::head();
    int idx = 1;
    while (cur) {
        bool isMatch = (choice == 1 && cur->athleteID == id) ||
                       (choice == 2 && idx == index);
        if (isMatch) {
            if (!prev) store::setHead(cur->next);
            else prev->next = cur->next;
            delete cur;
            break;
        }
        prev = cur;
        cur = cur->next;
        ++idx;
    }
    platform::animatedText("Record deleted successfully.\n", 25);
    std::cout << "Deleted row: " << deletedLine << "\n";
}

void showChangedRecords() {
    const auto& changed = csv::changedIndices();
    if (changed.empty()) {
        platform::animatedText("No changes have been made to the records.\n", 25);
        return;
    }
    std::ifstream in(csv::kDataPath);
    if (!in.is_open()) {
        platform::animatedText("Error: Unable to open CSV file.\n", 25);
        return;
    }
    std::string line;
    int row = 1;
    std::cout << "Changed Records:\n";
    while (std::getline(in, line)) {
        if (changed.count(row)) std::cout << "Index " << row << ": " << line << "\n";
        ++row;
    }
}

void displayGroupMates() {
    const std::vector<std::string> members = {
        "Group Member : Rafiur Rahman Mashrafi, ID: 2221971042",
        "Group Member : Dhruba Saha           , ID: 2232537042",
        "Group Member : Md. Tanvir Rahman     , ID: 2211462042",
        "Group Member : Foysal Mahamud        , ID: 2231078042",
    };
    for (const auto& m : members) platform::animatedText(m + "\n", 50);
}

}  // namespace

void run() {
    while (true) {
        platform::animatedText("\n--- Paris 2024 Olympic Summer Games ---\n", 50);
        std::cout << "1.  View Records\n"
                     "2.  Search for a Record\n"
                     "3.  Add a New Record\n"
                     "4.  Update an Existing Record\n"
                     "5.  Delete a Record\n"
                     "6.  Show Records After Change\n"
                     "7.  Show Country Statistics\n"
                     "8.  Show Total Athletes Participated\n"
                     "9.  Show Top Ranking Countries and Athletes\n"
                     "10. Show Most Popular Sports and Event-Specific Medal Counts\n"
                     "11. Exit\n"
                     "Enter your choice: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            platform::animatedText("Invalid choice. Please enter a number.\n", 25);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: viewRecords(); break;
            case 2: searchAthlete(); break;
            case 3: addAthlete(); break;
            case 4: updateAthlete(); break;
            case 5: deleteAthlete(); break;
            case 6: showChangedRecords(); break;
            case 7: stats::showCountryStatistics(); break;
            case 8: stats::showTotalAthletes(); break;
            case 9: stats::showTopRankingCountriesAndAthletes(); break;
            case 10: stats::showPopularSportsAndMedalCounts(); break;
            case 11:
                platform::animatedText("Exiting the system...\n", 50);
                displayGroupMates();
                return;
            default:
                platform::animatedText("Invalid choice. Please input the correct option.\n", 25);
        }
    }
}

}  // namespace menu
