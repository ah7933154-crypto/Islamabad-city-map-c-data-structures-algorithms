#ifndef MEDICAL_H
#define MEDICAL_H

#include "SmartCity.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <limits>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"

using namespace std;

// ========================= UTILITIES =========================

static inline string to_lower_med(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.length(); ++i)
        r[i] = tolower((unsigned char)r[i]);
    return r;
}

static inline unsigned long long fnv1a_hash_med(const string& s) {
    unsigned long long h = 1469598103934665603ULL;
    for (size_t i = 0; i < s.size(); i++) {
        h ^= (unsigned long long)(unsigned char)s[i];
        h *= 1099511628211ULL;
    }
    return h;
}



// ========================= FORWARD DECLARATIONS =========================

class Room;
class Med_Floor;
class Med;
class Pharmacy;
class Hospital;
//class PatientNode;
class DoctorNode;
class PatientHeapNode;
class PatientPriorityQueue;
class MedicineLocationNode;
class GlobalMedicineHashTable;

// ========================= ROOM CLASS =========================

class Room {
public:
    int roomNumber;
    string roomType;
    int bedCount;
    int availableBeds;
    bool isOccupied;
    Room* next;

    Room(int num, const string& type, int beds)
        : roomNumber(num), roomType(type), bedCount(beds),
        availableBeds(beds), isOccupied(false), next(nullptr) {
    }
};

// ========================= Med_Floor CLASS =========================

class Med_Floor {
public:
    int Med_FloorNumber;
    string specialization;
    Room* rooms;
    Med_Floor* next;

    Med_Floor(int num, const string& spec)
        : Med_FloorNumber(num), specialization(spec), rooms(nullptr), next(nullptr) {
    }

    ~Med_Floor() {
        Room* cur = rooms;
        while (cur) {
            Room* temp = cur;
            cur = cur->next;
            delete temp;
        }
    }

    void addRoom(int roomNum, const string& type, int beds) {
        Room* newRoom = new Room(roomNum, type, beds);
        newRoom->next = rooms;
        rooms = newRoom;
    }

    Room* findAvailableRoom(const string& type = "") {
        Room* cur = rooms;
        while (cur) {
            if (cur->availableBeds > 0 && (type.empty() || cur->roomType == type)) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    int countAvailableBeds() const {
        int total = 0;
        Room* cur = rooms;
        while (cur) {
            total += cur->availableBeds;
            cur = cur->next;
        }
        return total;
    }
};

// ========================= MEDICINE CLASS =========================

class Med {
public:
    string name;
    string formula;
    string category;
    double price;
    int quantity;
    Med* next;

    Med(const string& n, const string& f, const string& cat, int q, double p)
        : name(n), formula(f), category(cat), quantity(q), price(p), next(nullptr) {
    }
};

// ========================= PHARMACY CLASS =========================

class Pharmacy {
public:
    string id, name, sector;
    int nodeId;
    Med** table;
    int buckets;
    Pharmacy* next;

    Pharmacy(string pid, string pname, string psector, int nid)
        : id(pid), name(pname), sector(psector), nodeId(nid), buckets(17), next(nullptr) {
        table = new Med * [buckets];
        for (int i = 0; i < buckets; i++) table[i] = nullptr;
    }

    ~Pharmacy() {
        for (int i = 0; i < buckets; ++i) {
            Med* cur = table[i];
            while (cur) {
                Med* tmp = cur;
                cur = cur->next;
                delete tmp;
            }
        }
        delete[] table;
    }

    void addMedicine(const string& n, const string& f, const string& cat, int q, double p) {
        int idx = fnv1a_hash_med(n + f) % buckets;
        Med* m = new Med(n, f, cat, q, p);
        m->next = table[idx];
        table[idx] = m;
        cout << " Medicine " << n << " added to pharmacy " << name << "\n";
    }

    Med* searchMedicine(const string& key) {
        string lowerKey = to_lower_med(key);
        for (int i = 0; i < buckets; i++) {
            Med* cur = table[i];
            while (cur) {
                if (to_lower_med(cur->name) == lowerKey ||
                    to_lower_med(cur->formula) == lowerKey) {
                    return cur;
                }
                cur = cur->next;
            }
        }
        return nullptr;
    }

    void displayMedicines() {
        cout << "=== Medicines in " << name << " ===\n";
        bool hasMedicines = false;
        for (int i = 0; i < buckets; i++) {
            Med* cur = table[i];
            while (cur) {
                cout << "  • " << cur->name << " (" << cur->formula << ")\n";
                cout << "    Category: " << cur->category << "\n";
                cout << "    Price: Rs." << fixed << setprecision(2) << cur->price << "\n";
                cout << "    Quantity: " << cur->quantity << "\n";
                cout << "    --------------------\n";
                hasMedicines = true;
                cur = cur->next;
            }
        }
        if (!hasMedicines) {
            cout << "  No medicines available\n";
        }
    }
};

// ========================= MEDICINE LOCATION NODE =========================

struct MedicineLocationNode {
    string key;
    Pharmacy* pharmacy;
    double price;
    MedicineLocationNode* next;
    MedicineLocationNode(const string& k, Pharmacy* p, double pr)
        : key(k), pharmacy(p), price(pr), next(nullptr) {
    }
};

// ========================= GLOBAL MEDICINE HASH TABLE =========================

class GlobalMedicineHashTable {
    MedicineLocationNode** table;
    int buckets;
public:
    GlobalMedicineHashTable() : buckets(101) {
        table = new MedicineLocationNode * [buckets];
        for (int i = 0; i < buckets; i++) table[i] = nullptr;
    }
    ~GlobalMedicineHashTable() {
        for (int i = 0; i < buckets; i++) {
            MedicineLocationNode* cur = table[i];
            while (cur) {
                MedicineLocationNode* t = cur;
                cur = cur->next;
                delete t;
            }
        }
        delete[] table;
    }

    void add(const string& key, Pharmacy* p, double price) {
        int idx = fnv1a_hash_med(to_lower_med(key)) % buckets;
        MedicineLocationNode* node = new MedicineLocationNode(key, p, price);
        node->next = table[idx];
        table[idx] = node;
    }

    void search(const string& key) {
        string lowerKey = to_lower_med(key);
        int idx = fnv1a_hash_med(lowerKey) % buckets;
        MedicineLocationNode* cur = table[idx];
        bool found = false;

        cout << "=== Search Results for '" << key << "' ===\n";
        while (cur) {
            if (to_lower_med(cur->key) == lowerKey) {
                cout << "  • Found at: " << cur->pharmacy->name << "\n";
                cout << "    Sector: " << cur->pharmacy->sector << "\n";
                cout << "    Price: Rs." << fixed << setprecision(2) << cur->price << "\n";
                cout << "    --------------------\n";
                found = true;
            }
            cur = cur->next;
        }
        if (!found) cout << "  No matches found\n";
    }
};

// ========================= PATIENT NODE =========================

struct PatientNode {
    string cnic, name, disease, assignedDoctor;
    int age, priority;
    int roomNumber;
    int Med_FloorNumber;
    bool isAdmitted;
    PatientNode* next;

    PatientNode(const string& c, const string& n, int a, const string& d, int p)
        : cnic(c), name(n), age(a), disease(d), priority(p),
        roomNumber(-1), Med_FloorNumber(-1), isAdmitted(false), next(nullptr) {
    }
};

// ========================= DOCTOR NODE =========================

struct DoctorNode {
    string id, name, specialization;
    int Med_FloorAssigned;
    int patientCount;
    DoctorNode* next;
    DoctorNode(const string& i, const string& n, const string& s, int Med_Floor = -1)
        : id(i), name(n), specialization(s), Med_FloorAssigned(Med_Floor), patientCount(0), next(nullptr) {
    }
};

// ========================= PATIENT HEAP NODE =========================

struct PatientHeapNode {
    string cnic, name, disease;
    int priority;
    time_t arrivalTime;
    PatientHeapNode() : cnic(""), name(""), disease(""), priority(0), arrivalTime(0) {}
    PatientHeapNode(const string& c, const string& n, const string& d, int p)
        : cnic(c), name(n), disease(d), priority(p) {
        arrivalTime = time(nullptr);
    }
};

// ========================= PATIENT PRIORITY QUEUE =========================

class PatientPriorityQueue {
private:
    PatientHeapNode* heap;
    int capacity;
    int size;

    void heapifyUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[i].priority > heap[p].priority ||
                (heap[i].priority == heap[p].priority && heap[i].arrivalTime < heap[p].arrivalTime)) {
                swap(heap[i], heap[p]);
                i = p;
            }
            else break;
        }
    }

    void heapifyDown(int i) {
        while (true) {
            int l = 2 * i + 1;
            int r = 2 * i + 2;
            int largest = i;

            if (l < size) {
                if (heap[l].priority > heap[largest].priority ||
                    (heap[l].priority == heap[largest].priority && heap[l].arrivalTime < heap[largest].arrivalTime)) {
                    largest = l;
                }
            }
            if (r < size) {
                if (heap[r].priority > heap[largest].priority ||
                    (heap[r].priority == heap[largest].priority && heap[r].arrivalTime < heap[largest].arrivalTime)) {
                    largest = r;
                }
            }
            if (largest != i) {
                swap(heap[i], heap[largest]);
                i = largest;
            }
            else break;
        }
    }

