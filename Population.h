#include "SmartCity.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"

using namespace std;

// ========================= UTILITY FUNCTIONS =========================

inline int strToInt(const string& str) {
    int res = 0;
    for (size_t i = 0; i < str.length(); ++i) { //
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
        }
    }
    return res;
}



inline void trimString(string& str) {
    // Remove leading whitespace
    size_t start = 0;
    while (start < str.size() && (str[start] == ' ' || str[start] == '\t')) {
        start++;
    }

    // Remove trailing whitespace
    size_t end = str.size();
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t')) {
        end--;
    }

    str = str.substr(start, end - start);
}

// ========================= INPUT VALIDATION FUNCTIONS =========================



// ========================= FAMILY CLASS =========================

class Family {
public:
    string familyID;
    string familyName;
    string headCNIC;
    int memberCount;
    Family* next;

    Family() : memberCount(0), next(NULL) {}
    Family(const string& fid, const string& fname, const string& hcnic)
        : familyID(fid), familyName(fname), headCNIC(hcnic), memberCount(0), next(NULL) {
    }
};

// ========================= CITIZEN CLASS =========================

class Citizen {
public:
    string cnic;
    string name;
    int age;
    string sector;
    int street;
    int houseNo;
    string occupation;
    string gender;
    string familyID;
    Citizen* next;

    Citizen() : age(0), street(0), houseNo(0), next(NULL) {}

    Citizen(const string& cnic_, const string& name_, int age_, const string& sector_,
        int street_, int houseNo_, const string& occupation_, const string& gender_,
        const string& familyID_ = "")
        : cnic(cnic_), name(name_), age(age_), sector(sector_), street(street_),
        houseNo(houseNo_), occupation(occupation_), gender(gender_),
        familyID(familyID_), next(NULL) {
    }
};

// ========================= HASH TABLE FOR CITIZENS =========================

class HashNode {
public:
    string key;
    Citizen* person;
    HashNode* next;
    HashNode(const string& k, Citizen* p) : key(k), person(p), next(NULL) {}
};

class CitizenHashTable {
private:
    HashNode** table;
    int size;
    int count;

    unsigned long long rawHash(const string& key) const {
        unsigned long long h = 1469598103934665603ULL;
        for (size_t i = 0; i < key.size(); i++) {
            h ^= (unsigned long long)(unsigned char)key[i];
            h *= 1099511628211ULL;
        }
        return h;
    }

    int indexForKey(const string& key) const {
        if (size == 0) return 0;
        return (int)(rawHash(key) % (unsigned long long)size);
    }

    void resize() {
        int oldSize = size;
        HashNode** oldTable = table;
        size = (size == 0) ? 2 : size * 2;
        table = new HashNode * [size];
        for (int i = 0; i < size; ++i) table[i] = NULL;
        count = 0;

        for (int i = 0; i < oldSize; ++i) {
            HashNode* cur = oldTable[i];
            while (cur) {
                insert(cur->key, cur->person);
                HashNode* toDel = cur;
                cur = cur->next;
                delete toDel;
            }
        }
        delete[] oldTable;
    }

public:
    CitizenHashTable() : table(NULL), size(0), count(0) {}

    ~CitizenHashTable() {
        if (table) {
            for (int i = 0; i < size; ++i) {
                HashNode* cur = table[i];
                while (cur) {
                    HashNode* tmp = cur;
                    cur = cur->next;
                    delete tmp;
                }
            }
            delete[] table;
        }
    }

    void insert(const string& key, Citizen* c) {
        if (table == NULL) {
            size = 2;
            table = new HashNode * [size];
            for (int i = 0; i < size; ++i) table[i] = NULL;
        }
        if ((double)(count + 1) / (double)size > 0.75) {
            resize();
        }
        int idx = indexForKey(key);
        HashNode* node = new HashNode(key, c);
        node->next = table[idx];
        table[idx] = node;
        ++count;
    }

