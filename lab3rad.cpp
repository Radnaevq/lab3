#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include "Pipe.h"
#include "CS.h"
#include "utils.h"
#include "PipeManager.h"
#include "CSManager.h"

using namespace std;

std::vector<int> foundPipes;
std::vector<int> foundStations;

void Menu(PipeManager& pipeManager, CSManager& csManager) {
    while (1)
    {
        cout << "Choose an action\n1. Add pipe\n2. Add compressor station\n3. View all objects\n4. Edit pipe\n5. Edit compressor station\n6. Search pipes\n7. Search compressor stations\n8. Batch edit pipes\n9. Delete pipe\n10. Delete compressor station\n11. Save\n12. Load\n0. Exit\n";
        string input;
        getline(cin, input);
        logInput(input);

        if (input.empty()) {
            continue;
        }

        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isNumber = false;
                break;
            }
        }

        if (!isNumber) {
            cout << "Error, please enter only numbers (0-7)." << endl;
            continue;
        }

        int option = stoi(input);
        switch (option)
        {
        case 1: {
            pipeManager.createPipe();
            cout << "Pipe added successfully!" << endl;
            break;
        }

        case 2: {
            csManager.createCS();
            cout << "Compressor station added successfully!" << endl;
            break;
        }

        case 3: {
            cout << "\n PIPES" << endl;
            pipeManager.displayAllPipes();
            cout << "\n COMPRESSOR STATIONS" << endl;
            csManager.displayAllCSs();
            break;
        }

        case 4: {
            if (pipeManager.isEmpty()) {
                cout << "No pipe to edit, please add a pipe first." << endl;
                break;
            }

            cout << "Available pipes:" << endl;
            pipeManager.displayAllPipes();

            int pipeId;
            inputNumber(pipeId, "Enter pipe ID to edit: ");

            Pipe* pipe = pipeManager.getPipe(pipeId);
            if (!pipe) {
                cout << "Pipe with ID " << pipeId << " not found!" << endl;
                break;
            }

            while (true) {
                cout << "EDIT PIPE" << endl;
                cout << "1. Change name" << endl;
                cout << "2. Change length" << endl;
                cout << "3. Change diameter" << endl;
                cout << "4. Change repair status" << endl;
                cout << "5. Back to main menu" << endl;
                cout << "Choose parameter to edit: ";

                int pipeChoice;
                cin >> pipeChoice;
                logInput(to_string(pipeChoice));

                switch (pipeChoice) {
                case 1: {
                    cout << "Current name: " << pipe->getName() << endl;
                    cout << "Enter new name: ";
                    cin.ignore();
                    string tempName;
                    getline(cin, tempName);
                    pipe->setName(tempName);
                    logInput(tempName);
                    cout << "Name updated!" << endl;
                    break;
                }
                case 2: {
                    cout << "Current length: " << pipe->getLenght() << endl;
                    float tempLenght;
                    inputNumber(tempLenght, "Enter new length: ", true);
                    pipe->setLength(tempLenght);
                    cout << "Length updated!" << endl;
                    break;
                }
                case 3: {
                    cout << "Current diameter: " << pipe->getDiametr() << endl;
                    int tempDiametr;
                    inputNumber(tempDiametr, "Enter new diameter: ", true);
                    pipe->setDiametr(tempDiametr);
                    cout << "Diameter updated!" << endl;
                    break;
                }
                case 4: {
                    cout << "Current repair status: " << (pipe->getRepair() ? "1" : "0") << endl;
                    bool tempRepair;
                    inputInRange(tempRepair, "Enter new status (0 or 1): ", false, true);
                    pipe->setRepair(tempRepair);
                    cout << "Repair status updated!" << endl;
                    break;
                }
                case 5: {
                    cout << "Returning to main menu..." << endl << endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                default:
                    cout << "Invalid option!" << endl;
                    continue;
                }
                if (pipeChoice == 5) break;
            }
            break;
        }

        case 5: {
            if (csManager.isEmpty()) {
                cout << "No compressor station to edit, please add a compressor station first." << endl;
                break;
            }
            cout << "Available compressor stations:" << endl;
            csManager.displayAllCSs();

            int csId;
            inputNumber(csId, "Enter compressor station ID to edit: ");

            CS* cs = csManager.getCS(csId);
            if (!cs) {
                cout << "Compressor station with ID " << csId << " not found!" << endl;
                break;
            }

            while (true) {
                cout << "\nEDIT COMPRESSOR STATION ID: " << cs->getId() << endl;
                cout << "1. Change name" << endl;
                cout << "2. Change total workshops" << endl;
                cout << "3. Change working workshops" << endl;
                cout << "4. Change class" << endl;
                cout << "5. Start workshop" << endl;
                cout << "6. Stop workshop" << endl;
                cout << "7. Back to main menu" << endl;
                cout << "Choose parameter to edit: ";

                int csChoice;
                cin >> csChoice;
                logInput(to_string(csChoice));

                switch (csChoice) {
                case 1: {
                    cout << "Current name: " << cs->getName() << endl;
                    cout << "Enter new name: ";
                    cin.ignore();
                    string newName;
                    getline(cin, newName);
                    cs->setName(newName);
                    logInput(newName);
                    cout << "Name updated!" << endl;
                    break;
                }
                case 2: {
                    cout << "Current total workshops: " << cs->getWorkshop() << endl;
                    int newTotal;
                    inputNumber(newTotal, "Enter new total workshops: ", true);
                    cs->setWorkshop(newTotal);
                    cout << "Total workshops updated!" << endl;
                    break;
                }
                case 3: {
                    cout << "Current working workshops: " << cs->getW_work() << endl;
                    cout << "Total workshops: " << cs->getWorkshop() << endl;
                    int newWorking;
                    while (true) {
                        inputNumber(newWorking, "Enter new working workshops: ");
                        if (newWorking < 0) {
                            cout << "Error, cannot be negative. Try again." << endl;
                            continue;
                        }
                        if (newWorking > cs->getWorkshop()) {
                            cout << "Error, cannot exceed total workshops (" << cs->getWorkshop() << "). Try again." << endl;
                            continue;
                        }
                        break;
                    }
                    cs->setW_work(newWorking);
                    cout << "Working workshops updated!" << endl;
                    break;
                }
                case 4: {
                    cout << "Current class: " << cs->getClass_cs() << endl;
                    double newClass;
                    inputNumber(newClass, "Enter new class: ", true);
                    cs->setClass_cs(newClass);
                    cout << "Class updated!" << endl;
                    break;
                }
                case 5:
                    if (cs->startWorkshop()) {
                        cout << "Workshop started, now working: " << cs->getW_work() << " of " << cs->getWorkshop() << endl;
                    }
                    else {
                        cout << "All workshops are already working!" << endl;
                    }
                    break;
                case 6:
                    if (cs->stopWorkshop()) {
                        cout << "Workshop stopped, now working: " << cs->getW_work() << " of " << cs->getWorkshop() << endl;
                    }
                    else {
                        cout << "No workshops are working!" << endl;
                    }
                    break;
                case 7:
                    cout << "Returning to main menu..." << endl << endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                default:
                    cout << "Invalid option!" << endl;
                    continue;
                }
                if (csChoice == 7) break;
            }
            break;
        }

        case 6:
            if (pipeManager.isEmpty()) {
                cout << "No pipes available for search." << endl;
                break;
            }

            cout << "Search pipes by:\n";
            cout << "1. Name\n";
            cout << "2. Repair status\n";
            cout << "Choose search type: ";

            int searchType;
            cin >> searchType;
            logInput(to_string(searchType));
            cin.ignore();

            switch (searchType) {
            case 1: {
                cout << "Enter name to search: ";
                string name;
                getline(cin, name);
                logInput(name);
                foundPipes = SearchUtils::findPipesByName(pipeManager.getPipes(), name);
                break;
            }
            case 2: {
                bool repairStatus;
                inputInRange(repairStatus, "Enter repair status (0 - operational, 1 - in repair): ", false, true);
                foundPipes = SearchUtils::findPipesByRepairStatus(pipeManager.getPipes(), repairStatus);
                break;
            }
            default:
                cout << "Invalid search type!" << endl;
                break;
            }

            if (!foundPipes.empty()) {
                cout << "Found " << foundPipes.size() << " pipes:" << endl;
                pipeManager.displayPipes(foundPipes);
            }
            else {
                cout << "No pipes found." << endl;
            }
            break;

        case 7: {
            if (csManager.isEmpty()) {
                cout << "No compressor stations available for search." << endl;
                break;
            }

            cout << "Search compressor stations by:\n";
            cout << "1. Name\n";
            cout << "2. Idle percentage\n";
            cout << "Choose search type: ";

            int searchType;
            cin >> searchType;
            logInput(to_string(searchType));
            cin.ignore();

            switch (searchType) {
            case 1: {
                cout << "Enter name to search: ";
                string name;
                getline(cin, name);
                logInput(name);
                foundStations = SearchUtils::findCSsByName(csManager.getStations(), name);
                break;
            }
            case 2: {
                double minPercent, maxPercent;
                inputNumber(minPercent, "Enter minimum idle percentage: ");
                inputNumber(maxPercent, "Enter maximum idle percentage: ");
                foundStations = SearchUtils::findCSsByIdlePercentage(csManager.getStations(), minPercent, maxPercent);
                break;
            }
            default:
                cout << "Invalid search type!" << endl;
                break;
            }

            if (!foundStations.empty()) {
                cout << "Found " << foundStations.size() << " compressor stations:" << endl;
                csManager.displayCSs(foundStations);
            }
            else {
                cout << "No compressor stations found." << endl;
            }
            break;
        }

        case 8: {
            if (foundPipes.empty()) {
                cout << "No pipes selected. Please search for pipes first (option 6)." << endl;
                break;
            }

            cout << "Found " << foundPipes.size() << " pipes. Choose editing mode:\n";
            cout << "1. Edit all found pipes\n";
            cout << "2. Select specific pipes to edit\n";
            cout << "Choose mode: ";

            int mode;
            cin >> mode;
            logInput(to_string(mode));
            cin.ignore();

            std::vector<int> pipesToEdit;

            switch (mode) {
            case 1: {
                pipesToEdit = foundPipes;
                cout << "Editing all " << pipesToEdit.size() << " pipes." << endl;
                break;
            }
            case 2: {
                cout << "Select pipes to edit (enter IDs, 0 to finish):\n";
                pipeManager.displayPipes(foundPipes);

                while (true) {
                    int selectedId;
                    inputNumber(selectedId, "Enter pipe ID to select (0 to finish): ");

                    if (selectedId == 0) {
                        break;
                    }

                    if (std::find(foundPipes.begin(), foundPipes.end(), selectedId) != foundPipes.end()) {
                        if (std::find(pipesToEdit.begin(), pipesToEdit.end(), selectedId) == pipesToEdit.end()) {
                            pipesToEdit.push_back(selectedId);
                            cout << "Pipe ID " << selectedId << " added to selection." << endl;
                        }
                        else {
                            cout << "Pipe ID " << selectedId << " already selected." << endl;
                        }
                    }
                    else {
                        cout << "Pipe ID " << selectedId << " not found in search results." << endl;
                    }
                }

                if (pipesToEdit.empty()) {
                    cout << "No pipes selected for editing." << endl;
                    break;
                }

                cout << "Selected " << pipesToEdit.size() << " pipes for editing:" << endl;
                pipeManager.displayPipes(pipesToEdit);
                break;
            }
            default:
                cout << "Invalid mode!" << endl;
                break;
            }

            if (!pipesToEdit.empty()) {
                pipeManager.batchEditPipes(pipesToEdit);
            }
            break;
        }
        case 9: {
            if (pipeManager.isEmpty()) {
                cout << "No pipes available to delete." << endl;
                break;
            }

            cout << "Available pipes:" << endl;
            pipeManager.displayAllPipes();

            int pipeId;
            inputNumber(pipeId, "Enter pipe ID to delete: ");

            if (pipeManager.deletePipe(pipeId)) {
                cout << "Pipe with ID " << pipeId << " deleted successfully!" << endl;
                auto it = std::find(foundPipes.begin(), foundPipes.end(), pipeId);
                if (it != foundPipes.end()) {
                    foundPipes.erase(it);
                }
            }
            else {
                cout << "Pipe with ID " << pipeId << " not found!" << endl;
            }
            break;
        }

        case 10: {
            if (csManager.isEmpty()) {
                cout << "No compressor stations available to delete." << endl;
                break;
            }

            cout << "Available compressor stations:" << endl;
            csManager.displayAllCSs();

            int csId;
            inputNumber(csId, "Enter compressor station ID to delete: ");

            if (csManager.deleteCS(csId)) {
                cout << "Compressor station with ID " << csId << " deleted successfully!" << endl;
                auto it = std::find(foundStations.begin(), foundStations.end(), csId);
                if (it != foundStations.end()) {
                    foundStations.erase(it);
                }
            }
            else {
                cout << "Compressor station with ID " << csId << " not found!" << endl;
            }
            break;
        }
        case 11:
        {
            string filename;
            cout << "Enter filename to save: ";
            getline(cin, filename);
            logInput(filename);

            std::ofstream clearFile(filename, std::ios::trunc);
            clearFile.close();

            pipeManager.saveToFile(filename);
            csManager.saveToFile(filename);

            cout << "Data saved successfully!" << endl;
            break;
        }

        case 12:
        {
            string filename;
            cout << "Enter filename to load: ";
            getline(cin, filename);
            logInput(filename);

            pipeManager = PipeManager();
            csManager = CSManager();
            Pipe::resetIdCounter();
            CS::resetIdCounter();

            pipeManager.loadFromFile(filename);
            csManager.loadFromFile(filename);

            cout << "Data loaded successfully!" << endl;
            break;
        }

        case 0:
            return;

        default:
            cout << "Invalid option!" << endl;

        }
    }
}

int main()
{
    PipeManager pipeManager;
    CSManager csManager;
    Menu(pipeManager, csManager);
    return 0;
}