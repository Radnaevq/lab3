#include "CSManager.h"
#include <fstream>
#include <iostream>

void CSManager::addCS(const CS& cs) {
    stations[cs.getId()] = cs;
}

CS& CSManager::createCS() {
    int newId = CS::getNextId();
    CS& cs = stations[newId];
    std::cin >> cs;
    return cs;
}

bool CSManager::deleteCS(int id) {
    return stations.erase(id) > 0;
}

void CSManager::deleteCSs(const std::vector<int>& ids) {
    for (int id : ids) {
        stations.erase(id);
    }
}

CS* CSManager::getCS(int id) {
    auto it = stations.find(id);
    return (it != stations.end()) ? &it->second : nullptr;
}

void CSManager::displayAllCSs() const {
    if (stations.empty()) {
        std::cout << "No compressor stations available." << std::endl;
        return;
    }
    for (const auto& entry : stations) {
        std::cout << entry.second;
    }
}

void CSManager::displayCSs(const std::vector<int>& ids) const {
    for (int id : ids) {
        auto it = stations.find(id);
        if (it != stations.end()) {
            std::cout << it->second;
        }
    }
}

std::vector<int> CSManager::getAllIds() const {
    std::vector<int> ids;
    for (const auto& entry : stations) {
        ids.push_back(entry.first);
    }
    return ids;
}

void CSManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        std::cout << "Error opening file for writing!" << std::endl;
        return;
    }

    for (const auto& entry : stations) {
        file << entry.second;
    }

    file.close();
    std::cout << "Compressor stations saved to " << filename << std::endl;
}

void CSManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file for reading!" << std::endl;
        return;
    }

    std::string line;
<<<<<<< HEAD
    int maxId = 0;
=======
>>>>>>> 464a28a2023f0fdfefb81102fc68bf378db427cf
    while (getline(file, line)) {
        if (line == "CS") {
            CS cs;
            file >> cs;
            stations[cs.getId()] = cs;
<<<<<<< HEAD
            if (cs.getId() > maxId) {
                maxId = cs.getId();
            }
=======
>>>>>>> 464a28a2023f0fdfefb81102fc68bf378db427cf
        }
        else if (line == "PIPE") {
            for (int i = 0; i < 5; ++i) {
                getline(file, line);
            }
        }
    }
<<<<<<< HEAD
    if (maxId > 0) {
        CS::nextId = maxId + 1;
    }
    file.close();
    std::cout << "Compressor stations loaded from " << filename << std::endl;
}
=======
    file.close();
    std::cout << "Compressor stations loaded from " << filename << std::endl;
}
>>>>>>> 464a28a2023f0fdfefb81102fc68bf378db427cf
