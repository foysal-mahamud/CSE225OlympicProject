#pragma once

#include <string>

struct Athlete {
    std::string athleteID;
    std::string name;
    std::string gender;
    std::string type;
    std::string countryName;
    std::string nativeLanguage;
    std::string eventType;
    std::string eventName;
    std::string dob;
    std::string medalID;
    std::string medalType;
    std::string medalRank;
    std::string opponentName;
    std::string game;
    std::string eventStartTime;
    std::string eventEndTime;
    std::string venue;
    std::string stadium;
    Athlete* next = nullptr;
    bool isChanged = false;
};