    Citizen* search(const string& key) const {
        if (table == NULL) return NULL;
        int idx = indexForKey(key);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == key) return cur->person;
            cur = cur->next;
        }
        return NULL;
    }

    bool remove(const string& key) {
        if (table == NULL) return false;
        int idx = indexForKey(key);
        HashNode* cur = table[idx];
        HashNode* prev = NULL;
        while (cur) {
            if (cur->key == key) {
                if (prev == NULL) table[idx] = cur->next;
                else prev->next = cur->next;
                delete cur;
                --count;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    int getCount() const { return count; }
};

// ========================= HOUSING TREE =========================

class FamilyNode {
public:
    Family* familyInfo;
    Citizen* individualsHead;
    FamilyNode* next;
    FamilyNode(Family* fam) : familyInfo(fam), individualsHead(NULL), next(NULL) {}
};

class HouseNode {
public:
    int houseNo;
    string houseType; // "Apartment", "Bungalow", "Townhouse", etc.
    FamilyNode* familyHead;
    HouseNode* next;
    HouseNode(int h, const string& type = "Residential") : houseNo(h), houseType(type), familyHead(NULL), next(NULL) {}
};

class StreetNode {
public:
    int streetNo;
    string streetName;
    HouseNode* houseHead;
    StreetNode* next;
    StreetNode(int s, const string& name = "") : streetNo(s), streetName(name), houseHead(NULL), next(NULL) {}
};

class SectorNode {
public:
    string name;
    string sectorType; // "Residential", "Commercial", "Mixed", etc.
    float area; // in square km
    StreetNode* streetHead;
    SectorNode* next;
    SectorNode(const string& n, const string& type = "Residential", float a = 1.0f)
        : name(n), sectorType(type), area(a), streetHead(NULL), next(NULL) {
    }
};

class CityHousingTree {
public:
    SectorNode* head;

    CityHousingTree() : head(NULL) {}

    ~CityHousingTree() {
        SectorNode* s = head;
        while (s) {
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        FamilyNode* ftmp = f;
                        f = f->next;
                        delete ftmp;
                    }
                    HouseNode* htmp = h;
                    h = h->next;
                    delete htmp;
                }
                StreetNode* sttmp = st;
                st = st->next;
                delete sttmp;
            }
            SectorNode* stmp = s;
            s = s->next;
            delete stmp;
        }
    }

    SectorNode* getOrCreateSector(const string& sector, const string& type = "Residential", float area = 1.0f) {
        SectorNode* cur = head;
        while (cur) {
            if (cur->name == sector) return cur;
            cur = cur->next;
        }
        SectorNode* node = new SectorNode(sector, type, area);
        node->next = head;
        head = node;
        return node;
    }

    StreetNode* getOrCreateStreet(SectorNode* sec, int street, const string& name = "") {
        StreetNode* cur = sec->streetHead;
        while (cur) {
            if (cur->streetNo == street) return cur;
            cur = cur->next;
        }
        StreetNode* node = new StreetNode(street, name);
        node->next = sec->streetHead;
        sec->streetHead = node;
        return node;
    }

    HouseNode* getOrCreateHouse(StreetNode* st, int houseNo, const string& type = "Residential") {
        HouseNode* cur = st->houseHead;
        while (cur) {
            if (cur->houseNo == houseNo) return cur;
            cur = cur->next;
        }
        HouseNode* node = new HouseNode(houseNo, type);
        node->next = st->houseHead;
        st->houseHead = node;
        return node;
    }

    FamilyNode* getOrCreateFamily(HouseNode* house, Family* family) {
        FamilyNode* cur = house->familyHead;
        while (cur) {
            if (cur->familyInfo->familyID == family->familyID) return cur;
            cur = cur->next;
        }
        FamilyNode* node = new FamilyNode(family);
        node->next = house->familyHead;
        house->familyHead = node;
        return node;
    }

    void insertCitizenInTree(Citizen* c, Family* family = NULL) {
        SectorNode* sec = getOrCreateSector(c->sector);
        StreetNode* st = getOrCreateStreet(sec, c->street);
        HouseNode* house = getOrCreateHouse(st, c->houseNo);

        if (family) {
            FamilyNode* fam = getOrCreateFamily(house, family);
            c->next = fam->individualsHead;
            fam->individualsHead = c;
            fam->familyInfo->memberCount++;
        }
        else {
            // Create a default family based on CNIC
            string familyID = "FAM_" + c->cnic.substr(0, 5);
            Family* defaultFamily = new Family(familyID, c->name + " Family", c->cnic);
            FamilyNode* fam = getOrCreateFamily(house, defaultFamily);
            c->next = fam->individualsHead;
            c->familyID = familyID;
            fam->individualsHead = c;
            fam->familyInfo->memberCount++;
        }
    }

    void displayHierarchy() const {
        if (!head) {
            cout << "No sectors defined yet.\n";
            return;
        }

        SectorNode* sec = head;
        while (sec) {
            cout << "\n=== SECTOR: " << sec->name << " ===" << endl;
            cout << "Type: " << sec->sectorType << ", Area: " << sec->area << " sq km\n";

            StreetNode* st = sec->streetHead;
            if (!st) {
                cout << "  No streets in this sector.\n";
            }
            else {
                while (st) {
                    cout << "\n  Street #" << st->streetNo;
                    if (!st->streetName.empty()) cout << " (" << st->streetName << ")";
                    cout << ":\n";

                    HouseNode* h = st->houseHead;
                    if (!h) {
                        cout << "    No houses on this street.\n";
                    }
                    else {
                        while (h) {
                            cout << "    House #" << h->houseNo << " [" << h->houseType << "]:\n";

                            FamilyNode* f = h->familyHead;
                            if (!f) {
                                cout << "      No families in this house.\n";
                            }
                            else {
                                while (f) {
                                    cout << "      Family: " << f->familyInfo->familyName
                                        << " (ID: " << f->familyInfo->familyID
                                        << ", Members: " << f->familyInfo->memberCount << ")\n";

                                    Citizen* c = f->individualsHead;
                                    while (c) {
                                        cout << "        - " << c->name << " (" << c->cnic
                                            << ", " << c->age << " years, " << c->occupation << ")\n";
                                        c = c->next;
                                    }
                                    f = f->next;
                                }
                            }
                            h = h->next;
                        }
                    }
                    st = st->next;
                }
            }
            sec = sec->next;
        }
    }

    int getTotalPopulation() const {
        int total = 0;
        SectorNode* sec = head;
        while (sec) {
            StreetNode* st = sec->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* c = f->individualsHead;
                        while (c) {
                            total++;
                            c = c->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            sec = sec->next;
        }
        return total;
    }
};

