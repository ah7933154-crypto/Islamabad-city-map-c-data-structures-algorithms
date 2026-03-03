#pragma once
#include "SmartCity.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"

using namespace std;

// ========================= INPUT VALIDATION FUNCTIONS =========================


// Helper function to check if string can be converted to double


// ========================= FACILITY STRUCTURES =========================

// Base Facility using std::string for safety and compatibility with Graph
class Facility {
public:
    string name;
    string sector;
    int nodeId;
    Facility* next;

    Facility(string n, string s, int nid)
        : name(n), sector(s), nodeId(nid), next(nullptr) {
    }

    virtual ~Facility() {} // Virtual destructor for safety
};

class Mosque : public Facility {
public:
    Mosque(string n, string s, int nid) : Facility(n, s, nid) {}
};

class Park : public Facility {
public:
    Park(string n, string s, int nid) : Facility(n, s, nid) {}
};

class WaterCooler : public Facility {
public:
    WaterCooler(string n, string s, int nid) : Facility(n, s, nid) {}
};

// ========================= PUBLIC FACILITIES MANAGER =========================

class PublicFacilities {
private:
    Facility* mosques;
    Facility* parks;
    Facility* coolers;
    Graph* graph;

    // Helper to insert into linked list
    void insertFacility(Facility*& head, Facility* f) {
        f->next = head;
        head = f;
    }

    // Find specific facility object by Node ID
    Facility* findFacilityByNodeId(Facility* head, int id) {
        Facility* cur = head;
        while (cur) {
            if (cur->nodeId == id) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    // Validate and get coordinates
    bool getValidCoordinates(double& lat, double& lon) {
        string latStr, lonStr;
        cout << "Enter Latitude (-90 to 90): ";
        getline(cin, latStr);
        cout << "Enter Longitude (-180 to 180): ";
        getline(cin, lonStr);

        if (!isValidDouble(latStr, lat) || !isValidDouble(lonStr, lon)) {
            cout << "Error: Invalid coordinate format.\n";
            return false;
        }

        if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Coordinates out of valid range.\n";
            cout << "Latitude must be between -90 and 90.\n";
            cout << "Longitude must be between -180 and 180.\n";
            return false;
        }

        return true;
    }

    // Validate and get facility type
    bool getValidFacilityType(int& typeChoice) {
        string choiceStr;
        cout << "Select Type:\n1. Mosque\n2. Park\n3. Water Cooler\nChoice: ";
        getline(cin, choiceStr);

        if (!isValidInt(choiceStr, typeChoice)) {
            cout << "Error: Invalid choice. Please enter a number (1-3).\n";
            return false;
        }

        if (!isValidChoice(typeChoice, 1, 3)) {
            cout << "Error: Choice must be between 1 and 3.\n";
            return false;
        }

        return true;
    }

    // Check if facility with same name already exists
    bool facilityExists(const string& name, Facility* head) {
        Facility* cur = head;
        while (cur) {
            if (cur->name == name) return true;
            cur = cur->next;
        }
        return false;
    }

public:
    PublicFacilities(Graph* g) : graph(g), mosques(nullptr), parks(nullptr), coolers(nullptr) {

    }

    ~PublicFacilities() {
        // Clean up linked lists
        Facility* lists[3] = { mosques, parks, coolers };
        for (int i = 0; i < 3; i++) {
            Facility* cur = lists[i];
            while (cur) {
                Facility* temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
    }

    // --- Core Functions with Validation ---

    void addMosque(string name, string sector, double lat, double lon) {
        // Validate inputs
        if (!isValidFacilityName(name)) {
            cout << "Error: Invalid mosque name.\n";
            return;
        }

        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector format. Use format like 'G-10', 'F-8'.\n";
            return;
        }

        if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates.\n";
            return;
        }

        // Check if mosque with same name already exists
        if (facilityExists(name, mosques)) {
            cout << "Error: Mosque with name '" << name << "' already exists.\n";
            return;
        }

        int id = graph->addLocation(name, lat, lon, MOSQUE);
        if (id == -1) {
            cout << "Error: Graph is full. Cannot add more facilities.\n";
            return;
        }
        insertFacility(mosques, new Mosque(name, sector, id));
        cout << "✓ Mosque added: " << name << " (ID: " << id << ")\n";
    }

    void addPark(string name, string sector, double lat, double lon) {
        // Validate inputs
        if (!isValidFacilityName(name)) {
            cout << "Error: Invalid park name.\n";
            return;
        }

        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector format. Use format like 'G-10', 'F-8'.\n";
            return;
        }

        if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates.\n";
            return;
        }