public:
    PatientPriorityQueue(int cap = 200) : capacity(cap), size(0) {
        heap = new PatientHeapNode[capacity];
    }

    ~PatientPriorityQueue() {
        delete[] heap;
    }

    void insert(const string& cnic, const string& name, const string& disease, int priority) {
        if (size == capacity) {
            cout << "Emergency queue is full!\n";
            return;
        }
        heap[size] = PatientHeapNode(cnic, name, disease, priority);
        heapifyUp(size);
        size++;
    }

    PatientHeapNode extractMax() {
        if (size == 0) return PatientHeapNode();

        PatientHeapNode maxNode = heap[0];
        heap[0] = heap[--size];
        heapifyDown(0);
        return maxNode;
    }

    bool isEmpty() const {
        return size == 0;
    }

    int getSize() const {
        return size;
    }

    void displayQueue() {
        if (size == 0) {
            cout << "Emergency queue is empty\n";
            return;
        }

        cout << "=== EMERGENCY QUEUE (Total: " << size << ") ===\n";
        for (int i = 0; i < size; i++) {
            cout << "  " << (i + 1) << ". " << heap[i].name << "\n";
            cout << "     CNIC: " << heap[i].cnic << "\n";
            cout << "     Disease: " << heap[i].disease << "\n";
            cout << "     Priority: " << heap[i].priority;

            if (heap[i].priority >= 9) cout << " (Critical)";
            else if (heap[i].priority >= 7) cout << " (Serious)";
            else if (heap[i].priority >= 5) cout << " (Moderate)";
            else cout << " (Normal)";

            cout << "\n     --------------------\n";
        }
    }

    bool removePatient(const string& cnic) {
        int idx = -1;
        for (int i = 0; i < size; i++) {
            if (heap[i].cnic == cnic) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return false;

        heap[idx] = heap[--size];
        if (idx < size) {
            heapifyUp(idx);
            heapifyDown(idx);
        }
        return true;
    }
};

// ========================= HOSPITAL CLASS =========================

class Hospital {
public:
    string id, name, sector;
    int nodeId;
    int totalBeds, availableBeds;
    string specialization;

    Med_Floor* Med_Floors;
    int Med_FloorCount;

    DoctorNode* doctors;
    PatientNode* patients;
    PatientPriorityQueue* waitingQueue;
    Hospital* next;

    Hospital(const string& i, const string& n, const string& s, int nid, int beds, const string& spec)
        : id(i), name(n), sector(s), nodeId(nid), totalBeds(beds), availableBeds(beds),
        specialization(spec), Med_Floors(nullptr), Med_FloorCount(0), doctors(nullptr),
        patients(nullptr), next(nullptr) {
        waitingQueue = new PatientPriorityQueue();
    }

    ~Hospital() {
        while (Med_Floors) {
            Med_Floor* t = Med_Floors;
            Med_Floors = Med_Floors->next;
            delete t;
        }

        while (doctors) {
            DoctorNode* t = doctors;
            doctors = doctors->next;
            delete t;
        }

        while (patients) {
            PatientNode* t = patients;
            patients = patients->next;
            delete t;
        }

        delete waitingQueue;
    }

    void addMed_Floor(int Med_FloorNum, const string& spec) {
        Med_Floor* newMed_Floor = new Med_Floor(Med_FloorNum, spec);
        newMed_Floor->next = Med_Floors;
        Med_Floors = newMed_Floor;
        Med_FloorCount++;
        cout << " Med_Floor " << Med_FloorNum << " added (" << spec << ")\n";
    }