// ========================= AGGREGATOR STRUCTURES =========================

struct KeyCountNode {
    string key;
    int count;
    KeyCountNode* next;
    KeyCountNode(const string& k) : key(k), count(1), next(NULL) {}
};

inline void addOrIncrement(KeyCountNode*& head, const string& key) {
    KeyCountNode* cur = head;
    while (cur) {
        if (cur->key == key) { cur->count++; return; }
        cur = cur->next;
    }
    KeyCountNode* n = new KeyCountNode(key);
    n->next = head;
    head = n;
}

// ========================= POPULATION SECTOR =========================

class PopulationSector {
private:
    CitizenHashTable hashTable;
    CityHousingTree tree;
    Citizen* allCitizensHead;
    Graph* graph;

    // Family management
    Family* familiesHead;

    void addToAllCitizens(Citizen* c) {
        c->next = allCitizensHead;
        allCitizensHead = c;
    }

    Family* findFamily(const string& familyID) {
        Family* cur = familiesHead;
        while (cur) {
            if (cur->familyID == familyID) return cur;
            cur = cur->next;
        }
        return NULL;
    }

    Family* createFamily(const string& familyID, const string& familyName, const string& headCNIC) {
        Family* newFamily = new Family(familyID, familyName, headCNIC);
        newFamily->next = familiesHead;
        familiesHead = newFamily;
        return newFamily;
    }

    // Helper function for input validation
    bool getValidInt(int& value, const string& prompt) {
        cout << prompt;
        string input;
        getline(cin, input);

        if (input.empty()) return false;

        // Check if all characters are digits
        for (size_t i = 0; i < input.length(); ++i) {
            if (input[i] < '0' || input[i] > '9') {
                cout << "Error: Please enter numbers only.\n";
                return false;
            }
        }

        value = strToInt(input);
        return true;
    }

    bool getValidFloat(float& value, const string& prompt) {
        cout << prompt;
        string input;
        getline(cin, input);

        if (input.empty()) return false;

        bool hasDecimal = false;
        for (size_t i = 0; i < input.length(); ++i) {
            char c = input[i];
            if (c == '.') {
                if (hasDecimal) {
                    cout << "Error: Multiple decimal points.\n";
                    return false;
                }
                hasDecimal = true;
            }
            else if (c < '0' || c > '9') {
                cout << "Error: Invalid number format.\n";
                return false;
            }
        }

        stringstream ss(input);
        ss >> value;
        return !ss.fail();
    }

public:
    PopulationSector(Graph* g) : allCitizensHead(NULL), graph(g), familiesHead(NULL) {}

    ~PopulationSector() {
        // Delete all citizens
        Citizen* cur = allCitizensHead;
        while (cur) {
            Citizen* tmp = cur;
            cur = cur->next;
            delete tmp;
        }

        // Delete all families
        Family* fcur = familiesHead;
        while (fcur) {
            Family* ftmp = fcur;
            fcur = fcur->next;
            delete ftmp;
        }
    }

    // ==================== NEW FUNCTIONALITY: SECTOR/STREET/HOUSE MANAGEMENT ====================

