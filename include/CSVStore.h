#pragma once

#include <set>
#include <string>

#include "Athlete.h"

namespace csv {

// Path to the athlete data file. Kept centralized so a single rename
// or relocation only touches this constant.
extern const std::string kDataPath;

// Tracks line indices in the CSV that were modified during the session.
std::set<int>& changedIndices();

// Loads the CSV into the in-memory linked-list store. Skips the header row.
void loadFromCSV();

// Persists the current linked-list state back to the CSV (truncating it).
bool saveToCSV();

// Rewrites a single CSV row identified by athlete ID.
void updateRecord(const std::string& id, const Athlete& updatedAthlete);

// Counts the number of lines in the data CSV.
int countLines();

}  // namespace csv
