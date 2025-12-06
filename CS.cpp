#include "CS.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "utils.h"

int CS::nextId = 1;

CS::CS()
    : id(nextId++), name(""), workshop(0), w_work(0), class_cs(0.0) {
}

CS::CS(const std::string& name, int workshop, int w_work, double class_cs)
    : id(nextId++), name(name), workshop(workshop), w_work(w_work), class_cs(class_cs) {
    if (w_work > workshop) {
        this->w_work = workshop;
    }
}

void CS::setWorkshop(int total) {
    if (total < 0) throw std::invalid_argument("Total workshops cannot be negative");
    workshop = total;
    if (w_work > workshop) {
        w_work = workshop;
    }
}

void CS::setW_work(int working) {
    if (working < 0) throw std::invalid_argument("Working workshops cannot be negative");
    if (working > workshop) throw std::invalid_argument("Working workshops cannot exceed total workshops");
    w_work = working;
}

bool CS::startWorkshop() {
    if (w_work < workshop) {
        w_work++;
        return true;
    }
    return false;
}

bool CS::stopWorkshop() {
    if (w_work > 0) {
        w_work--;
        return true;
    }
    return false;
}

void CS::displayInfo() const {
    std::cout << *this;
}

std::ostream& operator<<(std::ostream& out, const CS& cs) {
    out << "Compressor Station ID: " << cs.id << std::endl;
    out << "Name: " << cs.name << std::endl;
    out << "Total workshops: " << cs.workshop << std::endl;
    out << "Working workshops: " << cs.w_work << std::endl;
    out << "Idle workshops: " << (cs.workshop - cs.w_work) << std::endl;
    out << "Idle percentage: " << cs.getIdlePercentage() << "%" << std::endl;
    out << "Class: " << cs.class_cs << std::endl;
    out << std::endl;
    return out;
}

std::istream& operator>>(std::istream& in, CS& cs) {
    std::cout << "Insert CS name: ";
    std::getline(in, cs.name);

    while (cs.name.empty()) {
        std::cout << "Error, insert cs name: ";
        std::getline(in, cs.name);
    }

    inputNumber(cs.workshop, "Insert the number of workshops: ", true);

    while (true) {
        inputNumber(cs.w_work, "Insert the number of workshops in operation: ");
        if (cs.w_work < 0) {
            std::cout << "Error, number cannot be negative. Try again." << std::endl;
            continue;
        }
        if (cs.w_work <= cs.workshop) {
            break;
        }
        std::cout << "Error. There can be no more operating stations than there are total stations.("
            << cs.workshop << "). Try again." << std::endl;
    }

    inputNumber(cs.class_cs, "Insert CS class: ", true);

    return in;
}

std::ofstream& operator<<(std::ofstream& out, const CS& cs) {
    out << "CS" << std::endl;
    out << cs.id << std::endl;
    out << cs.name << std::endl;
    out << cs.workshop << std::endl;
    out << cs.w_work << std::endl;
    out << cs.class_cs << std::endl;
    return out;
}

std::ifstream& operator>>(std::ifstream& in, CS& cs) {
    in >> cs.id;
    in.ignore();
    std::getline(in, cs.name);
    in >> cs.workshop >> cs.w_work >> cs.class_cs;
    in.ignore();
    return in;
}