    void addSectorInteractive() {
        string name, type;
        float area;

        cout << "Enter sector name: ";
        cin.ignore();
        getline(cin, name);

        if (!isValidSector(name)) {
            cout << "Error: Invalid sector name.\n";
            return;
        }

        cout << "Enter sector type (Residential/Commercial/Mixed/Industrial): ";
        getline(cin, type);

        if (!isValidSectorType(type)) {
            cout << "Error: Invalid sector type.\n";
            return;
        }

        cout << "Enter area in square km: ";
        string areaStr;
        getline(cin, areaStr);

        if (!getValidFloat(area, "")) {
            cout << "Error: Invalid area value.\n";
            return;
        }

        if (!isValidArea(area)) {
            cout << "Error: Area must be between 0 and 1000 sq km.\n";
            return;
        }

        tree.getOrCreateSector(name, type, area);
        cout << "Sector '" << name << "' added successfully!\n";
    }

    void addStreetInteractive() {
        string sectorName, streetName;
        int streetNo;

        cout << "Enter sector name: ";
        cin.ignore();
        getline(cin, sectorName);

        if (!isValidSector(sectorName)) {
            cout << "Error: Invalid sector name.\n";
            return;
        }

        if (!getValidInt(streetNo, "Enter street number: ")) {
            cout << "Error: Invalid street number.\n";
            return;
        }

        if (!isValidStreet(streetNo)) {
            cout << "Error: Street number must be between 1 and 9999.\n";
            return;
        }

        cout << "Enter street name (optional, press Enter to skip): ";
        getline(cin, streetName);

        if (!streetName.empty() && !isValidStreetName(streetName)) {
            cout << "Error: Invalid street name.\n";
            return;
        }

        SectorNode* sector = tree.getOrCreateSector(sectorName);
        tree.getOrCreateStreet(sector, streetNo, streetName);
        cout << "Street #" << streetNo << " added to sector '" << sectorName << "' successfully!\n";
    }

    void addHouseInteractive() {
        string sectorName, houseType;
        int streetNo, houseNo;

        cout << "Enter sector name: ";
        cin.ignore();
        getline(cin, sectorName);

        if (!isValidSector(sectorName)) {
            cout << "Error: Invalid sector name.\n";
            return;
        }

        if (!getValidInt(streetNo, "Enter street number: ")) {
            cout << "Error: Invalid street number.\n";
            return;
        }

        if (!isValidStreet(streetNo)) {
            cout << "Error: Street number must be between 1 and 9999.\n";
            return;
        }

        if (!getValidInt(houseNo, "Enter house number: ")) {
            cout << "Error: Invalid house number.\n";
            return;
        }

        if (!isValidHouseNo(houseNo)) {
            cout << "Error: House number must be between 1 and 99999.\n";
            return;
        }

        cout << "Enter house type (Residential/Apartment/Bungalow/Townhouse): ";
        getline(cin, houseType);

        if (!isValidHouseType(houseType)) {
            cout << "Error: Invalid house type.\n";
            return;
        }

        SectorNode* sector = tree.getOrCreateSector(sectorName);
        StreetNode* street = tree.getOrCreateStreet(sector, streetNo);
        tree.getOrCreateHouse(street, houseNo, houseType);
        cout << "House #" << houseNo << " added to street #" << streetNo
            << " in sector '" << sectorName << "' successfully!\n";
    }

    void addFamilyInteractive() {
        string familyID, familyName, headCNIC, sectorName;
        int streetNo, houseNo;

        cout << "Enter family ID: ";
        cin.ignore();
        getline(cin, familyID);

        if (!isValidFamilyID(familyID)) {
            cout << "Error: Invalid family ID.\n";
            return;
        }

        cout << "Enter family name: ";
        getline(cin, familyName);

        if (!isValidName(familyName)) {
            cout << "Error: Invalid family name.\n";
            return;
        }

        cout << "Enter head of family CNIC (format: XXXXX-XXXXXXX-X): ";
        getline(cin, headCNIC);

        if (!isValidCNIC(headCNIC)) {
            cout << "Error: Invalid CNIC format.\n";
            return;
        }

        cout << "Enter sector name where family resides: ";
        getline(cin, sectorName);

        if (!isValidSector(sectorName)) {
            cout << "Error: Invalid sector name.\n";
            return;
        }

        if (!getValidInt(streetNo, "Enter street number: ")) {
            cout << "Error: Invalid street number.\n";
            return;
        }

        if (!isValidStreet(streetNo)) {
            cout << "Error: Street number must be between 1 and 9999.\n";
            return;
        }

        if (!getValidInt(houseNo, "Enter house number: ")) {
            cout << "Error: Invalid house number.\n";
            return;
        }

        if (!isValidHouseNo(houseNo)) {
            cout << "Error: House number must be between 1 and 99999.\n";
            return;
        }

        // Create family
        Family* family = createFamily(familyID, familyName, headCNIC);

        // Add to housing tree
        SectorNode* sector = tree.getOrCreateSector(sectorName);
        StreetNode* street = tree.getOrCreateStreet(sector, streetNo);
        HouseNode* house = tree.getOrCreateHouse(street, houseNo);
        tree.getOrCreateFamily(house, family);

        cout << "Family '" << familyName << "' registered successfully!\n";
    }

