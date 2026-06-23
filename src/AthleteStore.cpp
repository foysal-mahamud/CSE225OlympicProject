#include "AthleteStore.h"

namespace store {

static Athlete* g_head = nullptr;

Athlete* head() { return g_head; }

void setHead(Athlete* node) { g_head = node; }

void insertAthlete(const Athlete& athlete) {
    Athlete* node = new Athlete(athlete);
    node->next = nullptr;
    if (!g_head) {
        g_head = node;
        return;
    }
    Athlete* cur = g_head;
    while (cur->next) cur = cur->next;
    cur->next = node;
}

Athlete* findByID(const std::string& id) {
    for (Athlete* cur = g_head; cur; cur = cur->next) {
        if (cur->athleteID == id) return cur;
    }
    return nullptr;
}

void clear() {
    while (g_head) {
        Athlete* tmp = g_head;
        g_head = g_head->next;
        delete tmp;
    }
}

}  // namespace store
