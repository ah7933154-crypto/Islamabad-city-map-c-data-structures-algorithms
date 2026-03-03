#include <iostream>
#include <string>
#include "SmartCity.h"
#include "Population.h"
#include "Education.h"
#include "Commercial.h"
#include "Medical.h"
#include "Transport.h"
#include "PublicFacilities.h"

using namespace std;

// Validation helper functions without <limits>
bool isValidIntegerInput(int& value, int minVal, int maxVal) {
    string input;
    getline(cin, input);

    // Check if input is empty
    if (input.empty()) {
        cout << "Input cannot be empty. Please enter a number between "
            << minVal << " and " << maxVal << ".\n";
        return false;
    }

    // Check if all characters are digits or minus sign
    for (size_t i = 0; i < input.length(); i++) {
        if (i == 0 && input[i] == '-') continue; // Allow minus sign at beginning
        if (input[i] < '0' || input[i] > '9') {
            cout << "Invalid input. Please enter a number between "
                << minVal << " and " << maxVal << ".\n";
            return false;
        }
    }

    // Convert string to integer
    int result = 0;
    bool negative = false;
    int i = 0;

    if (input[0] == '-') {
        negative = true;
        i = 1;
    }

    for (; i < input.length(); i++) {
        result = result * 10 + (input[i] - '0');
    }

    if (negative) result = -result;

    // Check range
    if (result < minVal || result > maxVal) {
        cout << "Please enter a number between " << minVal << " and " << maxVal << ".\n";
        return false;
    }

    value = result;
    return true;
}

bool isValidDoubleInput(double& value, double minVal, double maxVal) {
    string input;
    getline(cin, input);

    if (input.empty()) {
        cout << "Input cannot be empty.\n";
        return false;
    }

    // Check if input is valid double format
    bool hasDecimal = false;
    bool hasDigit = false;
    int startIndex = 0;

    if (input[0] == '-') {
        startIndex = 1;
    }

    for (size_t i = startIndex; i < input.length(); i++) {
        char c = input[i];
        if (c == '.') {
            if (hasDecimal) {
                cout << "Invalid number format. Only one decimal point allowed.\n";
                return false;
            }
            hasDecimal = true;
        }
        else if (c < '0' || c > '9') {
            cout << "Invalid number format. Please enter a valid number.\n";
            return false;
        }
        else {
            hasDigit = true;
        }
    }

    if (!hasDigit) {
        cout << "Invalid number format.\n";
        return false;
    }

    // Simple string to double conversion
    double result = 0.0;
    double fraction = 0.0;
    double divisor = 1.0;
    bool negative = false;
    bool decimal = false;
    int i = 0;

    if (input[0] == '-') {
        negative = true;
        i = 1;
    }

    for (; i < input.length(); i++) {
        char c = input[i];
        if (c == '.') {
            decimal = true;
            continue;
        }

        int digit = c - '0';
        if (!decimal) {
            result = result * 10.0 + digit;
        }
        else {
            fraction = fraction * 10.0 + digit;
            divisor *= 10.0;
        }
    }

    result = result + (fraction / divisor);
    if (negative) result = -result;

    // Check range
    if (result < minVal || result > maxVal) {
        cout << "Please enter a number between " << minVal << " and " << maxVal << ".\n";
        return false;
    }

    value = result;
    return true;
}

bool isValidStringInput(string& input, int minLength = 1, int maxLength = 100) {
    getline(cin, input);

    // Remove leading/trailing whitespace
    size_t first = 0;
    size_t last = input.length() - 1;

    while (first <= last && (input[first] == ' ' || input[first] == '\t' ||
        input[first] == '\n' || input[first] == '\r')) {
        first++;
    }

    while (last >= first && (input[last] == ' ' || input[last] == '\t' ||
        input[last] == '\n' || input[last] == '\r')) {
        last--;
    }

    if (first > last) {
        input = "";
    }
    else {
        input = input.substr(first, last - first + 1);
    }

    if (input.length() < minLength || input.length() > maxLength) {
        cout << "Input must be between " << minLength << " and "
            << maxLength << " characters.\n";
        return false;
    }
    return true;
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n'); // Use large number instead of numeric_limits
}

