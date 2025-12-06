#pragma once
#include "CS.h"
#include <unordered_map>
#include <vector>
#include <string>

class CSManager {
private:
    std::unordered_map<int, CS> stations;

public:
    void addCS(const CS& cs);
    CS& createCS();

    bool deleteCS(int id);
    void deleteCSs(const std::vector<int>& ids);

    CS* getCS(int id);
    void displayAllCSs() const;
    void displayCSs(const std::vector<int>& ids) const;

    bool isEmpty() const { return stations.empty(); }
    size_t getCount() const { return stations.size(); }
    std::vector<int> getAllIds() const;

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    const std::unordered_map<int, CS>& getStations() const { return stations; }
};