#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

// Clasa template - va fi instantiata cu <int> si <float> in proiect
template<typename T>
class ScoreBoard {
private:
    std::vector<std::pair<std::string, T>> entries;

public:
    void addEntry(const std::string& name, T score) {
        entries.push_back({name, score});
    }

    void sortDescending() {
        std::sort(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
    }

    void print(std::ostream& os) const {
        os << "=== Clasament ===\n";
        for (size_t i = 0; i < entries.size(); ++i)
            os << i + 1 << ". " << entries[i].first << ": " << entries[i].second << "\n";
    }

    T getTopScore() const {
        if (entries.empty()) return T{};
        return std::max_element(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; })->second;
    }

    // Cauta un entry dupa nume - algoritm cu lambda
    bool findEntry(const std::string& name) const {
        return std::any_of(entries.begin(), entries.end(),
            [&name](const auto& e) { return e.first == name; });
    }

    size_t size() const { return entries.size(); }
    void clear() { entries.clear(); }
};