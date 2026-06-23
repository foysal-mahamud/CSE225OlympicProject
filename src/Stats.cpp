#include "Stats.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AthleteStore.h"
#include "Platform.h"

namespace stats {

namespace {
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}
}  // namespace

void showCountryStatistics() {
    if (!store::head()) {
        platform::animatedText("No records available to generate statistics.\n", 15);
        return;
    }
    std::cout << "Enter the country name to view statistics: ";
    std::string input;
    std::getline(std::cin, input);
    std::string target = toLower(input);

    int total = 0, gold = 0, silver = 0, bronze = 0;
    for (Athlete* cur = store::head(); cur; cur = cur->next) {
        if (toLower(cur->countryName) != target) continue;
        ++total;
        if (cur->medalType == "Gold Medal") ++gold;
        else if (cur->medalType == "Silver Medal") ++silver;
        else if (cur->medalType == "Bronze Medal") ++bronze;
    }

    if (total == 0) {
        platform::animatedText("No records found for the specified country.\n", 15);
        return;
    }
    std::cout << "\n--- Country Statistics ---\n"
              << "Country Name: " << input << "\n"
              << "Total Athletes: " << total << "\n"
              << "Gold Medals: " << gold << "\n"
              << "Silver Medals: " << silver << "\n"
              << "Bronze Medals: " << bronze << "\n";
}

void showTotalAthletes() {
    if (!store::head()) {
        platform::animatedText("No records available.\n", 15);
        return;
    }
    std::set<std::string> unique;
    for (Athlete* cur = store::head(); cur; cur = cur->next) {
        if (!cur->name.empty()) unique.insert(cur->name);
    }
    platform::animatedText("\n--- Olympic Participation Statistics ---\n", 25);
    std::cout << "Total Athletes Participated in the Olympics: " << unique.size() << "\n";
}

void showTopRankingCountriesAndAthletes() {
    if (!store::head()) {
        platform::animatedText("No records available to generate rankings.\n", 15);
        return;
    }
    std::unordered_map<std::string, int> countries, athletes;
    for (Athlete* cur = store::head(); cur; cur = cur->next) {
        if (!cur->countryName.empty()) ++countries[cur->countryName];
        if (!cur->name.empty()) ++athletes[cur->name];
    }
    auto byCountDesc = [](const std::pair<std::string, int>& a,
                          const std::pair<std::string, int>& b) {
        return a.second > b.second;
    };
    std::vector<std::pair<std::string, int>> cList(countries.begin(), countries.end());
    std::vector<std::pair<std::string, int>> aList(athletes.begin(), athletes.end());
    std::sort(cList.begin(), cList.end(), byCountDesc);
    std::sort(aList.begin(), aList.end(), byCountDesc);

    platform::animatedText("\n--- Top 5 Ranking Countries Based on Medal Count ---\n", 25);
    for (int i = 0; i < static_cast<int>(cList.size()) && i < 5; ++i) {
        std::cout << (i + 1) << ". " << cList[i].first
                  << " - Total Medals: " << cList[i].second << "\n";
    }
    platform::animatedText("\n--- Top 5 Performing Athletes Based on Medal Count ---\n", 25);
    for (int i = 0; i < static_cast<int>(aList.size()) && i < 5; ++i) {
        std::cout << (i + 1) << ". " << aList[i].first
                  << " - Total Medals: " << aList[i].second << "\n";
    }
}

void showPopularSportsAndMedalCounts() {
    if (!store::head()) {
        platform::animatedText("No records available to analyze Olympic sports.\n", 15);
        return;
    }
    std::unordered_map<std::string, int> sportCount;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> eventCount;
    for (Athlete* cur = store::head(); cur; cur = cur->next) {
        if (cur->eventType.empty()) continue;
        ++sportCount[cur->eventType];
        if (!cur->eventName.empty()) ++eventCount[cur->eventType][cur->eventName];
    }
    std::vector<std::pair<std::string, int>> sorted(sportCount.begin(), sportCount.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const std::pair<std::string, int>& a,
                 const std::pair<std::string, int>& b) { return a.second > b.second; });

    platform::animatedText("\n--- Most Popular Olympic Sports (by Medal Count) ---\n", 25);
    for (const auto& s : sorted) {
        std::cout << s.first << " - Total Medals: " << s.second << "\n";
    }
    platform::animatedText("\n--- Event-Specific Medal Counts ---\n", 25);
    for (const auto& s : sorted) {
        std::cout << "\nSport: " << s.first << "\n";
        for (const auto& e : eventCount[s.first]) {
            std::cout << "  Event: " << e.first << " - Medals: " << e.second << "\n";
        }
    }
}

}  // namespace stats
