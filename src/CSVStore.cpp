#include "CSVStore.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "AthleteStore.h"
#include "Platform.h"

namespace csv {

const std::string kDataPath = "data/athletes.csv";

std::set<int>& changedIndices() {
    static std::set<int> instance;
    return instance;
}

static Athlete parseRow(const std::string& line) {
    Athlete a;
    std::string fields[18];
    std::string remaining = line;
    size_t index = 0;
    size_t pos = 0;
    while ((pos = remaining.find(',')) != std::string::npos && index < 17) {
        fields[index++] = remaining.substr(0, pos);
        remaining.erase(0, pos + 1);
    }
    fields[index] = remaining;
    a.athleteID = fields[0];
    a.name = fields[1];
    a.gender = fields[2];
    a.type = fields[3];
    a.countryName = fields[4];
    a.nativeLanguage = fields[5];
    a.eventType = fields[6];
    a.eventName = fields[7];
    a.dob = fields[8];
    a.medalID = fields[9];
    a.medalType = fields[10];
    a.medalRank = fields[11];
    a.opponentName = fields[12];
    a.game = fields[13];
    a.eventStartTime = fields[14];
    a.eventEndTime = fields[15];
    a.venue = fields[16];
    a.stadium = fields[17];
    return a;
}

void loadFromCSV() {
    std::ifstream file(kDataPath);
    if (!file.is_open()) {
        platform::animatedText(
            "No CSV file found at " + kDataPath + ". Initializing empty store.\n\n", 15);
        return;
    }
    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (firstLine) {  // Skip header row.
            firstLine = false;
            continue;
        }
        if (line.empty()) continue;
        store::insertAthlete(parseRow(line));
    }
}

static void writeRow(std::ostream& out, const Athlete& a) {
    out << a.athleteID << "," << a.name << "," << a.gender << "," << a.type << ","
        << a.countryName << "," << a.nativeLanguage << "," << a.eventType << ","
        << a.eventName << "," << a.dob << "," << a.medalID << "," << a.medalType << ","
        << a.medalRank << "," << a.opponentName << "," << a.game << "," << a.eventStartTime
        << "," << a.eventEndTime << "," << a.venue << "," << a.stadium << "\n";
}

bool saveToCSV() {
    std::ofstream file(kDataPath, std::ios::trunc);
    if (!file.is_open()) {
        platform::animatedText("Error: Unable to open the CSV file for saving records.\n", 15);
        return false;
    }
    for (Athlete* cur = store::head(); cur; cur = cur->next) {
        writeRow(file, *cur);
    }
    platform::animatedText("All records saved to " + kDataPath + ".\n", 10);
    return true;
}

void updateRecord(const std::string& id, const Athlete& updatedAthlete) {
    std::ifstream in(kDataPath);
    if (!in.is_open()) {
        platform::animatedText("Error: Unable to open CSV file.\n", 15);
        return;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos && line.substr(0, pos) == id) {
            std::ostringstream out;
            writeRow(out, updatedAthlete);
            std::string row = out.str();
            if (!row.empty() && row.back() == '\n') row.pop_back();
            lines.push_back(row);
        } else {
            lines.push_back(line);
        }
    }
    in.close();

    std::ofstream out(kDataPath, std::ios::trunc);
    if (!out.is_open()) {
        platform::animatedText("Error: Unable to write to the CSV file.\n", 15);
        return;
    }
    for (const auto& row : lines) out << row << "\n";
}

int countLines() {
    std::ifstream file(kDataPath);
    int count = 0;
    std::string line;
    while (std::getline(file, line)) ++count;
    return count;
}

}  // namespace csv
