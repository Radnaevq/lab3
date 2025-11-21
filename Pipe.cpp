#include "Pipe.h"
#include "utils.h"
#include <iostream>
#include <fstream>

int Pipe::nextId = 1;

Pipe::Pipe() : id(nextId++), name(""), length(0), diameter(0), inRepair(false) {}

Pipe::Pipe(const std::string& name, double length, int diameter, bool inRepair)
    : id(nextId++), name(name), length(length), diameter(diameter), inRepair(inRepair) {
}

void Pipe::displayInfo() const {
    std::cout << *this;
}

std::ostream& operator<<(std::ostream& out, const Pipe& pipe) {
    out << " Pipe ID: " << pipe.id << std::endl;
    out << "Name: " << pipe.name << std::endl;
    out << "Length: " << pipe.length << std::endl;
    out << "Diameter: " << pipe.diameter << std::endl;
    out << "Status: " << pipe.inRepair << std::endl;
    out << std::endl;
    return out;
}

std::istream& operator>>(std::istream& in, Pipe& pipe) {
    std::cout << "Insert pipe name: ";
    std::getline(in, pipe.name);
    logInput(pipe.name);

    while (pipe.name.empty()) {
        std::cout << "Error, insert pipe name: ";
        std::getline(in, pipe.name);
        logInput(pipe.name);
    }

    inputNumber(pipe.length, "Insert pipe length: ", true);
    inputNumber(pipe.diameter, "Insert pipe diameter: ", true);
    inputInRange(pipe.inRepair, "Pipe condition (0 for operational, 1 for repair): ", false, true);

    return in;
}

std::ofstream& operator<<(std::ofstream& out, const Pipe& pipe) {
    out << "PIPE" << std::endl;
    out << pipe.id << std::endl;
    out << pipe.name << std::endl;
    out << pipe.length << std::endl;
    out << pipe.diameter << std::endl;
    out << pipe.inRepair << std::endl;
    return out;
}

std::ifstream& operator>>(std::ifstream& in, Pipe& pipe) {
    in >> pipe.id;
    in.ignore();
    std::getline(in, pipe.name);
    in >> pipe.length >> pipe.diameter >> pipe.inRepair;
    in.ignore();
    return in;
}