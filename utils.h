#pragma once
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "Pipe.h"
#include "CS.h" 

// Класс для логирования ввода
class InputLogger {
public:
    InputLogger() {
        static bool firstTime = true;
        if (firstTime) {
            firstTime = false;
            std::ofstream clearLog("user_input.log", std::ios::trunc);
            clearLog.close();
        }
    }

    void log(const std::string& input) {
        std::ofstream logFile("user_input.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << input << std::endl;
        }
    }

    static InputLogger& getInstance() {
        static InputLogger instance;
        return instance;
    }
};

inline void logInput(const std::string& input) {
    InputLogger::getInstance().log(input);
}

// Шаблон для ввода чисел
template<typename T>
inline bool inputNumber(T& variable, const std::string& prompt, bool positiveOnly = false) {
    while (true) {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);
        logInput(input);

        std::istringstream iss(input);
        if (iss >> variable) {
            char remaining;
            if (iss >> remaining) {
                std::cout << "Error, please enter only numbers without extra characters." << std::endl;
                continue;
            }

            if (positiveOnly && variable <= 0) {
                std::cout << "Error, value must be positive" << std::endl;
                continue;
            }

            return true;
        }
        else {
            std::cout << "Invalid input, please enter a valid number." << std::endl;
        }
    }
}

// Шаблон для ввода чисел в диапазоне (исправлена опечатка &a)
template<typename T>
inline bool inputInRange(T& variable, const std::string& prompt, T minValue, T maxValue) {
    while (true) {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);
        logInput(input);

        std::istringstream iss(input);
        if (iss >> variable) {
            char remaining;
            if (iss >> remaining) {
                std::cout << "Error, please enter only numbers without extra characters." << std::endl;
                continue;
            }

            if (variable >= minValue && variable <= maxValue) {
                return true;
            }
            else {
                std::cout << "Error, value must be between " << minValue << " and " << maxValue << std::endl;
                continue;
            }
        }
        else {
            std::cout << "Invalid input, please enter a valid number." << std::endl;
        }
    }
}

// Специализация для bool
template<>
inline bool inputInRange<bool>(bool& variable, const std::string& prompt, bool minValue, bool maxValue) {
    while (true) {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);
        logInput(input);

        std::istringstream iss(input);
        int temp;
        if (iss >> temp) {
            char remaining;
            if (iss >> remaining) {
                std::cout << "Error, please enter only 0 or 1 without extra characters." << std::endl;
                continue;
            }

            if (temp == 0 || temp == 1) {
                variable = (temp == 1);
                return true;
            }
            else {
                std::cout << "Error, please enter 0 or 1." << std::endl;
                continue;
            }
        }
        else {
            std::cout << "Invalid input, please enter 0 or 1." << std::endl;
        }
    }
}

template<typename Key, typename Value, typename Predicate>
std::vector<Key> findInMap(const std::unordered_map<Key, Value>& map, Predicate predicate) {
    std::vector<Key> result;
    for (const auto& pair : map) {
        if (predicate(pair.second)) {
            result.push_back(pair.first);
        }
    }
    return result;
}

namespace SearchUtils {
    inline std::vector<int> findPipesByName(const std::unordered_map<int, Pipe>& pipes, const std::string& name) {
        return findInMap(pipes, [&name](const Pipe& pipe) {
            return pipe.getName().find(name) != std::string::npos;
            });
    }

    inline std::vector<int> findPipesByRepairStatus(const std::unordered_map<int, Pipe>& pipes, bool inRepair) {
        return findInMap(pipes, [inRepair](const Pipe& pipe) {
            return pipe.getRepair() == inRepair;
            });
    }

    inline std::vector<int> findCSsByName(const std::unordered_map<int, CS>& stations, const std::string& name) {
        return findInMap(stations, [&name](const CS& cs) {
            return cs.getName().find(name) != std::string::npos;
            });
    }

    inline std::vector<int> findCSsByIdlePercentage(const std::unordered_map<int, CS>& stations, double minPercent, double maxPercent = 100.0) {
        return findInMap(stations, [minPercent, maxPercent](const CS& cs) {
            double idlePercent = cs.getIdlePercentage();
            return idlePercent >= minPercent && idlePercent <= maxPercent;
            });
    }
}