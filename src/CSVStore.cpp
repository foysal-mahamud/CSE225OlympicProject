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

// ---------------------------------------------------------------------------
// RFC 4180-compliant CSV tokenizer.
// Handles quoted fields that contain commas, escaped double-quotes (""),
// and Windows-style \r\n line endings.
// ---------------------------------------------------------------------------
static std::vector<std::string> tokenizeCSV(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                // Two consecutive quotes inside a quoted field → literal quote.
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inQuotes = false;  // closing quote
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(field);
                field.clear();
            } else if (c == '\r') {
                // skip carriage return (Windows line endings)
            } else {
                field += c;
            }
        }
    }
    fields.push_back(field);  // push last field (no trailing comma needed)
    return fields;
}

// ---------------------------------------------------------------------------
// Parse one data row into an Athlete struct.
//
// The project ships with a 30-column CSV (Paris 2024 dataset) that has
// several blank "spacer" columns and a Country Code column not tracked by
// the struct.  Column index map for the ORIGINAL format:
//
//   0  Athlete ID     12 Medal ID        23 Start Time
//   1  Name           13 Medal Type      24 End Time
//   2  Gender         14 Medal Rank      28 Venue
//   3  Type           16 Opponent Name   29 Stadium
//   5  Country Name   18 Game
//   6  Native Lang.
//   7  Event Type
//   8  Event Name
//   9  Date of Birth
//
// After the program saves data it rewrites a compact 18-column file:
//   Athlete ID, Name, Gender, Type, Country Name, Native Language,
//   Event Type, Event Name, Date of Birth, Medal ID, Medal Type,
//   Medal Rank, Opponent Name, Game, Start Time, End Time, Venue, Stadium
//
// We detect which format we have by field count (>=25 → original).
// ---------------------------------------------------------------------------
static Athlete parseRow(const std::string& line) {
    auto f = tokenizeCSV(line);
    Athlete a;

    auto get = [&](size_t i) -> std::string {
        return i < f.size() ? f[i] : "";
    };

    if (f.size() >= 25) {
        // ── Original 30-column Paris 2024 dataset ──
        a.athleteID      = get(0);
        a.name           = get(1);
        a.gender         = get(2);
        a.type           = get(3);
        // [4] = Country Code (3-letter, not stored in struct)
        a.countryName    = get(5);
        a.nativeLanguage = get(6);
        a.eventType      = get(7);
        a.eventName      = get(8);
        a.dob            = get(9);
        // [10][11] empty spacer columns
        a.medalID        = get(12);
        a.medalType      = get(13);
        a.medalRank      = get(14);
        // [15] = Event Date  [17] empty
        a.opponentName   = get(16);
        // [17] empty
        a.game           = get(18);
        // [19-22] empty spacer columns
        a.eventStartTime = get(23);
        a.eventEndTime   = get(24);
        // [25-27] empty spacer columns
        a.venue          = get(28);
        a.stadium        = get(29);
    } else {
        // ── Compact 18-column format written by saveToCSV() ──
        a.athleteID      = get(0);
        a.name           = get(1);
        a.gender         = get(2);
        a.type           = get(3);
        a.countryName    = get(4);
        a.nativeLanguage = get(5);
        a.eventType      = get(6);
        a.eventName      = get(7);
        a.dob            = get(8);
        a.medalID        = get(9);
        a.medalType      = get(10);
        a.medalRank      = get(11);
        a.opponentName   = get(12);
        a.game           = get(13);
        a.eventStartTime = get(14);
        a.eventEndTime   = get(15);
        a.venue          = get(16);
        a.stadium        = get(17);
    }
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
        if (firstLine) {  // skip header row
            firstLine = false;
            continue;
        }
        if (line.empty() || line == "\r") continue;
        store::insertAthlete(parseRow(line));
    }
}

// Writes one athlete as a comma-separated row (18-column compact format).
static void writeRow(std::ostream& out, const Athlete& a) {
    out << a.athleteID      << "," << a.name           << "," << a.gender       << ","
        << a.type           << "," << a.countryName    << "," << a.nativeLanguage << ","
        << a.eventType      << "," << a.eventName      << "," << a.dob          << ","
        << a.medalID        << "," << a.medalType      << "," << a.medalRank    << ","
        << a.opponentName   << "," << a.game           << "," << a.eventStartTime << ","
        << a.eventEndTime   << "," << a.venue          << "," << a.stadium      << "\n";
}

// CSV header matching the 18-column compact format written by writeRow().
static const char kCSVHeader[] =
    "Athlete ID,Name,Gender,Type,Country Name,Native Language,"
    "Event Type,Event Name,Date of Birth,Medal ID,Medal Type,"
    "Medal Rank,Opponent Name,Game,Start Time,End Time,Venue,Stadium\n";

bool saveToCSV() {
    std::ofstream file(kDataPath, std::ios::trunc);
    if (!file.is_open()) {
        platform::animatedText("Error: Unable to open the CSV file for saving records.\n", 15);
        return false;
    }
    file << kCSVHeader;  // write header so the next load doesn't skip a data row
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
        // Strip trailing \r so the ID comparison works on Windows-format files.
        if (!line.empty() && line.back() == '\r') line.pop_back();

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