    void displayHousingInfo() {
        cout << "\n=== CITY HOUSING INFORMATION ===\n";
        tree.displayHierarchy();
        cout << "\nTotal Population: " << tree.getTotalPopulation() << " citizens\n";
        cout << "Total Citizens in Database: " << hashTable.getCount() << "\n";
    }

    void saveHousingInfoToFile() {
        ofstream fout("housing_info.txt");
        if (!fout.is_open()) {
            cout << "Error opening file for writing.\n";
            return;
        }

        fout << "=== CITY HOUSING INFORMATION ===\n\n";

        SectorNode* sec = tree.head;
        while (sec) {
            fout << "SECTOR: " << sec->name << "\n";
            fout << "Type: " << sec->sectorType << ", Area: " << sec->area << " sq km\n";

            StreetNode* st = sec->streetHead;
            while (st) {
                fout << "  Street #" << st->streetNo;
                if (!st->streetName.empty()) fout << " (" << st->streetName << ")";
                fout << ":\n";

                HouseNode* h = st->houseHead;
                while (h) {
                    fout << "    House #" << h->houseNo << " [" << h->houseType << "]:\n";

                    FamilyNode* f = h->familyHead;
                    while (f) {
                        fout << "      Family: " << f->familyInfo->familyName
                            << " (ID: " << f->familyInfo->familyID
                            << ", Members: " << f->familyInfo->memberCount << ")\n";

                        Citizen* c = f->individualsHead;
                        while (c) {
                            fout << "        - " << c->name << " (CNIC: " << c->cnic
                                << ", Age: " << c->age << ", " << c->occupation
                                << ", Gender: " << c->gender << ")\n";
                            c = c->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            fout << "\n";
            sec = sec->next;
        }

        fout << "=== SUMMARY ===\n";
        fout << "Total Sectors: " << countSectors() << "\n";
        fout << "Total Streets: " << countStreets() << "\n";
        fout << "Total Houses: " << countHouses() << "\n";
        fout << "Total Families: " << countFamilies() << "\n";
        fout << "Total Population: " << tree.getTotalPopulation() << "\n";

        fout.close();
        cout << "Housing information saved to 'housing_info.txt'\n";
    }

    int countSectors() const {
        int count = 0;
        SectorNode* sec = tree.head;
        while (sec) {
            count++;
            sec = sec->next;
        }
        return count;
    }

    int countStreets() const {
        int count = 0;
        SectorNode* sec = tree.head;
        while (sec) {
            StreetNode* st = sec->streetHead;
            while (st) {
                count++;
                st = st->next;
            }
            sec = sec->next;
        }
        return count;
    }

    int countHouses() const {
        int count = 0;
        SectorNode* sec = tree.head;
        while (sec) {
            StreetNode* st = sec->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    count++;
                    h = h->next;
                }
                st = st->next;
            }
            sec = sec->next;
        }
        return count;
    }

    int countFamilies() const {
        int count = 0;
        SectorNode* sec = tree.head;
        while (sec) {
            StreetNode* st = sec->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        count++;
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            sec = sec->next;
        }
        return count;
    }

    // ==================== EXISTING FUNCTIONALITY (UPDATED) ====================

    void addCitizen(const string& cnic, const string& name, int age, const string& sector,
        int street, int houseNo, const string& occupation, string gender,
        const string& familyID = "") {

        if (hashTable.search(cnic) != NULL) {
            cout << "Error: Citizen with this CNIC already exists!" << endl;
            return;
        }
        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format! Must be in the form XXXXX-XXXXXXX-X." << endl;
            return;
        }
        if (!isValidName(name)) {
            cout << "Error: Invalid name format!" << endl;
            return;
        }
        if (!isValidAge(age)) {
            cout << "Error: Age must be between 0 and 120." << endl;
            return;
        }
        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector!" << endl;
            return;
        }
        if (!isValidStreet(street)) {
            cout << "Error: Street number must be between 1 and 9999." << endl;
            return;
        }
        if (!isValidHouseNo(houseNo)) {
            cout << "Error: House number must be between 1 and 99999." << endl;
            return;
        }
        if (!isValidOccupation(occupation)) {
            cout << "Error: Invalid occupation format!" << endl;
            return;
        }
        if (!isValidGender(gender)) {
            cout << "Error: Invalid gender! Only Male/Female variants are allowed." << endl;
            return;
        }
        if (!familyID.empty() && !isValidFamilyID(familyID)) {
            cout << "Error: Invalid family ID format!" << endl;
            return;
        }

        Family* family = NULL;
        if (!familyID.empty()) {
            family = findFamily(familyID);
            if (!family) {
                cout << "Warning: Family ID not found. Creating individual record only.\n";
            }
        }

        Citizen* c = new Citizen(cnic, name, age, sector, street, houseNo, occupation, gender, familyID);
        addToAllCitizens(c);
        hashTable.insert(c->cnic, c);
        tree.insertCitizenInTree(c, family);
        cout << "Citizen added successfully!" << endl;
    }

