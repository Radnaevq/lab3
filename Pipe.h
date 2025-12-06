#pragma once
#include <string>
#include <iostream>

class Pipe {
private:
    static int nextId;
    int id;
    std::string name;
    double length;
    int diameter;
    bool inRepair;
    double getWeight() const {
        if (inRepair) {
            return std::numeric_limits<double>::max();
        }
        return length;
    }

public:
    Pipe();
    Pipe(const std::string& name, double length, int diameter, bool inRepair = false);

    static int getNextId() { return nextId; }
    int getId() const { return id; }
    std::string getName() const { return name; }
    double getLenght() const { return length; }
    int getDiametr() const { return diameter; }
    bool getRepair() const { return inRepair; }

    void setName(const std::string& newName) { name = newName; }
    void setLength(double newLength) { length = newLength; }
    void setDiametr(int newDiameter) { diameter = newDiameter; }
    void setRepair(bool repair) { inRepair = repair; }

    void toggleRepair() { inRepair = !inRepair; }
    bool isEmpty() const { return name.empty(); }
    void displayInfo() const;

    friend class PipeManager;
    friend std::ostream& operator<<(std::ostream& out, const Pipe& pipe);
    friend std::istream& operator>>(std::istream& in, Pipe& pipe);
    friend std::ofstream& operator<<(std::ofstream& out, const Pipe& pipe);
    friend std::ifstream& operator>>(std::ifstream& in, Pipe& pipe);

    static void resetIdCounter() { nextId = 1; }

};