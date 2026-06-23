#pragma once

#include "Athlete.h"

namespace store {

// Returns the head of the linked list of athlete records.
Athlete* head();

// Replaces the current head. Used by load/delete routines.
void setHead(Athlete* node);

// Appends an athlete to the end of the linked list.
void insertAthlete(const Athlete& athlete);

// Finds an athlete by ID. Returns nullptr if not found.
Athlete* findByID(const std::string& id);

// Frees every node in the linked list.
void clear();

}  // namespace store