    void addRoomToMed_Floor(int Med_FloorNum, int roomNum, const string& type, int beds) {
        Med_Floor* Med_Floor = findMed_Floor(Med_FloorNum);
        if (!Med_Floor) {
            cout << " Med_Floor " << Med_FloorNum << " not found\n";
            return;
        }
        Med_Floor->addRoom(roomNum, type, beds);
        cout << " Room " << roomNum << " added to Med_Floor " << Med_FloorNum << " (" << type << ", " << beds << " beds)\n";
    }

    Med_Floor* findMed_Floor(int Med_FloorNum) {
        Med_Floor* cur = Med_Floors;
        while (cur) {
            if (cur->Med_FloorNumber == Med_FloorNum) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Room* findAvailableRoom(const string& roomType = "", const string& specialization = "") {
        Med_Floor* Med_FloorCur = Med_Floors;
        while (Med_FloorCur) {
            if (!specialization.empty() && Med_FloorCur->specialization != specialization) {
                Med_FloorCur = Med_FloorCur->next;
                continue;
            }

            Room* room = Med_FloorCur->findAvailableRoom(roomType);
            if (room) {
                return room;
            }
            Med_FloorCur = Med_FloorCur->next;
        }
        return nullptr;
    }

    void displayMed_FloorsAndRooms() {
        cout << "=== " << name << " - Med_Floors & Rooms ===\n";
        cout << "Total Med_Floors: " << Med_FloorCount << "\n";
        cout << "Available Beds: " << availableBeds << "/" << totalBeds << "\n\n";

        if (!Med_Floors) {
            cout << "No Med_Floors added yet\n";
            return;
        }

        Med_Floor* Med_FloorCur = Med_Floors;
        while (Med_FloorCur) {
            cout << "Med_Floor " << Med_FloorCur->Med_FloorNumber << " (" << Med_FloorCur->specialization << ")\n";
            cout << "Available beds on Med_Floor: " << Med_FloorCur->countAvailableBeds() << "\n";

            Room* roomCur = Med_FloorCur->rooms;
            if (!roomCur) {
                cout << "  No rooms on this Med_Floor\n";
            }
            else {
                while (roomCur) {
                    cout << "  Room " << roomCur->roomNumber << " [" << roomCur->roomType << "]: ";
                    cout << roomCur->availableBeds << "/" << roomCur->bedCount << " beds available\n";
                    roomCur = roomCur->next;
                }
            }
            cout << "  --------------------\n";
            Med_FloorCur = Med_FloorCur->next;
        }
    }

    Med** hospitalMedicines;
    int medBuckets;

    void initMedicineStorage() {
        medBuckets = 13;
        hospitalMedicines = new Med * [medBuckets];
        for (int i = 0; i < medBuckets; i++) hospitalMedicines[i] = nullptr;
    }

    void addMedicineToHospital(const string& name, const string& formula, const string& category, int quantity, double price) {
        if (!hospitalMedicines) initMedicineStorage();

        int idx = fnv1a_hash_med(name + formula) % medBuckets;
        Med* newMed = new Med(name, formula, category, quantity, price);
        newMed->next = hospitalMedicines[idx];
        hospitalMedicines[idx] = newMed;

        cout << " Medicine " << name << " added to hospital stock\n";
    }

    Med* searchMedicineInHospital(const string& key) {
        if (!hospitalMedicines) return nullptr;

        string lowerKey = to_lower_med(key);
        for (int i = 0; i < medBuckets; i++) {
            Med* cur = hospitalMedicines[i];
            while (cur) {
                if (to_lower_med(cur->name) == lowerKey ||
                    to_lower_med(cur->formula) == lowerKey) {
                    return cur;
                }
                cur = cur->next;
            }
        }
        return nullptr;
    }

    void displayHospitalMedicines() {
        if (!hospitalMedicines) {
            cout << "Hospital medicine storage not initialized\n";
            return;
        }

        cout << "=== " << name << " - Medicine Stock ===\n";
        bool hasMedicines = false;
        for (int i = 0; i < medBuckets; i++) {
            Med* cur = hospitalMedicines[i];
            while (cur) {
                cout << "  • " << cur->name << " (" << cur->formula << ")\n";
                cout << "    Category: " << cur->category << "\n";
                cout << "    Quantity: " << cur->quantity << "\n";
                cout << "    Price: Rs." << fixed << setprecision(2) << cur->price << "\n";
                cout << "    --------------------\n";
                hasMedicines = true;
                cur = cur->next;
            }
        }
        if (!hasMedicines) {
            cout << "  No medicines in hospital stock\n";
        }
    }

    void addDoctor(const string& id, const string& name, const string& spec, int Med_Floor = -1) {
        DoctorNode* newDoctor = new DoctorNode(id, name, spec, Med_Floor);
        newDoctor->next = doctors;
        doctors = newDoctor;
        cout << " Doctor " << name << " added (" << spec << ")\n";
    }

    DoctorNode* findAvailableDoctor(const string& specialization = "") {
        DoctorNode* cur = doctors;
        while (cur) {
            if ((specialization.empty() || cur->specialization == specialization) && cur->patientCount < 10) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }
};

// ========================= MEDICAL SECTOR CONTROLLER =========================

class MedicalSector {
private:
    Graph* mgraph;

    Hospital* hospitalTable[101];
    Pharmacy* pharmacyTable[101];

    GlobalMedicineHashTable globalMedName;
    GlobalMedicineHashTable globalMedFormula;

    int hash(const string& key) {
        return fnv1a_hash_med(key) % 101;
    }

public:
    MedicalSector(Graph* g) : mgraph(g) {
        for (int i = 0; i < 101; i++) {
            hospitalTable[i] = nullptr;
            pharmacyTable[i] = nullptr;
        }
    }

    // --- HOSPITAL MANAGEMENT ---

    void addHospital(const string& id, const string& name, const string& sector,
        int beds, const string& spec, double lat, double lon) {
        int nodeId = mgraph->addLocation(name, lat, lon, HOSPITAL);
        Hospital* h = new Hospital(id, name, sector, nodeId, beds, spec);

        int idx = hash(id);
        h->next = hospitalTable[idx];
        hospitalTable[idx] = h;

        cout << "Hospital Registered: " << name << " (ID: " << id << ")\n";
    }

    Hospital* findHospital(const string& id) {
        int idx = hash(id);
        Hospital* h = hospitalTable[idx];
        while (h && h->id != id) h = h->next;
        return h;
    }

    Pharmacy* findPharmacy(const string& id) {
        int idx = hash(id);
        Pharmacy* p = pharmacyTable[idx];
        while (p && p->id != id) p = p->next;
        return p;
    }

    // ==================== DISPLAY FUNCTION ====================

    void displayEverything() {
        cout << "\n" << string(60, '=') << "\n";
        cout << "          MEDICAL SECTOR - COMPLETE OVERVIEW\n";
        cout << string(60, '=') << "\n\n";

        // Count statistics
        int hospitalCount = 0;
        int pharmacyCount = 0;
        int totalBeds = 0;
        int availableBeds = 0;
        int totalPatients = 0;
        int totalEmergency = 0;

        cout << "1. HOSPITALS IN THE SYSTEM\n";
        cout << string(40, '-') << "\n";

        for (int i = 0; i < 101; i++) {
            Hospital* h = hospitalTable[i];
            while (h) {
                hospitalCount++;
                totalBeds += h->totalBeds;
                availableBeds += h->availableBeds;
                totalEmergency += h->waitingQueue->getSize();

                cout << "  Hospital: " << h->name << " (ID: " << h->id << ")\n";
                cout << "    Sector: " << h->sector << "\n";
                cout << "    Specialization: " << h->specialization << "\n";
                cout << "    Beds: " << h->availableBeds << "/" << h->totalBeds << " available\n";
                cout << "    Floors: " << h->Med_FloorCount << "\n";
                cout << "    Emergency Queue: " << h->waitingQueue->getSize() << " patients\n";

                // Count patients
                PatientNode* p = h->patients;
                int patientCount = 0;
                while (p) {
                    patientCount++;
                    p = p->next;
                }
                totalPatients += patientCount;
                cout << "    Admitted Patients: " << patientCount << "\n";
                cout << string(30, '~') << "\n";

                h = h->next;
            }
        }

        if (hospitalCount == 0) {
            cout << "  No hospitals registered\n";
        }

        cout << "\n2. PHARMACIES IN THE SYSTEM\n";
        cout << string(40, '-') << "\n";

        for (int i = 0; i < 101; i++) {
            Pharmacy* p = pharmacyTable[i];
            while (p) {
                pharmacyCount++;
                cout << "  Pharmacy: " << p->name << " (ID: " << p->id << ")\n";
                cout << "    Sector: " << p->sector << "\n";
                cout << string(30, '~') << "\n";
                p = p->next;
            }
        }

        if (pharmacyCount == 0) {
            cout << "  No pharmacies registered\n";
        }

        cout << "\n3. SYSTEM STATISTICS\n";
        cout << string(40, '-') << "\n";
        cout << "  Total Hospitals: " << hospitalCount << "\n";
        cout << "  Total Pharmacies: " << pharmacyCount << "\n";
        cout << "  Total Hospital Beds: " << totalBeds << "\n";
        cout << "  Available Beds: " << availableBeds << " ("
            << fixed << setprecision(1)
            << (totalBeds > 0 ? (availableBeds * 100.0 / totalBeds) : 0)
            << "% available)\n";
        cout << "  Total Admitted Patients: " << totalPatients << "\n";
        cout << "  Total Emergency Patients: " << totalEmergency << "\n";

        cout << "\n" << string(60, '=') << "\n";
        cout << "                 END OF REPORT\n";
        cout << string(60, '=') << "\n\n";
    }

    // ==================== INTERACTIVE FUNCTIONS WITH VALIDATION ====================

    void manageHospitalMed_FloorsInteractive() {
        string hospitalId;
        cout << "Enter Hospital ID: ";
        cin >> hospitalId;
        cin.ignore();

        if (!isValidMedicalID(hospitalId)) {
            cout << "Error: Invalid hospital ID\n";
            return;
        }

        Hospital* h = findHospital(hospitalId);
        if (!h) {
            cout << "Error: Hospital not found\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- Med_Floor & ROOM MANAGEMENT - " << h->name << " ---\n";
            cout << "1. Add Med_Floor\n";
            cout << "2. Add Room to Med_Floor\n";
            cout << "3. View Med_Floors & Rooms\n";
            cout << "4. View Available Beds\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            if (!(cin >> choice)) {
                cout << "Error: Invalid choice\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();

            switch (choice) {
            case 1: {
                int Med_FloorNum;
                string spec;
                cout << "Enter Med_Floor Number: ";
                if (!(cin >> Med_FloorNum)) {
                    cout << "Error: Invalid floor number\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();
                cout << "Enter Specialization: ";
                getline(cin, spec);
                if (spec.empty()) {
                    cout << "Error: Specialization cannot be empty\n";
                    break;
                }
                h->addMed_Floor(Med_FloorNum, spec);
                break;
            }
            case 2: {
                int Med_FloorNum, roomNum, beds;
                string type;
                cout << "Enter Med_Floor Number: ";
                if (!(cin >> Med_FloorNum)) {
                    cout << "Error: Invalid floor number\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cout << "Enter Room Number: ";
                if (!(cin >> roomNum)) {
                    cout << "Error: Invalid room number\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();
                cout << "Enter Room Type (General/ICU/Emergency/Private/Surgery): ";
                getline(cin, type);
                if (type.empty()) {
                    cout << "Error: Room type cannot be empty\n";
                    break;
                }
                cout << "Enter Number of Beds: ";
                if (!(cin >> beds) || !isValidBedCount(beds)) {
                    cout << "Error: Invalid bed count\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                h->addRoomToMed_Floor(Med_FloorNum, roomNum, type, beds);
                break;
            }
            case 3:
                h->displayMed_FloorsAndRooms();
                break;
            case 4: {
                cout << "Available Beds: " << h->availableBeds << "/" << h->totalBeds << "\n";
                break;
            }
            case 0:
                break;
            default:
                cout << "Error: Invalid choice\n";
            }
        } while (choice != 0);
    }

    void manageHospitalMedicinesInteractive() {
        string hospitalId;
        cout << "Enter Hospital ID: ";
        cin >> hospitalId;
        cin.ignore();

        if (!isValidMedicalID(hospitalId)) {
            cout << "Error: Invalid hospital ID\n";
            return;
        }

        Hospital* h = findHospital(hospitalId);
        if (!h) {
            cout << "Error: Hospital not found\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- HOSPITAL MEDICINE MANAGEMENT - " << h->name << " ---\n";
            cout << "1. Add Medicine to Hospital Stock\n";
            cout << "2. Search Medicine in Hospital\n";
            cout << "3. View Hospital Medicine Stock\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            if (!(cin >> choice)) {
                cout << "Error: Invalid choice\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();

            switch (choice) {
            case 1: {
                string name, formula, category;
                int quantity;
                double price;

                cout << "Medicine Name: ";
                getline(cin, name);
                if (name.empty()) {
                    cout << "Error: Medicine name cannot be empty\n";
                    break;
                }
                cout << "Chemical Formula: ";
                getline(cin, formula);
                if (formula.empty()) {
                    cout << "Error: Formula cannot be empty\n";
                    break;
                }
                cout << "Category: ";
                getline(cin, category);
                if (category.empty()) {
                    cout << "Error: Category cannot be empty\n";
                    break;
                }
                cout << "Quantity: ";
                if (!(cin >> quantity) || !isValidQuantity(quantity)) {
                    cout << "Error: Invalid quantity\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cout << "Price: ";
                if (!(cin >> price) || !isValidPrice(price)) {
                    cout << "Error: Invalid price\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();

                h->addMedicineToHospital(name, formula, category, quantity, price);
                break;
            }
            case 2: {
                string searchTerm;
                cout << "Enter medicine name or formula to search: ";
                getline(cin, searchTerm);
                if (searchTerm.empty()) {
                    cout << "Error: Search term cannot be empty\n";
                    break;
                }

                Med* found = h->searchMedicineInHospital(searchTerm);
                if (found) {
                    cout << " Medicine Found:\n";
                    cout << "  Name: " << found->name << "\n";
                    cout << "  Formula: " << found->formula << "\n";
                    cout << "  Category: " << found->category << "\n";
                    cout << "  Quantity: " << found->quantity << "\n";
                    cout << "  Price: Rs." << fixed << setprecision(2) << found->price << "\n";
                }
                else {
                    cout << " Medicine not found in hospital stock\n";
                }
                break;
            }
            case 3:
                h->displayHospitalMedicines();
                break;
            case 0:
                break;
            default:
                cout << "Error: Invalid choice\n";
            }
        } while (choice != 0);
    }

    void emergencySystemInteractive() {
        string hospitalId;
        cout << "Enter Hospital ID: ";
        cin >> hospitalId;
        cin.ignore();

        if (!isValidMedicalID(hospitalId)) {
            cout << "Error: Invalid hospital ID\n";
            return;
        }

        Hospital* h = findHospital(hospitalId);
        if (!h) {
            cout << "Error: Hospital not found\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- EMERGENCY SYSTEM - " << h->name << " ---\n";
            cout << "1. Add Emergency Patient\n";
            cout << "2. View Emergency Queue\n";
            cout << "3. Admit Next Emergency Patient\n";
            cout << "4. Remove Patient from Queue\n";
            cout << "5. View Current Emergency Status\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            if (!(cin >> choice)) {
                cout << "Error: Invalid choice\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();

            switch (choice) {
            case 1: {
                string cnic, name, disease;
                int priority;

                cout << "Patient CNIC (13 digits): ";
                getline(cin, cnic);
                if (!isValidCNIC(cnic)) {
                    cout << "Error: Invalid CNIC\n";
                    break;
                }
                cout << "Patient Name: ";
                getline(cin, name);
                if (!isValidName(name)) {
                    cout << "Error: Invalid name\n";
                    break;
                }
                cout << "Disease/Symptoms: ";
                getline(cin, disease);
                if (disease.empty()) {
                    cout << "Error: Disease cannot be empty\n";
                    break;
                }
                cout << "Priority (1-10, 10=Critical): ";
                if (!(cin >> priority) || !isValidPriority(priority)) {
                    cout << "Error: Invalid priority\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();

                if (h->availableBeds > 0 && priority >= 8) {
                    cout << "Critical patient! Attempting immediate admission...\n";
                    Room* availableRoom = h->findAvailableRoom("ICU", "Emergency");
                    if (availableRoom) {
                        PatientNode* newPatient = new PatientNode(cnic, name, 0, disease, priority);
                        newPatient->isAdmitted = true;
                        newPatient->roomNumber = availableRoom->roomNumber;
                        availableRoom->availableBeds--;
                        h->availableBeds--;

                        newPatient->next = h->patients;
                        h->patients = newPatient;

                        cout << " Critical patient admitted to ICU Room " << availableRoom->roomNumber << "\n";
                    }
                    else {
                        h->waitingQueue->insert(cnic, name, disease, priority);
                        cout << " Patient added to emergency queue\n";
                    }
                }
                else {
                    h->waitingQueue->insert(cnic, name, disease, priority);
                    cout << "Patient added to emergency queue\n";
                }
                break;
            }
            case 2:
                h->waitingQueue->displayQueue();
                break;
            case 3: {
                if (h->waitingQueue->isEmpty()) {
                    cout << "Emergency queue is empty\n";
                    break;
                }

                if (h->availableBeds == 0) {
                    cout << "No beds available\n";
                    break;
                }

                PatientHeapNode nextPatient = h->waitingQueue->extractMax();

                Room* availableRoom = h->findAvailableRoom();
                if (availableRoom) {
                    PatientNode* newPatient = new PatientNode(
                        nextPatient.cnic, nextPatient.name, 0, nextPatient.disease, nextPatient.priority);
                    newPatient->isAdmitted = true;
                    newPatient->roomNumber = availableRoom->roomNumber;
                    availableRoom->availableBeds--;
                    h->availableBeds--;

                    newPatient->next = h->patients;
                    h->patients = newPatient;

                    cout << " Admitted patient: " << nextPatient.name << "\n";
                    cout << "  Room: " << availableRoom->roomNumber << "\n";
                    cout << "  Priority: " << nextPatient.priority << "\n";
                }
                else {
                    cout << " No suitable room available\n";
                }
                break;
            }
            case 4: {
                string cnic;
                cout << "Enter patient CNIC to remove: ";
                getline(cin, cnic);

                if (!isValidCNIC(cnic)) {
                    cout << "Error: Invalid CNIC\n";
                    break;
                }

                if (h->waitingQueue->removePatient(cnic)) {
                    cout << " Patient removed from emergency queue\n";
                }
                else {
                    cout << "Patient not found in queue\n";
                }
                break;
            }
            case 5: {
                cout << "=== EMERGENCY STATUS ===\n";
                cout << "Available Beds: " << h->availableBeds << "/" << h->totalBeds << "\n";
                cout << "Patients in Emergency Queue: " << h->waitingQueue->getSize() << "\n";
                break;
            }
            case 0:
                break;
            default:
                cout << "Error: Invalid choice\n";
            }
        } while (choice != 0);
    }

    void patientRegistrationInteractive() {
        string hospitalId;
        cout << "Enter Hospital ID: ";
        cin >> hospitalId;
        cin.ignore();

        if (!isValidMedicalID(hospitalId)) {
            cout << "Error: Invalid hospital ID\n";
            return;
        }

        Hospital* h = findHospital(hospitalId);
        if (!h) {
            cout << "Error: Hospital not found\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- PATIENT REGISTRATION - " << h->name << " ---\n";
            cout << "1. Register New Patient\n";
            cout << "2. Assign Doctor to Patient\n";
            cout << "3. View All Patients\n";
            cout << "4. Discharge Patient\n";
            cout << "5. Add Doctor to Hospital\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            if (!(cin >> choice)) {
                cout << "Error: Invalid choice\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();

            switch (choice) {
            case 1: {
                string cnic, name, disease;
                int age, priority;

                cout << "Patient CNIC (13 digits): ";
                getline(cin, cnic);
                if (!isValidCNIC(cnic)) {
                    cout << "Error: Invalid CNIC\n";
                    break;
                }
                cout << "Patient Name: ";
                getline(cin, name);
                if (!isValidName(name)) {
                    cout << "Error: Invalid name\n";
                    break;
                }
                cout << "Age: ";
                if (!(cin >> age) || age < 0 || age > 150) {
                    cout << "Error: Invalid age\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();
                cout << "Disease/Symptoms: ";
                getline(cin, disease);
                if (disease.empty()) {
                    cout << "Error: Disease cannot be empty\n";
                    break;
                }
                cout << "Priority (1-10): ";
                if (!(cin >> priority) || !isValidPriority(priority)) {
                    cout << "Error: Invalid priority\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();

                if (h->availableBeds > 0) {
                    Room* room = h->findAvailableRoom();
                    if (room) {
                        PatientNode* newPatient = new PatientNode(cnic, name, age, disease, priority);
                        newPatient->isAdmitted = true;
                        newPatient->roomNumber = room->roomNumber;
                        room->availableBeds--;
                        h->availableBeds--;

                        newPatient->next = h->patients;
                        h->patients = newPatient;

                        cout << " Patient " << name << " admitted to Room " << room->roomNumber << "\n";
                    }
                    else {
                        cout << " No suitable room available\n";
                    }
                }
                else {
                    cout << " No beds available. Patient added to waiting list\n";
                    h->waitingQueue->insert(cnic, name, disease, priority);
                }
                break;
            }
            case 2: {
                string patientCNIC, doctorID;
                cout << "Enter Patient CNIC: ";
                getline(cin, patientCNIC);
                if (!isValidCNIC(patientCNIC)) {
                    cout << "Error: Invalid CNIC\n";
                    break;
                }
                cout << "Enter Doctor ID: ";
                getline(cin, doctorID);
                if (!isValidMedicalID(doctorID)) {
                    cout << "Error: Invalid doctor ID\n";
                    break;
                }

                PatientNode* patient = h->patients;
                while (patient && patient->cnic != patientCNIC) patient = patient->next;

                DoctorNode* doctor = h->doctors;
                while (doctor && doctor->id != doctorID) doctor = doctor->next;

                if (!patient) {
                    cout << " Patient not found\n";
                }
                else if (!doctor) {
                    cout << " Doctor not found\n";
                }
                else {
                    patient->assignedDoctor = doctor->name;
                    doctor->patientCount++;
                    cout << " Doctor " << doctor->name << " assigned to patient " << patient->name << "\n";
                }
                break;
            }
            case 3: {
                cout << "=== PATIENTS IN " << h->name << " ===\n";
                PatientNode* cur = h->patients;
                if (!cur) {
                    cout << "No patients admitted\n";
                }
                else {
                    int count = 0;
                    while (cur) {
                        cout << (++count) << ". " << cur->name << "\n";
                        cout << "   CNIC: " << cur->cnic << "\n";
                        cout << "   Age: " << cur->age << "\n";
                        cout << "   Disease: " << cur->disease << "\n";
                        cout << "   Room: " << (cur->roomNumber == -1 ? "Not assigned" : to_string(cur->roomNumber)) << "\n";
                        cout << "   Doctor: " << (cur->assignedDoctor.empty() ? "Not assigned" : cur->assignedDoctor) << "\n";
                        cout << "   --------------------\n";
                        cur = cur->next;
                    }
                }
                break;
            }
            case 4: {
                string cnic;
                cout << "Enter patient CNIC to discharge: ";
                getline(cin, cnic);

                if (!isValidCNIC(cnic)) {
                    cout << "Error: Invalid CNIC\n";
                    break;
                }

                PatientNode* prev = nullptr;
                PatientNode* cur = h->patients;
                while (cur && cur->cnic != cnic) {
                    prev = cur;
                    cur = cur->next;
                }

                if (!cur) {
                    cout << " Patient not found\n";
                }
                else {
                    if (cur->roomNumber != -1) {
                        h->availableBeds++;
                    }

                    if (prev) prev->next = cur->next;
                    else h->patients = cur->next;

                    if (!cur->assignedDoctor.empty()) {
                        DoctorNode* doctor = h->doctors;
                        while (doctor && doctor->name != cur->assignedDoctor)
                            doctor = doctor->next;
                        if (doctor && doctor->patientCount > 0)
                            doctor->patientCount--;
                    }

                    delete cur;
                    cout << " Patient discharged\n";

                    if (!h->waitingQueue->isEmpty() && h->availableBeds > 0) {
                        PatientHeapNode nextPatient = h->waitingQueue->extractMax();
                        Room* room = h->findAvailableRoom();
                        if (room) {
                            PatientNode* newPatient = new PatientNode(
                                nextPatient.cnic, nextPatient.name, 0, nextPatient.disease, nextPatient.priority);
                            newPatient->isAdmitted = true;
                            newPatient->roomNumber = room->roomNumber;
                            room->availableBeds--;
                            h->availableBeds--;

                            newPatient->next = h->patients;
                            h->patients = newPatient;

                            cout << " Admitted next patient from queue: " << nextPatient.name << "\n";
                        }
                    }
                }
                break;
            }
            case 5: {
                string id, name, spec;
                int Med_Floor;

                cout << "Doctor ID: ";
                getline(cin, id);
                if (!isValidMedicalID(id)) {
                    cout << "Error: Invalid doctor ID\n";
                    break;
                }
                cout << "Doctor Name: ";
                getline(cin, name);
                if (!isValidName(name)) {
                    cout << "Error: Invalid name\n";
                    break;
                }
                cout << "Specialization: ";
                getline(cin, spec);
                if (spec.empty()) {
                    cout << "Error: Specialization cannot be empty\n";
                    break;
                }
                cout << "Med_Floor Assignment (-1 for none): ";
                if (!(cin >> Med_Floor)) {
                    cout << "Error: Invalid floor assignment\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore();

                h->addDoctor(id, name, spec, Med_Floor);
                break;
            }
            case 0:
                break;
            default:
                cout << "Error: Invalid choice\n";
            }
        } while (choice != 0);
    }

    void registerHospitalInteractive() {
        string id, name, sector, spec;
        int beds;
        double lat, lon;

        cout << "Hospital ID: ";
        cin >> id;
        cin.ignore();
        if (!isValidMedicalID(id)) {
            cout << "Error: Invalid hospital ID\n";
            return;
        }

        cout << "Name: ";
        getline(cin, name);
        if (!isValidName(name)) {
            cout << "Error: Invalid name\n";
            return;
        }

        cout << "Sector: ";
        getline(cin, sector);
        if (sector.empty()) {
            cout << "Error: Sector cannot be empty\n";
            return;
        }

        cout << "Specialization: ";
        getline(cin, spec);
        if (spec.empty()) {
            cout << "Error: Specialization cannot be empty\n";
            return;
        }

        cout << "Total Beds: ";
        if (!(cin >> beds) || !isValidBedCount(beds)) {
            cout << "Error: Invalid bed count\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Latitude Longitude: ";
        if (!(cin >> lat >> lon) || !isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        addHospital(id, name, sector, beds, spec, lat, lon);
    }

    void findNearestHospitalInteractive() {
        double lat, lon;
        cout << "Enter your Latitude Longitude: ";
        if (!(cin >> lat >> lon) || !isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int nodeId = mgraph->findNearestNodeOfType(lat, lon, HOSPITAL);
        if (nodeId == -1) {
            cout << " No hospitals found in the city.\n";
            return;
        }

        Hospital* found = nullptr;
        for (int i = 0; i < 101; i++) {
            Hospital* curr = hospitalTable[i];
            while (curr) {
                if (curr->nodeId == nodeId) {
                    found = curr;
                    break;
                }
                curr = curr->next;
            }
            if (found) break;
        }

        cout << "\n=== NEAREST HOSPITAL ===\n";
        if (found) {
            cout << "Name: " << found->name << "\n";
            cout << "ID: " << found->id << "\n";
            cout << "Sector: " << found->sector << "\n";
            cout << "Specialization: " << found->specialization << "\n";
            cout << "Available Beds: " << found->availableBeds << "/" << found->totalBeds << "\n";
            cout << "Emergency Queue: " << (found->waitingQueue ? found->waitingQueue->getSize() : 0) << " patients\n";
        }

        Coordinate c = mgraph->getCoord(nodeId);
        double distance = haversine(lat, lon, c.lat, c.lon);
        cout << "Distance: " << fixed << setprecision(2) << distance << " km\n";
    }

    void addPharmacy(const string& id, const string& name, const string& sector, double lat, double lon) {
        int nodeId = mgraph->addLocation(name, lat, lon, PHARMACY);
        Pharmacy* p = new Pharmacy(id, name, sector, nodeId);

        int idx = hash(id);
        p->next = pharmacyTable[idx];
        pharmacyTable[idx] = p;

        cout << "Pharmacy Registered: " << name << " (ID: " << id << ")\n";
    }

    void registerPharmacyInteractive() {
        string id, name, sector;
        double lat, lon;
        cout << "Pharmacy ID: ";
        cin >> id;
        cin.ignore();

        if (!isValidMedicalID(id)) {
            cout << "Error: Invalid pharmacy ID\n";
            return;
        }

        cout << "Name: ";
        getline(cin, name);
        if (!isValidName(name)) {
            cout << "Error: Invalid name\n";
            return;
        }

        cout << "Sector: ";
        getline(cin, sector);
        if (sector.empty()) {
            cout << "Error: Sector cannot be empty\n";
            return;
        }

        cout << "Latitude Longitude: ";
        if (!(cin >> lat >> lon) || !isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        addPharmacy(id, name, sector, lat, lon);
    }

    void findNearestPharmacyInteractive() {
        double lat, lon;
        cout << "Enter your Latitude Longitude: ";
        if (!(cin >> lat >> lon) || !isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
            cout << "Error: Invalid coordinates\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int id = mgraph->findNearestNodeOfType(lat, lon, PHARMACY);

        if (id != -1) {
            Coordinate c = mgraph->getCoord(id);
            double distance = haversine(lat, lon, c.lat, c.lon);
            cout << " Nearest Pharmacy: " << mgraph->getName(id) << "\n";
            cout << "  Distance: " << fixed << setprecision(2) << distance << " km\n";
        }
        else {
            cout << "No pharmacies found.\n";
        }
    }

    void searchMedicineInteractive() {
        int choice;
        cout << "Search by:\n";
        cout << "1. Medicine Name\n";
        cout << "2. Chemical Formula\n";
        cout << "Choice: ";
        if (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "Error: Invalid choice\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore();

        string term;
        cout << "Enter search term: ";
        getline(cin, term);
        if (term.empty()) {
            cout << "Error: Search term cannot be empty\n";
            return;
        }

        if (choice == 1) {
            globalMedName.search(term);
        }
        else if (choice == 2) {
            globalMedFormula.search(term);
        }
    }

    void addMedicineToPharmacyInteractive() {
        string pid, medName, formula, category;
        int qty;
        double price;

        cout << "Pharmacy ID: ";
        cin >> pid;
        cin.ignore();

        if (!isValidMedicalID(pid)) {
            cout << "Error: Invalid pharmacy ID\n";
            return;
        }

        cout << "Medicine Name: ";
        getline(cin, medName);
        if (medName.empty()) {
            cout << "Error: Medicine name cannot be empty\n";
            return;
        }

        cout << "Chemical Formula: ";
        getline(cin, formula);
        if (formula.empty()) {
            cout << "Error: Formula cannot be empty\n";
            return;
        }

        cout << "Category: ";
        getline(cin, category);
        if (category.empty()) {
            cout << "Error: Category cannot be empty\n";
            return;
        }

        cout << "Quantity: ";
        if (!(cin >> qty) || !isValidQuantity(qty)) {
            cout << "Error: Invalid quantity\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Price: ";
        if (!(cin >> price) || !isValidPrice(price)) {
            cout << "Error: Invalid price\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        Pharmacy* p = findPharmacy(pid);
        if (!p) {
            cout << "Error: Pharmacy not found\n";
            return;
        }

        p->addMedicine(medName, formula, category, qty, price);
        globalMedName.add(medName, p, price);
        globalMedFormula.add(formula, p, price);
    }

    void viewPharmacyMedicinesInteractive() {
        string pid;
        cout << "Enter Pharmacy ID: ";
        cin >> pid;
        cin.ignore();

        if (!isValidMedicalID(pid)) {
            cout << "Error: Invalid pharmacy ID\n";
            return;
        }

        Pharmacy* p = findPharmacy(pid);
        if (!p) {
            cout << "Error: Pharmacy not found\n";
            return;
        }

        p->displayMedicines();
    }

    void loadHospitalsFromCSV(const string& filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Cannot open hospital file: " << filename << "\n";
            return;
        }

        string line;
        getline(file, line);

        int count = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string id, name, sector, bedsStr, spec;
            string latStr, lonStr;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, sector, ',');
            getline(ss, bedsStr, ',');
            getline(ss, spec, ',');
            getline(ss, latStr, ',');
            getline(ss, lonStr);

            int beds = 0;
            double lat = 0.0, lon = 0.0;

            try {
                beds = stoi(bedsStr);
                lat = stod(latStr);
                lon = stod(lonStr);
            }
            catch (...) {
                beds = 50;
                lat = 33.6 + (rand() % 1000) / 1000.0;
                lon = 73.0 + (rand() % 1000) / 1000.0;
            }

            addHospital(id, name, sector, beds, spec, lat, lon);
            count++;
        }
        file.close();
        cout << " Loaded " << count << " hospitals from " << filename << "\n";
    }

    void loadPharmaciesFromCSV(const string& filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Cannot open pharmacy file: " << filename << "\n";
            return;
        }

        string line;
        getline(file, line);

        int count = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string id, name, sector, latStr, lonStr;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, sector, ',');
            getline(ss, latStr, ',');
            getline(ss, lonStr);

            double lat = 0.0, lon = 0.0;
            try {
                lat = stod(latStr);
                lon = stod(lonStr);
            }
            catch (...) {
                lat = 33.6 + (rand() % 1000) / 1000.0;
                lon = 73.0 + (rand() % 1000) / 1000.0;
            }

            addPharmacy(id, name, sector, lat, lon);
            count++;
        }
        file.close();
        cout << " Loaded " << count << " pharmacies from " << filename << "\n";
    }

    void loadFromCSV(const string& filename) {
        if (filename.find("hospital") != string::npos || filename.find("hosp") != string::npos) {
            loadHospitalsFromCSV(filename);
        }
        else if (filename.find("pharmacy") != string::npos || filename.find("pharm") != string::npos) {
            loadPharmaciesFromCSV(filename);
        }
        else {
            cout << " Unknown file type. Use 'hospital.csv' or 'pharmacy.csv'\n";
        }
    }

    void medicalManagementMenu() {
        int choice;
        do {
            cout << "\n===============================================\n";
            cout << "           MEDICAL SECTOR MANAGEMENT          \n";
            cout << "===============================================\n";
            cout << "1. Hospital Management\n";
            cout << "2. Pharmacy Management\n";
            cout << "3. Medicine Search\n";
            cout << "4. Patient Registration & Management\n";
            cout << "5. Emergency System\n";
            cout << "6. Med_Floor & Room Management\n";
            cout << "7. Hospital Medicine Management\n";
            cout << "8. Load Data from CSV\n";
            cout << "9. Find Nearest Hospital\n";
            cout << "10. Find Nearest Pharmacy\n";
            cout << "11. Display System Overview\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter your choice (0-11): ";
            if (!(cin >> choice)) {
                cout << "Error: Invalid choice\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice) {
            case 1: {
                int hospChoice;
                cout << "\n--- HOSPITAL MANAGEMENT ---\n";
                cout << "1. Register New Hospital\n";
                cout << "2. View Hospital Details\n";
                cout << "0. Back\n";
                cout << "Choice: ";
                if (!(cin >> hospChoice)) {
                    cout << "Error: Invalid choice\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                if (hospChoice == 1) registerHospitalInteractive();
                else if (hospChoice == 2) {
                    string hid;
                    cout << "Enter Hospital ID: ";
                    cin >> hid;
                    cin.ignore();

                    if (!isValidMedicalID(hid)) {
                        cout << "Error: Invalid hospital ID\n";
                        break;
                    }

                    Hospital* h = findHospital(hid);
                    if (h) {
                        cout << "\n=== HOSPITAL DETAILS ===\n";
                        cout << "Name: " << h->name << "\n";
                        cout << "ID: " << h->id << "\n";
                        cout << "Sector: " << h->sector << "\n";
                        cout << "Specialization: " << h->specialization << "\n";
                        cout << "Beds: " << h->availableBeds << "/" << h->totalBeds << "\n";
                        cout << "Med_Floors: " << h->Med_FloorCount << "\n";
                        cout << "Emergency Queue: " << h->waitingQueue->getSize() << " patients\n";
                    }
                    else {
                        cout << "Hospital not found\n";
                    }
                }
                break;
            }
            case 2: {
                int pharmChoice;
                cout << "\n--- PHARMACY MANAGEMENT ---\n";
                cout << "1. Register New Pharmacy\n";
                cout << "2. Add Medicine to Pharmacy\n";
                cout << "3. View Pharmacy Medicines\n";
                cout << "0. Back\n";
                cout << "Choice: ";
                if (!(cin >> pharmChoice)) {
                    cout << "Error: Invalid choice\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                if (pharmChoice == 1) registerPharmacyInteractive();
                else if (pharmChoice == 2) addMedicineToPharmacyInteractive();
                else if (pharmChoice == 3) viewPharmacyMedicinesInteractive();
                break;
            }
            case 3:
                searchMedicineInteractive();
                break;
            case 4:
                patientRegistrationInteractive();
                break;
            case 5:
                emergencySystemInteractive();
                break;
            case 6:
                manageHospitalMed_FloorsInteractive();
                break;
            case 7:
                manageHospitalMedicinesInteractive();
                break;
            case 8: {
                string filename;
                cout << "Enter CSV filename: ";
                cin >> filename;
                cin.ignore();
                loadFromCSV(filename);
                break;
            }
            case 9:
                findNearestHospitalInteractive();
                break;
            case 10:
                findNearestPharmacyInteractive();
                break;
            case 11:
                displayEverything();
                break;
            case 0:
                break;
            default:
                cout << "Error: Invalid choice\n";
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

        } while (choice != 0);
    }

};

#endif // MEDICAL_H