int main() {
    Graph cityGraph;

    PopulationSector ps(&cityGraph);
    EducationSector edu(&cityGraph);
    CommercialSector commercial(&cityGraph);
    MedicalSector medical(&cityGraph);
    TransportSector transport(&cityGraph);
    PublicFacilities publicFacilities(&cityGraph);

    int mainChoice;

    do {
        cout << "\n";
        cout << "??????????????????????????????????????????????\n";
        cout << "?   SMART CITY MANAGEMENT SYSTEM - v2.0      ?\n";
        cout << "??????????????????????????????????????????????\n";
        cout << "  1. Population Sector\n";
        cout << "  2. Education Sector\n";
        cout << "  3. Commercial Sector\n";
        cout << "  4. Medical Sector\n";
        cout << "  5. Transport Sector\n";
        cout << "  6. Public Facilities Sector\n";
        cout << "  7. View System Statistics\n";
        cout << "  0. Exit\n";
        cout << "?????????????????????????????????????????????\n";
        cout << "Enter choice: ";

        // Validate main menu choice
        while (!isValidIntegerInput(mainChoice, 0, 7)) {
            cout << "Enter choice (0-7): ";
        }

        switch (mainChoice) {
        case 1: {
            int choice = 0;
            do {
                cout << "\n--- POPULATION SECTOR MENU ---\n";
                cout << "1. Citizen Management\n";
                cout << "2. Housing Management\n";
                cout << "3. Statistics & Analytics\n";
                cout << "0. Back to Main Menu\n";
                cout << "Choice: ";

                while (!isValidIntegerInput(choice, 0, 3)) {
                    cout << "Choice (0-3): ";
                }

                switch (choice) {
                case 1: { // Citizen Management
                    int citizenChoice = 0;
                    do {
                        cout << "\n--- CITIZEN MANAGEMENT ---\n";
                        cout << "1. Load population CSV\n";
                        cout << "2. Add citizen\n";
                        cout << "3. Search by CNIC\n";
                        cout << "4. Delete citizen\n";
                        cout << "0. Back\n";
                        cout << "Choice: ";

                        while (!isValidIntegerInput(citizenChoice, 0, 4)) {
                            cout << "Choice (0-4): ";
                        }

                        switch (citizenChoice) {
                        case 1: {
                            string file;
                            cout << "Enter CSV filename: ";
                            while (!isValidStringInput(file, 1, 50)) {
                                cout << "Enter CSV filename (1-50 chars): ";
                            }
                            ps.loadFromCSV(file);
                            pressEnterToContinue();
                            break;
                        }
                        case 2:
                            ps.addCitizenInteractive();
                            pressEnterToContinue();
                            break;
                        case 3:
                            ps.searchCitizenInteractive();
                            pressEnterToContinue();
                            break;
                        case 4:
                            ps.deleteCitizenInteractive();
                            pressEnterToContinue();
                            break;
                        case 0:
                            break;
                        }
                    } while (citizenChoice != 0);
                    break;
                }
                case 2: // Housing Management
                    ps.housingManagementMenu();
                    pressEnterToContinue();
                    break;
                case 3: { // Statistics & Analytics
                    int statsChoice = 0;
                    do {
                        cout << "\n--- STATISTICS & ANALYTICS ---\n";
                        cout << "1. Age distribution\n";
                        cout << "2. Occupation summary\n";
                        cout << "3. Population density\n";
                        cout << "4. Gender ratio\n";
                        cout << "5. Show all statistics\n";
                        cout << "0. Back\n";
                        cout << "Choice: ";

                        while (!isValidIntegerInput(statsChoice, 0, 5)) {
                            cout << "Choice (0-5): ";
                        }

                        switch (statsChoice) {
                        case 1:
                            ps.ageDistribution();
                            pressEnterToContinue();
                            break;
                        case 2:
                            ps.occupationSummary();
                            pressEnterToContinue();
                            break;
                        case 3:
                            ps.populationDensity();
                            pressEnterToContinue();
                            break;
                        case 4:
                            ps.genderRatio();
                            pressEnterToContinue();
                            break;
                        case 5:
                            cout << "\n=== ALL POPULATION STATISTICS ===\n";
                            ps.ageDistribution();
                            ps.occupationSummary();
                            ps.populationDensity();
                            ps.genderRatio();
                            pressEnterToContinue();
                            break;
                        case 0:
                            break;
                        }
                    } while (statsChoice != 0);
                    break;
                }
                case 0:
                    break;
                }
            } while (choice != 0);
            break;
        }
        case 2: {
            int choice;
            do {
                cout << "\n???? EDUCATION SECTOR ????\n";
                cout << "  1. Register Faculty & Class\n";
                cout << "  2. Register Student\n";
                cout << "  3. Add Department\n";
                cout << "  4. View School Organogram\n";
                cout << "  5. Search Schools by Subject\n";
                cout << "  6. Top Ranked Schools\n";
                cout << "  7. Locate Nearest School\n";
                cout << "  8. Update School Rating\n";
                cout << "  9. Load Schools from CSV\n";
                cout << " 10. Display Everything\n";
                cout << "  0. Back to main menu\n";
                cout << "?????????????????????????????\n";
                cout << "Choice: ";

                while (!isValidIntegerInput(choice, 0, 10)) {
                    cout << "Choice (0-10): ";
                }

                switch (choice) {
                case 1:
                    edu.registerFacultyAndClass();
                    pressEnterToContinue();
                    break;
                case 2:
                    edu.registerStudent();
                    pressEnterToContinue();
                    break;
                case 3:
                    edu.addDepartment();
                    pressEnterToContinue();
                    break;
                case 4:
                    edu.displayHierarchy();
                    pressEnterToContinue();
                    break;
                case 5:
                    edu.searchBySubject();
                    pressEnterToContinue();
                    break;
                case 6:
                    edu.showRankings();
                    pressEnterToContinue();
                    break;
                case 7:
                    edu.findNearestSchool();
                    pressEnterToContinue();
                    break;
                case 8:
                    edu.updateSchoolRating();
                    pressEnterToContinue();
                    break;
                case 9: {
                    string file;
                    cout << "CSV filename: ";
                    while (!isValidStringInput(file, 1, 50)) {
                        cout << "CSV filename (1-50 chars): ";
                    }
                    edu.loadFromCSV(file);
                    pressEnterToContinue();
                    break;
                }
                case 10:
                    edu.displayEverything();
                    pressEnterToContinue();
                    break;
                case 0:
                    break;
                default:
                    cout << "? Invalid choice.\n";
                    pressEnterToContinue();
                    break;
                }
            } while (choice != 0);
            break;
        }

        case 3: {
            int choice;
            do {
                cout << "\n???? COMMERCIAL SECTOR ????\n";
                cout << "  1. Register Shopping Mall\n";
                cout << "  2. Add Floor to Mall\n";
                cout << "  3. Add Shop to Floor\n";
                cout << "  4. Add Product to Shop\n";
                cout << "  5. Search Products by Category\n";
                cout << "  6. Find Nearest Mall\n";
                cout << "  7. Display All Malls\n";
                cout << "  0. Back to main menu\n";
                cout << "?????????????????????????????\n";
                cout << "Choice: ";

                while (!isValidIntegerInput(choice, 0, 7)) {
                    cout << "Choice (0-7): ";
                }

                switch (choice) {
                case 1:
                    commercial.registerMallInteractive();
                    pressEnterToContinue();
                    break;
                case 2:
                    commercial.addFloorInteractive();
                    pressEnterToContinue();
                    break;
                case 3:
                    commercial.addShopInteractive();
                    pressEnterToContinue();
                    break;
                case 4:
                    commercial.addProductInteractive();
                    pressEnterToContinue();
                    break;
                case 5:
                    commercial.searchByCategoryInteractive();
                    pressEnterToContinue();
                    break;
                case 6:
                    commercial.findNearestMallInteractive();
                    pressEnterToContinue();
                    break;
                case 7:
                    commercial.displayAll();
                    pressEnterToContinue();
                    break;
                case 0:
                    break;
                default:
                    cout << "? Invalid choice.\n";
                    pressEnterToContinue();
                    break;
                }
            } while (choice != 0);
            break;
        }

        case 4: {
            medical.medicalManagementMenu();
            pressEnterToContinue();
            break;
        }

        case 5: {
            int choice;
            do {
                cout << "\n???? TRANSPORT SECTOR ????\n";
                cout << " 1. Register Bus Stop/Station\n";
                cout << " 2. Connect Two Stops\n";
                cout << " 3. Find Nearest Stop (by coordinates)\n";
                cout << " 4. Display All Bus Stops\n";
                cout << " 5. Register Transport Company\n";
                cout << " 6. Register Bus\n";
                cout << " 7. Load Buses from CSV\n";
                cout << " 8. Start Bus Service\n";
                cout << " 9. Add Passenger to Bus\n";
                cout << " 10. Move All Buses / Simulate Time\n";
                cout << " 11. Drop off Passengers\n";
                cout << " 12. List Buses at Stop\n";
                cout << " 13. Find Shortest Path (Dijkstra)\n";
                cout << " 14. Display Bus Status\n";
                cout << " 15. Display System Statistics\n";
                cout << " 16. Load Stops from CSV\n";
                cout << " 17. Real-Time Bus Journey Simulation (BONUS FEATURE)\n";  // ← NEW!
                cout << "18. Display All Buses\n";
                cout << " 0. Back to main menu\n";
                cout << "?????????????????????????????????\n";
                cout << "Choice: ";
                while (!(cin >> choice) || choice < 0 || choice > 18) {
                    cout << "Invalid input. Choice (0-18): ";
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                cin.ignore();  // Clear newline after number input

                switch (choice) {
                case 1:
                    transport.registerStopInteractive();
                    pressEnterToContinue();
                    break;
                case 2:
                    transport.connectStopsInteractive();
                    pressEnterToContinue();
                    break;
                case 3:
                    transport.findNearestStopInteractive();
                    pressEnterToContinue();
                    break;
                case 4:
                    transport.displayAllStops();
                    pressEnterToContinue();
                    break;
                case 5:
                    transport.registerCompanyInteractive();
                    pressEnterToContinue();
                    break;
                case 6:
                    transport.registerBusInteractive();
                    pressEnterToContinue();
                    break;
                case 7: {
                    string file;
                    cout << "CSV filename: ";
                    getline(cin, file);
                    if (file.empty()) file = "buses.csv";
                    transport.loadFromFile(file);
                    pressEnterToContinue();
                    break;
                }
                case 8:
                    transport.startBusInteractive();
                    pressEnterToContinue();
                    break;
                case 9:
                    transport.addPassengerInteractive();
                    pressEnterToContinue();
                    break;
                case 10: {
                    int minutes;
                    cout << "Enter minutes to simulate: ";
                    cin >> minutes;
                    if (minutes <= 0) minutes = 5;
                    transport.simulateTimeStepWithDropOff(minutes);
                    pressEnterToContinue();
                    break;
                }
                case 11:
                    transport.dropOffAllPassengers();
                    pressEnterToContinue();
                    break;
                case 12:
                    transport.listBusesAtStopInteractive();
                    pressEnterToContinue();
                    break;
                case 13:
                    transport.findShortestPathInteractive();
                    pressEnterToContinue();
                    break;
                case 14:
                    transport.displayBusStatusInteractive();
                    pressEnterToContinue();
                    break;
                case 15:
                    transport.displaySystemStats();
                    pressEnterToContinue();
                    break;
                case 16: {
                    string file;
                    cout << "Stops CSV filename: ";
                    getline(cin, file);
                    if (file.empty()) file = "stops.csv";
                    transport.loadStopsCSV(file);
                    pressEnterToContinue();
                    break;
                }
                case 17: {  // ← NEW BONUS CASE
                    string busNo;
                    cout << "Enter Bus Number to simulate journey: ";
                    cin >> busNo;
                    transport.simulateBusJourney(busNo, 15);  // 15 steps = full visual effect
                    break;
                }
                case 18:   // ← NEW CASE
                    transport.displayAllBuses();
                    pressEnterToContinue();
                    break;
                case 0:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
                    pressEnterToContinue();
                    break;
                }
            } while (choice != 0);
            break;
        }
        case 6: {
            int choice;
            do {
                cout << "\n???? PUBLIC FACILITIES ????\n";
                cout << "  1. Register Facility (Mosque/Park/etc.)\n";
                cout << "  2. Find Nearest Facility by Type\n";
                cout << "  3. Display Facilities by Type\n";
                cout << "  4. Display All Facilities\n";
                cout << "  0. Back to main menu\n";
                cout << "????????????????????????????????\n";
                cout << "Choice: ";

                while (!isValidIntegerInput(choice, 0, 4)) {
                    cout << "Choice (0-4): ";
                }

                switch (choice) {
                case 1:
                    publicFacilities.registerFacilityInteractive();
                    pressEnterToContinue();
                    break;
                case 2:
                    publicFacilities.findNearestFacilityInteractive();
                    pressEnterToContinue();
                    break;
                case 3:
                    publicFacilities.displayFacilitiesByType();
                    pressEnterToContinue();
                    break;
                case 4:
                    cout << "\n=== ALL PUBLIC FACILITIES ===\n";
                    // Display all facilities functionality would go here
                    pressEnterToContinue();
                    break;
                case 0:
                    break;
                default:
                    cout << " Invalid choice.\n";
                    pressEnterToContinue();
                    break;
                }
            } while (choice != 0);
            break;
        }

        case 7: {
            cout << "\n???? SYSTEM STATISTICS ????\n";
            cout << "Graph Nodes: " << cityGraph.getNodeCount() << "\n";
            transport.displaySystemStats();
            pressEnterToContinue();
            break;
        }

        case 0:
            cout << "\n?????????????????????????????????????\n";
            cout << "?  Thank you for using Smart City!  ?\n";
            cout << "?????????????????????????????????????\n";
            break;

        default:
            cout << "Invalid choice. Please try again.\n";
            pressEnterToContinue();
            break;
        }
    } while (mainChoice != 0);

    return 0;
}