    Citizen* searchByCNIC(const string& cnic) {
        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format for search!" << endl;
            return NULL;
        }
        return hashTable.search(cnic);
    }

    bool deleteByCNIC(const string& cnic) {
        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format for deletion!" << endl;
            return false;
        }

        Citizen* target = hashTable.search(cnic);
        if (!target) return false;

        bool removed = hashTable.remove(cnic);
        if (!removed) return false;

        // Remove from tree and allCitizens list
        SectorNode* s = tree.head;
        while (s) {
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* prev = NULL;
                        Citizen* cur = f->individualsHead;
                        while (cur) {
                            if (cur == target) {
                                if (prev == NULL) f->individualsHead = cur->next;
                                else prev->next = cur->next;

                                // Update family member count
                                if (f->familyInfo) {
                                    f->familyInfo->memberCount--;
                                }

                                // Remove from allCitizens list
                                Citizen* pprev = NULL;
                                Citizen* pcur = allCitizensHead;
                                while (pcur) {
                                    if (pcur == target) {
                                        if (pprev == NULL) allCitizensHead = pcur->next;
                                        else pprev->next = pcur->next;
                                        break;
                                    }
                                    pprev = pcur;
                                    pcur = pcur->next;
                                }
                                delete target;
                                return true;
                            }
                            prev = cur;
                            cur = cur->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            s = s->next;
        }
        return false;
    }

    bool loadFromCSV(const string& filename) {
        ifstream fin(filename.c_str());
        if (!fin.is_open()) {
            cout << "Error opening file: " << filename << endl;
            return false;
        }

        string line;
        bool headerSkipped = false;
        int loaded = 0;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            if (!headerSkipped) {
                headerSkipped = true;
                continue;
            }

            // Parse CSV line without using vector
            string fields[8]; // Maximum 8 fields
            int fieldCount = 0;
            size_t start = 0;
            size_t end = 0;

            while (fieldCount < 8 && start < line.length()) {
                end = line.find(',', start);
                if (end == string::npos) end = line.length();

                string field = line.substr(start, end - start);
                trimString(field);
                fields[fieldCount] = field;
                fieldCount++;

                start = end + 1;
            }

            if (fieldCount < 7) continue;
            if (!isValidCNIC(fields[0])) continue;
            if (!isAllDigits(fields[2])) continue;
            if (!isAllDigits(fields[4])) continue;
            if (!isAllDigits(fields[5])) continue;

            int age = strToInt(fields[2]);
            string sector = fields[3];
            if (!isValidSector(sector)) continue;
            int street = strToInt(fields[4]);
            if (!isValidStreet(street)) continue;
            int houseNo = strToInt(fields[5]);
            if (!isValidHouseNo(houseNo)) continue;
            if (!isValidOccupation(fields[6])) continue;

            string gender = "Male";
            if (!fields[0].empty()) {
                char lastDigit = fields[0].back();
                if (lastDigit >= '0' && lastDigit <= '9') {
                    if ((lastDigit - '0') % 2 == 0) gender = "Female";
                }
            }

            string familyID = (fieldCount > 7) ? fields[7] : "";
            if (!familyID.empty() && !isValidFamilyID(familyID)) continue;

            addCitizen(fields[0], fields[1], age, sector, street, houseNo, fields[6], gender, familyID);
            ++loaded;
        }
        fin.close();
        cout << "Loaded " << loaded << " citizens from CSV.\n";
        return true;
    }

    void ageDistribution() {
        int a0_17 = 0, a18_35 = 0, a36_60 = 0, a61p = 0;
        SectorNode* s = tree.head;
        while (s) {
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* cur = f->individualsHead;
                        while (cur) {
                            if (cur->age <= 17) a0_17++;
                            else if (cur->age <= 35) a18_35++;
                            else if (cur->age <= 60) a36_60++;
                            else a61p++;
                            cur = cur->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            s = s->next;
        }
        int total = a0_17 + a18_35 + a36_60 + a61p;
        cout << "\n=== AGE DISTRIBUTION ===\n";
        cout << "0-17: " << a0_17 << " (" << (total > 0 ? (a0_17 * 100 / total) : 0) << "%)\n";
        cout << "18-35: " << a18_35 << " (" << (total > 0 ? (a18_35 * 100 / total) : 0) << "%)\n";
        cout << "36-60: " << a36_60 << " (" << (total > 0 ? (a36_60 * 100 / total) : 0) << "%)\n";
        cout << "61+: " << a61p << " (" << (total > 0 ? (a61p * 100 / total) : 0) << "%)\n";
        cout << "Total: " << total << " citizens\n";
    }

    void occupationSummary() {
        KeyCountNode* head = NULL;
        int total = 0;

        SectorNode* s = tree.head;
        while (s) {
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* cur = f->individualsHead;
                        while (cur) {
                            addOrIncrement(head, cur->occupation);
                            total++;
                            cur = cur->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            s = s->next;
        }

        cout << "\n=== OCCUPATION SUMMARY ===\n";
        KeyCountNode* cur = head;
        while (cur) {
            cout << cur->key << ": " << cur->count << " ("
                << (total > 0 ? (cur->count * 100 / total) : 0) << "%)\n";
            KeyCountNode* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
        cout << "Total employed: " << total << "\n";
    }

    void populationDensity() {
        KeyCountNode* head = NULL;
        int totalPopulation = 0;

        SectorNode* s = tree.head;
        while (s) {
            int sectCount = 0;
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* cur = f->individualsHead;
                        while (cur) { sectCount++; cur = cur->next; }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            totalPopulation += sectCount;
            float density = (s->area > 0) ? (sectCount / s->area) : sectCount;

            KeyCountNode* node = new KeyCountNode(s->name);
            node->count = sectCount;
            node->next = head;
            head = node;

            cout << "Sector " << s->name << ": " << sectCount << " people, Density: "
                << density << " people/sq km\n";
            s = s->next;
        }

        cout << "\n=== POPULATION DENSITY SUMMARY ===\n";
        KeyCountNode* cur = head;
        while (cur) {
            cout << cur->key << ": " << cur->count << "\n";
            KeyCountNode* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
        cout << "Total city population: " << totalPopulation << "\n";
    }

    void genderRatio() {
        int male = 0, female = 0;
        SectorNode* s = tree.head;
        while (s) {
            StreetNode* st = s->streetHead;
            while (st) {
                HouseNode* h = st->houseHead;
                while (h) {
                    FamilyNode* f = h->familyHead;
                    while (f) {
                        Citizen* cur = f->individualsHead;
                        while (cur) {
                            if (cur->gender == "Male") male++;
                            else if (cur->gender == "Female") female++;
                            cur = cur->next;
                        }
                        f = f->next;
                    }
                    h = h->next;
                }
                st = st->next;
            }
            s = s->next;
        }
        int total = male + female;
        cout << "\n=== GENDER RATIO ===\n";
        cout << "Male: " << male << " (" << (total > 0 ? (male * 100 / total) : 0) << "%)\n";
        cout << "Female: " << female << " (" << (total > 0 ? (female * 100 / total) : 0) << "%)\n";
        cout << "Total: " << total << "\n";
    }

    void addCitizenInteractive() {
        string cnic, name, sector, occupation, gender, familyID;
        int age, street, houseNo;

        cout << "Enter CNIC (format: XXXXX-XXXXXXX-X): ";
        cin >> cnic;
        cin.ignore();

        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format.\n";
            return;
        }

        cout << "Enter Name: ";
        getline(cin, name);

        if (!isValidName(name)) {
            cout << "Error: Invalid name format.\n";
            return;
        }

        if (!getValidInt(age, "Enter Age: ")) {
            cout << "Error: Invalid age.\n";
            return;
        }

        if (!isValidAge(age)) {
            cout << "Error: Age must be between 0 and 120.\n";
            return;
        }

        cout << "Enter Sector: ";
        getline(cin, sector);

        if (!isValidSector(sector)) {
            cout << "Error: Invalid sector.\n";
            return;
        }

        if (!getValidInt(street, "Enter Street Number: ")) {
            cout << "Error: Invalid street number.\n";
            return;
        }

        if (!isValidStreet(street)) {
            cout << "Error: Street number must be between 1 and 9999.\n";
            return;
        }

        if (!getValidInt(houseNo, "Enter House Number: ")) {
            cout << "Error: Invalid house number.\n";
            return;
        }

        if (!isValidHouseNo(houseNo)) {
            cout << "Error: House number must be between 1 and 99999.\n";
            return;
        }

        cout << "Enter Occupation: ";
        getline(cin, occupation);

        if (!isValidOccupation(occupation)) {
            cout << "Error: Invalid occupation.\n";
            return;
        }

        cout << "Enter Gender (Male/Female): ";
        getline(cin, gender);

        if (!isValidGender(gender)) {
            cout << "Error: Invalid gender.\n";
            return;
        }

        cout << "Enter Family ID (optional, press Enter to skip): ";
        getline(cin, familyID);

        if (!familyID.empty() && !isValidFamilyID(familyID)) {
            cout << "Error: Invalid family ID format.\n";
            return;
        }

        addCitizen(cnic, name, age, sector, street, houseNo, occupation, gender, familyID);
    }

    void searchCitizenInteractive() {
        string cnic;
        cout << "Enter CNIC to search: ";
        cin >> cnic;
        cin.ignore();

        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format.\n";
            return;
        }

        Citizen* c = searchByCNIC(cnic);
        if (c) {
            cout << "\n=== CITIZEN FOUND ===\n";
            printCitizen(c);

            // Show family information if available
            if (!c->familyID.empty()) {
                Family* family = findFamily(c->familyID);
                if (family) {
                    cout << "Family: " << family->familyName << " (ID: " << family->familyID << ")\n";
                }
            }
        }
        else {
            cout << "Citizen not found.\n";
        }
    }

    void deleteCitizenInteractive() {
        string cnic;
        cout << "Enter CNIC to delete: ";
        cin >> cnic;
        cin.ignore();

        if (!isValidCNIC(cnic)) {
            cout << "Error: Invalid CNIC format.\n";
            return;
        }

        if (deleteByCNIC(cnic)) {
            cout << "Citizen deleted successfully.\n";
        }
        else {
            cout << "Citizen not found.\n";
        }
    }

    void printCitizen(const Citizen* c) const {
        if (!c) { cout << "NULL\n"; return; }
        cout << "CNIC: " << c->cnic << "\n";
        cout << "Name: " << c->name << "\n";
        cout << "Age: " << c->age << "\n";
        cout << "Address: Sector " << c->sector << ", Street " << c->street
            << ", House #" << c->houseNo << "\n";
        cout << "Occupation: " << c->occupation << "\n";
        cout << "Gender: " << c->gender << "\n";
        if (!c->familyID.empty()) {
            cout << "Family ID: " << c->familyID << "\n";
        }
    }

    // ==================== NEW MENU FUNCTION ====================

    void housingManagementMenu() {
        int choice;
        do {
            cout << "\n--- HOUSING MANAGEMENT MENU ---\n";
            cout << "1. Add Sector\n";
            cout << "2. Add Street\n";
            cout << "3. Add House\n";
            cout << "4. Add Family\n";
            cout << "5. Display Housing Hierarchy\n";
            cout << "6. Save Housing Info to File\n";
            cout << "7. Show Statistics\n";
            cout << "0. Back to Main Menu\n";
            cout << "Choice: ";

            string choiceStr;
            cin >> choiceStr;

            if (!isAllDigits(choiceStr)) {
                cout << "Error: Invalid choice. Please enter a number.\n";
                continue;
            }

            choice = strToInt(choiceStr);

            switch (choice) {
            case 1: addSectorInteractive(); break;
            case 2: addStreetInteractive(); break;
            case 3: addHouseInteractive(); break;
            case 4: addFamilyInteractive(); break;
            case 5: displayHousingInfo(); break;
            case 6: saveHousingInfoToFile(); break;
            case 7:
                cout << "\n=== HOUSING STATISTICS ===\n";
                cout << "Sectors: " << countSectors() << "\n";
                cout << "Streets: " << countStreets() << "\n";
                cout << "Houses: " << countHouses() << "\n";
                cout << "Families: " << countFamilies() << "\n";
                cout << "Total Population: " << tree.getTotalPopulation() << "\n";
                break;
            case 0: break;
            default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
};

