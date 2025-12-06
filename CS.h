#pragma once
#include <string>
#include <iostream>

class CS {
private:
    static int nextId;
    int id;
    std::string name;
    int workshop;
    int w_work;
    double class_cs;

public:
    CS();
    CS(const std::string& name, int workshop, int w_work, double class_cs);

    static int getNextId() { return nextId; }
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getWorkshop() const { return workshop; }
    int getW_work() const { return w_work; }
    int getIdleWorkshops() const { return workshop - w_work; }
    double getClass_cs() const { return class_cs; }
    double getIdlePercentage() const {
        return workshop > 0 ? (getIdleWorkshops() * 100.0 / workshop) : 0.0;
    }

    void setName(const std::string& newName) { name = newName; }
    void setWorkshop(int total);
    void setW_work(int working);
    void setClass_cs(double cls) { class_cs = cls; }

    bool startWorkshop();
    bool stopWorkshop();
    bool isEmpty() const { return name.empty(); }
    void displayInfo() const;

    friend class CSManager;
    friend std::ostream& operator<<(std::ostream& out, const CS& cs);
    friend std::istream& operator>>(std::istream& in, CS& cs);
    friend std::ofstream& operator<<(std::ofstream& out, const CS& cs);
    friend std::ifstream& operator>>(std::ifstream& in, CS& cs);

    static void resetIdCounter() { nextId = 1; }
};