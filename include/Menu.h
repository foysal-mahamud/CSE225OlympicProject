#pragma once

#include "Athlete.h"

namespace menu {

// Pretty-prints a single athlete record.
void displayAthlete(const Athlete& athlete);

// Top-level menu loop. Returns when the user selects Exit.
void run();

}  // namespace menu