        // Check if park with same name already exists
        if (facilityExists(name, parks)) {
            cout << "Error: Park with name '" << name << "' already exists.\n";
            return;
        }

        int id = graph->addLocation(name, lat, lon, PARK);
        if (id == -1) {
            cout << "Error: Graph is full. Cannot add more facilities.\n";
            return;
        }
        insertFacility(parks, new Park(name, sector, id));
        cout << "✓ Park added: " << name << " (ID: " << id << ")\n";
    }

    void addWaterCooler(string name, string sector, double lat, double lon) {
        // Validate inputs
        if (!isValidFacilityName(name)) {
            cout << "Error: Invalid water cooler name.\n";
            return;
        }

        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector format. Use format like 'G-10', 'F-8'.\n";
            return;
        }

        if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates.\n";
            return;
        }

        // Check if water cooler with same name already exists
        if (facilityExists(name, coolers)) {
            cout << "Error: Water cooler with name '" << name << "' already exists.\n";
            return;
        }

        int id = graph->addLocation(name, lat, lon, WATER_COOLER);
        if (id == -1) {
            cout << "Error: Graph is full. Cannot add more facilities.\n";
            return;
        }
        insertFacility(coolers, new WaterCooler(name, sector, id));
        cout << "✓ Water cooler added: " << name << " (ID: " << id << ")\n";
    }

    // --- Interactive Wrappers for Main Menu ---

    void registerFacilityInteractive() {
        string name, sector;
        int typeChoice;
        double lat = 0.0, lon = 0.0;

        cout << "\n=== Register New Facility ===\n";

        // Get facility name
        cout << "Enter Facility Name: ";
        getline(cin, name);
        if (!isValidFacilityName(name)) {
            cout << "Error: Invalid facility name.\n";
            return;
        }

        // Get sector
        cout << "Enter Sector (e.g., G-10, F-8): ";
        getline(cin, sector);
        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector format. Use format like 'G-10', 'F-8'.\n";
            return;
        }

        // Get facility type
        if (!getValidFacilityType(typeChoice)) {
            return;
        }

        // Get coordinates
        if (!getValidCoordinates(lat, lon)) {
            return;
        }

        // Add facility based on type
        switch (typeChoice) {
        case 1:
            addMosque(name, sector, lat, lon);
            break;
        case 2:
            addPark(name, sector, lat, lon);
            break;
        case 3:
            addWaterCooler(name, sector, lat, lon);
            break;
        default:
            cout << "Error: Invalid type selected.\n";
        }
    }

    void findNearestFacilityInteractive() {
        int typeChoice;
        double lat = 0.0, lon = 0.0;

        cout << "\n=== Find Nearest Facility ===\n";

        // Get facility type
        if (!getValidFacilityType(typeChoice)) {
            return;
        }

        // Get coordinates
        if (!getValidCoordinates(lat, lon)) {
            return;
        }

        LocationType lType = OTHER;
        Facility* listHead = nullptr;
        string typeName = "";

        if (typeChoice == 1) {
            lType = MOSQUE;
            listHead = mosques;
            typeName = "Mosque";
        }
        else if (typeChoice == 2) {
            lType = PARK;
            listHead = parks;
            typeName = "Park";
        }
        else if (typeChoice == 3) {
            lType = WATER_COOLER;
            listHead = coolers;
            typeName = "Water Cooler";
        }
        else {
            cout << "Error: Invalid choice.\n";
            return;
        }

        // Check if any facilities of this type exist
        if (!listHead) {
            cout << "Error: No " << typeName << "s registered in the system.\n";
            return;
        }

        // Use Graph to find nearest node of that type
        int nearestNodeId = graph->findNearestNodeOfType(lat, lon, lType);

        if (nearestNodeId != -1) {
            // Retrieve Facility details using the node ID
            Facility* found = findFacilityByNodeId(listHead, nearestNodeId);
            Coordinate c = graph->getCoord(nearestNodeId);
            double dist = haversine(lat, lon, c.lat, c.lon);

            cout << "\n=== NEAREST " << typeName << " ===\n";
            cout << "Name: " << graph->getName(nearestNodeId) << "\n";
            if (found) cout << "Sector: " << found->sector << "\n";
            cout << "Location: (" << fixed << setprecision(6) << c.lat << ", " << c.lon << ")\n";
            cout << "Distance: " << fixed << setprecision(2) << dist << " km\n";
        }
        else {
            cout << "No " << typeName << " found near your location.\n";
        }
    }

    void displayFacilitiesByType() {
        cout << "\n=== PUBLIC FACILITIES REPORT ===\n";

        // Count statistics
        int mosqueCount = 0, parkCount = 0, coolerCount = 0;

        cout << "\n--- MOSQUES ---\n";
        Facility* cur = mosques;
        if (!cur) {
            cout << "No mosques registered.\n";
        }
        else {
            while (cur) {
                mosqueCount++;
                Coordinate c = graph->getCoord(cur->nodeId);
                cout << "- " << cur->name << " | Sector: " << cur->sector
                    << " | Coordinates: (" << fixed << setprecision(6)
                    << c.lat << ", " << c.lon << ")\n";
                cur = cur->next;
            }
            cout << "Total Mosques: " << mosqueCount << "\n";
        }

        cout << "\n--- PARKS ---\n";
        cur = parks;
        if (!cur) {
            cout << "No parks registered.\n";
        }
        else {
            while (cur) {
                parkCount++;
                Coordinate c = graph->getCoord(cur->nodeId);
                cout << "- " << cur->name << " | Sector: " << cur->sector
                    << " | Coordinates: (" << fixed << setprecision(6)
                    << c.lat << ", " << c.lon << ")\n";
                cur = cur->next;
            }
            cout << "Total Parks: " << parkCount << "\n";
        }

        cout << "\n--- WATER COOLERS ---\n";
        cur = coolers;
        if (!cur) {
            cout << "No water coolers registered.\n";
        }
        else {
            while (cur) {
                coolerCount++;
                Coordinate c = graph->getCoord(cur->nodeId);
                cout << "- " << cur->name << " | Sector: " << cur->sector
                    << " | Coordinates: (" << fixed << setprecision(6)
                    << c.lat << ", " << c.lon << ")\n";
                cur = cur->next;
            }
            cout << "Total Water Coolers: " << coolerCount << "\n";
        }

        cout << "\n=== SUMMARY ===\n";
        cout << "Total Facilities: " << mosqueCount + parkCount + coolerCount << "\n";
        cout << "  • Mosques: " << mosqueCount << "\n";
        cout << "  • Parks: " << parkCount << "\n";
        cout << "  • Water Coolers: " << coolerCount << "\n";
        cout << "================================\n";
    }

    // Additional function to check if system has any facilities
    bool hasFacilities() const {
        return mosques != nullptr || parks != nullptr || coolers != nullptr;
    }

    // Function to display system statistics
    void displayStatistics() {
        int mosqueCount = 0, parkCount = 0, coolerCount = 0;

        Facility* cur = mosques;
        while (cur) { mosqueCount++; cur = cur->next; }

        cur = parks;
        while (cur) { parkCount++; cur = cur->next; }

        cur = coolers;
        while (cur) { coolerCount++; cur = cur->next; }

        cout << "\n=== PUBLIC FACILITIES STATISTICS ===\n";
        cout << "Total Mosques: " << mosqueCount << "\n";
        cout << "Total Parks: " << parkCount << "\n";
        cout << "Total Water Coolers: " << coolerCount << "\n";
        cout << "Total Facilities: " << mosqueCount + parkCount + coolerCount << "\n";

        if (graph) {
            int graphNodes = graph->getNodeCount();
            int facilityNodes = mosqueCount + parkCount + coolerCount;
            cout << "Graph Capacity: " << facilityNodes << "/" << graphNodes
                << " nodes used (" << fixed << setprecision(1)
                << (graphNodes > 0 ? (facilityNodes * 100.0 / graphNodes) : 0)
                << "%)\n";
        }
        cout << "====================================\n";
    }

    // Getters for specific Logic (if needed)
    Facility* getMosques() const { return mosques; }
    Facility* getParks() const { return parks; }
    Facility* getCoolers() const { return coolers; }
};