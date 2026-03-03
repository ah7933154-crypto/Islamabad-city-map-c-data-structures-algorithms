#pragma once
#include "SmartCity.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <windows.h>   
using namespace std;

#ifndef INF
#define INF 1e9
#endif

// ================= INPUT VALIDATION UTILITIES =================
class InputValidator {
public:
    // String validation without regex
    static bool isValidString(const string& s, int minLen = 1, int maxLen = 100) {
        if (s.empty()) return false;
        if (s.length() < minLen || s.length() > maxLen) return false;

        // Check for at least one non-whitespace character
        bool hasNonSpace = false;
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r') {
                hasNonSpace = true;
                break;
            }
        }
        if (!hasNonSpace) return false;

        return true;
    }

    static bool isAlphanumeric(const string& s) {
        if (s.empty()) return false;
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '-')) {
                return false;
            }
        }
        return true;
    }

    static bool isValidName(const string& s) {
        if (s.empty() || s.length() > 50) return false;

        // Allow letters, spaces, hyphens, apostrophes, and periods
        bool hasLetter = false;
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                hasLetter = true;
            }
            else if (!(c == ' ' || c == '-' || c == '\'' || c == '.' || c == ',' ||
                (c >= '0' && c <= '9'))) {
                return false;
            }
        }
        return hasLetter;
    }

    static bool isValidEmail(const string& s) {
        if (s.empty()) return false;

        // Simple email validation without regex
        int atCount = 0;
        int dotCount = 0;
        bool hasCharBeforeAt = false;
        bool hasCharAfterAt = false;
        bool hasCharAfterDot = false;

        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];

            if (c == '@') {
                atCount++;
                if (i == 0 || i == s.length() - 1) return false;
                hasCharBeforeAt = true;
            }
            else if (c == '.') {
                if (i == 0 || i == s.length() - 1) return false;
                if (atCount > 0) {
                    dotCount++;
                    // Check if there's at least one character before dot
                    if (i > 0 && s[i - 1] != '@') {
                        hasCharAfterDot = true;
                    }
                }
            }
            else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '+' ||
                c == '-' || c == '.')) {
                return false;
            }
        }

        // Check if we have exactly one @ and at least one . after @
        if (atCount != 1) return false;
        if (dotCount == 0) return false;
        if (!hasCharBeforeAt) return false;
        if (!hasCharAfterDot) return false;

        return true;
    }

    static bool isValidPhone(const string& s) {
        if (s.empty()) return false;

        // Count digits
        int digitCount = 0;
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (c >= '0' && c <= '9') {
                digitCount++;
            }
            else if (!(c == ' ' || c == '-' || c == '(' || c == ')' || c == '+')) {
                return false;
            }
        }

        return digitCount >= 7 && digitCount <= 15;
    }

    // Number validation
    static bool isValidInt(const string& s, int min = -2147483647, int max = 2147483647) {
        if (s.empty()) return false;

        // Check each character
        bool hasDigit = false;
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (i == 0 && (c == '-' || c == '+')) continue;
            if (!(c >= '0' && c <= '9')) return false;
            hasDigit = true;
        }

        if (!hasDigit) return false;

        // Simple range check without stoi (to avoid exceptions)
        // We'll use a simple conversion for checking
        long long value = 0;
        bool negative = false;
        size_t i = 0;

        if (s[0] == '-') {
            negative = true;
            i = 1;
        }
        else if (s[0] == '+') {
            i = 1;
        }

        for (; i < s.length(); i++) {
            value = value * 10 + (s[i] - '0');
            if (value > 2147483647) {
                // Overflow
                return false;
            }
        }

        if (negative) value = -value;

        return value >= min && value <= max;
    }

    static bool isValidDouble(const string& s, double min = -1e308, double max = 1e308) {
        if (s.empty()) return false;

        // Check format
        bool hasDecimal = false;
        bool hasDigit = false;
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (i == 0 && (c == '-' || c == '+')) continue;
            if (c == '.') {
                if (hasDecimal) return false; // Multiple decimals
                hasDecimal = true;
            }
            else if (!(c >= '0' && c <= '9')) {
                return false;
            }
            else {
                hasDigit = true;
            }
        }

        if (!hasDigit) return false;
        return true;
    }

    static bool isValidLatitude(double lat) {
        return lat >= -90.0 && lat <= 90.0;
    }

    static bool isValidLongitude(double lon) {
        return lon >= -180.0 && lon <= 180.0;
    }

    static bool isValidBusNumber(const string& s) {
        if (s.empty() || s.length() > 20) return false;

        // Format: B101, 101A, etc.
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9'))) {
                return false;
            }
        }

        return true;
    }

    static bool isValidTime(int hour, int minute) {
        return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
    }

    static bool isValidCapacity(int capacity) {
        return capacity > 0 && capacity <= 200;
    }

    static bool isValidSpeed(double speed) {
        return speed > 0 && speed <= 120;
    }

    static bool isValidFarePerKm(double fare) {
        return fare > 0 && fare <= 100;
    }

    // Input prompts with validation
    static string getValidString(const string& prompt, int minLen = 1, int maxLen = 100) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidString(input, minLen, maxLen)) {
                return input;
            }
            cout << "Invalid input. Please enter a valid string (";
            cout << minLen << "-" << maxLen << " characters).\n";
        }
    }

    static int getValidInt(const string& prompt, int min = -2147483647, int max = 2147483647) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidInt(input, min, max)) {
                // Convert safely
                int value = 0;
                bool negative = false;
                size_t i = 0;

                if (input[0] == '-') {
                    negative = true;
                    i = 1;
                }
                else if (input[0] == '+') {
                    i = 1;
                }

                for (; i < input.length(); i++) {
                    value = value * 10 + (input[i] - '0');
                }

                if (negative) value = -value;
                return value;
            }
            else {
                cout << "Invalid input. Please enter an integer between " << min << " and " << max << ".\n";
            }
        }
    }

    static double getValidDouble(const string& prompt, double min = -1e308, double max = 1e308) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidDouble(input, min, max)) {
                // Simple double conversion
                double result = 0.0;
                double fraction = 0.0;
                double divisor = 1.0;
                bool negative = false;
                bool decimal = false;
                size_t i = 0;

                if (input[0] == '-') {
                    negative = true;
                    i = 1;
                }
                else if (input[0] == '+') {
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
                return result;
            }
            else {
                cout << "Invalid input. Please enter a number.\n";
            }
        }
    }

    static string getValidName(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidName(input)) {
                return input;
            }
            cout << "Invalid name. Please use only letters, numbers, spaces, hyphens, apostrophes, and periods.\n";
        }
    }

    static string getValidEmail(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidEmail(input)) {
                return input;
            }
            cout << "Invalid email format. Please enter a valid email address.\n";
        }
    }

    static string getValidPhone(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidPhone(input)) {
                return input;
            }
            cout << "Invalid phone number. Please enter 7-15 digits with optional spaces, hyphens, or parentheses.\n";
        }
    }

    static string getValidBusNumber(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = trim(input);

            if (isValidBusNumber(input)) {
                return input;
            }
            cout << "Invalid bus number. Use only letters and numbers (max 20 characters).\n";
        }
    }

    static void getValidTime(const string& prompt, int& hour, int& minute) {
        string input;
        while (true) {
            cout << prompt << " (HH:MM format): ";
            getline(cin, input);
            input = trim(input);

            if (input.length() == 5 && input[2] == ':') {
                string hourStr = input.substr(0, 2);
                string minuteStr = input.substr(3, 2);

                if (isValidInt(hourStr, 0, 23) && isValidInt(minuteStr, 0, 59)) {
                    // Convert hour
                    hour = (hourStr[0] - '0') * 10 + (hourStr[1] - '0');
                    // Convert minute
                    minute = (minuteStr[0] - '0') * 10 + (minuteStr[1] - '0');
                    return;
                }
            }
            cout << "Invalid time format. Please use HH:MM (24-hour format).\n";
        }
    }

    static string trim(const string& s) {
        if (s.empty()) return s;
        size_t first = 0, last = s.length() - 1;
        while (first <= last &&
            (s[first] == ' ' || s[first] == '\t' || s[first] == '\n' || s[first] == '\r')) {
            first++;
        }
        while (last >= first &&
            (s[last] == ' ' || s[last] == '\t' || s[last] == '\n' || s[last] == '\r')) {
            last--;
        }
        if (first > last) return "";
        string result = "";
        for (size_t i = first; i <= last; i++) {
            result += s[i];
        }
        return result;
    }
};

// String utility functions
string customSubstr(const string& s, size_t start, size_t length = string::npos) {
    if (start >= s.length()) return "";
    size_t end = (length == string::npos) ? s.length() : min(start + length, s.length());
    string result = "";
    for (size_t i = start; i < end; i++) {
        result += s[i];
    }
    return result;
}

string intToString(int num) {
    if (num == 0) return "0";
    bool negative = num < 0;
    if (negative) num = -num;

    char buffer[20];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    if (negative) buffer[i++] = '-';

    string result = "";
    for (int j = i - 1; j >= 0; j--) {
        result = result + buffer[j];
    }
    return result;
}

// ========================= HELPER STRUCTS =========================
struct Schedule {
    int startHour, startMin, endHour, endMin, interval;
    Schedule() : startHour(6), startMin(0), endHour(22), endMin(0), interval(15) {}
    Schedule(int sh, int sm, int eh, int em, int i)
        : startHour(sh), startMin(sm), endHour(eh), endMin(em), interval(i) {
    }

    bool isOperating(int hh, int mm) const {
        int cur = hh * 60 + mm, s = startHour * 60 + startMin, e = endHour * 60 + endMin;
        return cur >= s && cur <= e;
    }

    void display() const {
        cout << setw(2) << setfill('0') << startHour << ":"
            << setw(2) << startMin << " - "
            << setw(2) << endHour << ":"
            << setw(2) << endMin << " every " << interval << "m";
    }
};

struct Passenger {
    string name, contact, destName, boardingTime;
    int fromID, toID;
    double fare;
    Passenger() : fromID(-1), toID(-1), fare(0.0) {}
    Passenger(string n, string c, string d, int f, int t, double fr, string bt)
        : name(n), contact(c), destName(d), fromID(f), toID(t), fare(fr), boardingTime(bt) {
    }
};

class CircularPassengerQueue {
    Passenger* arr;
    int capacity, front, rear, count;
public:
    CircularPassengerQueue(int cap = 100) : capacity(cap > 0 ? cap : 100), front(0), rear(-1), count(0) {
        arr = new Passenger[capacity];
    }
    ~CircularPassengerQueue() { delete[] arr; }

    bool isFull() const { return count == capacity; }
    bool isEmpty() const { return count == 0; }

    bool enqueue(const Passenger& p) {
        if (isFull()) return false;
        rear = (rear + 1) % capacity;
        arr[rear] = p;
        count++;
        return true;
    }

    bool dequeue(Passenger& p) {
        if (isEmpty()) return false;
        p = arr[front];
        front = (front + 1) % capacity;
        count--;
        return true;
    }

    int size() const { return count; }

    void display() const {
        if (isEmpty()) { cout << "No passengers.\n"; return; }
        for (int i = 0, idx = front; i < count; i++, idx = (idx + 1) % capacity) {
            cout << i + 1 << ". " << arr[idx].name << " -> " << arr[idx].destName
                << " | Fare: " << arr[idx].fare << "\n";
        }
    }
};

struct RouteNode {
    int stopID;
    string name;
    double lat, lon;
    RouteNode* next;
    RouteNode(int id, const string& n, double la, double lo)
        : stopID(id), name(n), lat(la), lon(lo), next(nullptr) {
    }
};

class BusRoute {
public:
    RouteNode* head;
    RouteNode* tail;
    int length;

    BusRoute() : head(nullptr), tail(nullptr), length(0) {}
    ~BusRoute() { clear(); }

    void clear() {
        while (head) {
            RouteNode* t = head;
            head = head->next;
            delete t;
        }
        tail = nullptr;
        length = 0;
    }

    void addStop(int id, const string& name, double la, double lo) {
        RouteNode* n = new RouteNode(id, name, la, lo);
        if (!head) head = tail = n;
        else { tail->next = n; tail = n; }
        length++;
    }

    bool containsStop(int id) const {
        for (RouteNode* t = head; t; t = t->next)
            if (t->stopID == id) return true;
        return false;
    }

    int getStopID(int idx) const {
        RouteNode* t = head;
        for (int i = 0; t && i < idx; i++, t = t->next);
        return t ? t->stopID : -1;
    }

    string getStopName(int idx) const {
        RouteNode* t = head;
        for (int i = 0; t && i < idx; i++, t = t->next);
        return t ? t->name : "";
    }

    Coordinate getStopCoord(int idx) const {
        RouteNode* t = head;
        for (int i = 0; t && i < idx; i++, t = t->next);
        return t ? Coordinate{ t->lat, t->lon } : Coordinate{ 0,0 };
    }

    void displayRoute() const {
        if (!head) {
            cout << "  No route defined\n";
            return;
        }
        cout << "  Route: ";
        RouteNode* curr = head;
        int count = 0;
        while (curr) {
            if (count > 0) cout << " -> ";
            cout << curr->name;
            curr = curr->next;
            count++;
        }
        cout << "\n";
    }
};

struct Bus {
    string busNo, company, model;
    int capacity;
    Schedule schedule;
    double speed;
    int currentIdx;
    int currentStopID;
    string currentStopName;
    BusRoute route;
    CircularPassengerQueue passengers;
    RouteNode* historyTop;
    int totalServed;
    double totalRevenue;
    double totalDistance;
    bool isActive;

    Bus(const string& no, const string& com, const string& mod, int cap, Schedule sch, double spd)
        : busNo(no), company(com), model(mod), capacity(cap), schedule(sch), speed(spd),
        currentIdx(0), currentStopID(-1), passengers(cap), historyTop(nullptr),
        totalServed(0), totalRevenue(0.0), totalDistance(0.0), isActive(false) {
    }

    ~Bus() {
        while (historyTop) {
            RouteNode* t = historyTop;
            historyTop = historyTop->next;
            delete t;
        }
    }

    void pushHistory(int id, const string& name, const string& ts) {
        RouteNode* n = new RouteNode(id, name + " @ " + ts, 0, 0);
        n->next = historyTop;
        historyTop = n;
    }

    double occupancyRate() const {
        return capacity > 0 ? (passengers.size() * 100.0) / capacity : 0.0;
    }

    void displayStatus() const {
        cout << "\n=== BUS " << busNo << " ===\n";
        cout << "Company: " << company << "\n";
        cout << "Model: " << model << " | Capacity: " << capacity << "\n";
        cout << "Current Stop: " << (currentStopName.empty() ? "Not started" : currentStopName) << "\n";
        cout << "Schedule: "; schedule.display(); cout << "\n";
        cout << "Passengers: " << passengers.size() << "/" << capacity
            << " (" << fixed << setprecision(1) << occupancyRate() << "%)\n";
        cout << "Total Served: " << totalServed << " | Revenue: Rs." << totalRevenue << "\n";
        cout << "Distance Traveled: " << totalDistance << " km\n";
        cout << "Status: " << (isActive ? "ACTIVE" : "INACTIVE") << "\n";
        route.displayRoute();
    }
};

struct BusRefNode {
    string busNo;
    BusRefNode* next;
    BusRefNode(const string& n) : busNo(n), next(nullptr) {}
};

class BusRefList {
public:
    BusRefNode* head;
    int count;
    BusRefList() : head(nullptr), count(0) {}
    ~BusRefList() {
        while (head) {
            BusRefNode* t = head;
            head = head->next;
            delete t;
        }
    }
    void add(const string& busNo) {
        BusRefNode* n = new BusRefNode(busNo);
        n->next = head;
        head = n;
        count++;
    }
    bool remove(const string& busNo) {
        BusRefNode* prev = nullptr;
        BusRefNode* cur = head;
        while (cur) {
            if (cur->busNo == busNo) {
                if (prev) prev->next = cur->next;
                else head = cur->next;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }
    bool contains(const string& busNo) const {
        for (BusRefNode* t = head; t; t = t->next)
            if (t->busNo == busNo) return true;
        return false;
    }
    void print() const {
        if (!head) { cout << "None"; return; }
        BusRefNode* t = head;
        int i = 0;
        while (t) {
            if (i) cout << ", ";
            cout << t->busNo;
            t = t->next;
            ++i;
        }
    }
    int getCount() const { return count; }
};

struct TransportCompany {
    string name, contact, email;
    int fleetSize;
    double totalRevenue;
    TransportCompany() : fleetSize(0), totalRevenue(0.0) {}
    TransportCompany(string n, string c, string e)
        : name(n), contact(c), email(e), fleetSize(0), totalRevenue(0.0) {
    }

    void display() const {
        cout << "\n=== COMPANY: " << name << " ===\n";
        cout << "Contact: " << contact << "\n";
        cout << "Email: " << email << "\n";
        cout << "Fleet Size: " << fleetSize << " buses\n";
        cout << "Total Revenue: Rs." << fixed << setprecision(2) << totalRevenue << "\n";
    }
};

// ========================= HASH TABLES =========================
class HashTable_String_BusPtr {
    struct HashNode {
        string key;
        Bus* value;
        HashNode* next;
        HashNode(const string& k, Bus* v) : key(k), value(v), next(nullptr) {}
    };
    HashNode** table;
    int capacity, size;

    unsigned long hashString(const string& s) const {
        unsigned long hash = 5381;
        for (size_t i = 0; i < s.length(); i++) {
            hash = ((hash << 5) + hash) + (unsigned char)s[i];
        }
        return hash;
    }
public:
    HashTable_String_BusPtr(int cap = 101) : capacity(cap), size(0) {
        table = new HashNode * [capacity];
        for (int i = 0; i < capacity; i++) table[i] = nullptr;
    }

    ~HashTable_String_BusPtr() {
        for (int i = 0; i < capacity; i++) {
            while (table[i]) {
                HashNode* t = table[i];
                table[i] = table[i]->next;
                delete t;
            }
        }
        delete[] table;
    }

    void insert(const string& key, Bus* value) {
        int idx = hashString(key) % capacity;
        HashNode* n = new HashNode(key, value);
        n->next = table[idx];
        table[idx] = n;
        size++;
    }

    Bus* search(const string& key) const {
        int idx = hashString(key) % capacity;
        for (HashNode* cur = table[idx]; cur; cur = cur->next)
            if (cur->key == key) return cur->value;
        return nullptr;
    }

    void getAllBuses(Bus** busArray, int& count) const {
        count = 0;
        for (int i = 0; i < capacity; i++) {
            HashNode* curr = table[i];
            while (curr) {
                busArray[count++] = curr->value;
                curr = curr->next;
            }
        }
    }
};

class HashTable_Int_BusRefListPtr {
    struct HashNode {
        int key;
        BusRefList* value;
        HashNode* next;
        HashNode(int k, BusRefList* v) : key(k), value(v), next(nullptr) {}
    };
    HashNode** table;
    int capacity;
public:
    HashTable_Int_BusRefListPtr(int cap = 101) : capacity(cap) {
        table = new HashNode * [capacity];
        for (int i = 0; i < capacity; i++) table[i] = nullptr;
    }

    ~HashTable_Int_BusRefListPtr() {
        for (int i = 0; i < capacity; i++) {
            while (table[i]) {
                HashNode* t = table[i];
                table[i] = table[i]->next;
                delete t->value;
                delete t;
            }
        }
        delete[] table;
    }

    BusRefList* search(int key) const {
        int idx = (key < 0 ? -key : key) % capacity;
        for (HashNode* cur = table[idx]; cur; cur = cur->next)
            if (cur->key == key) return cur->value;
        return nullptr;
    }

    void addBusToStop(int stopID, const string& busNo) {
        int idx = (stopID < 0 ? -stopID : stopID) % capacity;
        BusRefList* list = search(stopID);
        if (!list) {
            list = new BusRefList();
            HashNode* n = new HashNode(stopID, list);
            n->next = table[idx];
            table[idx] = n;
        }
        if (!list->contains(busNo)) list->add(busNo);
    }

    void removeBusFromStop(int stopID, const string& busNo) {
        BusRefList* list = search(stopID);
        if (list) list->remove(busNo);
    }
};

class HashTable_String_TransportCompany {
    struct HashNode {
        string key;
        TransportCompany value;
        HashNode* next;
        HashNode(const string& k, TransportCompany v) : key(k), value(v), next(nullptr) {}
    };
    HashNode** table;
    int capacity, size;

    unsigned long hashString(const string& s) const {
        unsigned long hash = 5381;
        for (size_t i = 0; i < s.length(); i++) {
            hash = ((hash << 5) + hash) + (unsigned char)s[i];
        }
        return hash;
    }
public:
    HashTable_String_TransportCompany(int cap = 101) : capacity(cap), size(0) {
        table = new HashNode * [capacity];
        for (int i = 0; i < capacity; i++) table[i] = nullptr;
    }

    ~HashTable_String_TransportCompany() {
        for (int i = 0; i < capacity; i++) {
            while (table[i]) {
                HashNode* t = table[i];
                table[i] = table[i]->next;
                delete t;
            }
        }
        delete[] table;
    }

    void insert(const string& key, const TransportCompany& value) {
        int idx = hashString(key) % capacity;
        HashNode* n = new HashNode(key, value);
        n->next = table[idx];
        table[idx] = n;
        size++;
    }

    TransportCompany* search(const string& key) {
        int idx = hashString(key) % capacity;
        for (HashNode* cur = table[idx]; cur; cur = cur->next)
            if (cur->key == key) return &cur->value;
        return nullptr;
    }

    void displayAll() const {
        if (size == 0) {
            cout << "No companies registered.\n";
            return;
        }
        for (int i = 0; i < capacity; i++) {
            for (HashNode* c = table[i]; c; c = c->next) {
                c->value.display();
            }
        }
    }
};

class TransportSector {
private:
    Graph* masterGraph;
    HashTable_String_BusPtr busTable;
    HashTable_Int_BusRefListPtr stopBuses;
    HashTable_String_TransportCompany companies;

    string* allBusNumbers;
    int totalBuses;
    int busArrayCapacity;

    double farePerKm;
    int simulationHour, simulationMinute;

    // ================= PRIVATE HELPER FUNCTIONS =================
    double haversine(double lat1, double lon1, double lat2, double lon2) const {
        if (!InputValidator::isValidLatitude(lat1) || !InputValidator::isValidLongitude(lon1) ||
            !InputValidator::isValidLatitude(lat2) || !InputValidator::isValidLongitude(lon2)) {
            return INF;
        }

        const double R = 6371.0;
        double dLat = (lat2 - lat1) * 3.14159265358979323846 / 180.0;
        double dLon = (lon2 - lon1) * 3.14159265358979323846 / 180.0;
        double a = sin(dLat / 2) * sin(dLat / 2) +
            cos(lat1 * 3.14159265358979323846 / 180.0) *
            cos(lat2 * 3.14159265358979323846 / 180.0) *
            sin(dLon / 2) * sin(dLon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return R * c;
    }

    string trim(const string& s) const {
        return InputValidator::trim(s);
    }

    double calculateFare(int fromStopID, int toStopID) const {
        if (fromStopID < 0 || toStopID < 0 || fromStopID == toStopID) {
            return 0;
        }

        double distance = masterGraph->getDistance(fromStopID, toStopID);
        if (distance >= INF) {
            Coordinate fromCoord = masterGraph->getCoord(fromStopID);
            Coordinate toCoord = masterGraph->getCoord(toStopID);

            distance = haversine(fromCoord.lat, fromCoord.lon, toCoord.lat, toCoord.lon);
        }

        return max(0.0, distance * farePerKm);
    }

    double toDouble(string s) {
        double result = 0;
        double frac = 0;
        double div = 1;
        bool decimal = false;
        bool negative = false;
        int i = 0;
        if (s.length() > 0 && s[0] == '-') { negative = true; i = 1; }
        for (; i < s.length(); i++) {
            if (s[i] == '.') { decimal = true; continue; }
            if (s[i] < '0' || s[i] > '9') continue;
            if (!decimal) result = result * 10 + (s[i] - '0');
            else { frac = frac * 10 + (s[i] - '0'); div *= 10; }
        }
        result = result + frac / div;
        if (negative) result = -result;
        return result;
    }

    string removeSurroundingQuotes(const string& s) {
        if (s.length() >= 2 && s[0] == '"' && s[s.length() - 1] == '"') {
            string result = "";
            for (size_t i = 1; i < s.length() - 1; i++) {
                if (i + 1 < s.length() - 1 && s[i] == '"' && s[i + 1] == '"') {
                    result += '"';
                    i++; // skip the second "
                }
                else {
                    result += s[i];
                }
            }
            return result;
        }
        return s;
    }


    string removeQuotes(string s) {
        string r = "";
        for (size_t i = 0; i < s.length(); i++) {
            char c = s[i];
            if (c != '"' && c != ' ') r += c;
        }
        return r;
    }

    void splitCoordinates(string coord, string& lat, string& lon) {
        lat = ""; lon = "";
        bool afterComma = false;
        for (size_t i = 0; i < coord.length(); i++) {
            char c = coord[i];
            if (c == ',') { afterComma = true; continue; }
            if (!afterComma) lat += c;
            else lon += c;
        }
    }

    void splitCSV(const string& line, string fields[], int& count) {
        count = 0;
        string temp = "";
        bool inQuotes = false;
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            if (c == '"') {
                if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                    // Escaped quote inside field
                    temp += '"';
                    i++; // skip second "
                }
                else {
                    inQuotes = !inQuotes;
                }
            }
            else if (c == ',' && !inQuotes) {
                fields[count++] = temp;
                temp = "";
            }
            else {
                temp += c;
            }
        }
        fields[count++] = temp;
    }
    void parseRouteString(const string& routeStr, Bus* bus) {
        if (routeStr.empty() || !bus) return;

        string cleaned = "";
        // First pass: normalize arrows to " -> "
        for (size_t i = 0; i < routeStr.length(); ++i) {
            if (i + 1 < routeStr.length() && routeStr[i] == '-' && routeStr[i + 1] == '>') {
                cleaned += " -> ";
                i++; // skip the '>'
            }
            else if (routeStr[i] == '>') {
                cleaned += " -> "; // treat single '>' as arrow
            }
            else {
                cleaned += routeStr[i];
            }
        }

        // Now split by " -> "
        string temp = "";
        int prevStopID = -1;

        for (size_t i = 0; i < cleaned.length(); ++i) {
            if (cleaned.substr(i, 4) == " -> ") {
                string stopName = InputValidator::trim(temp);
                if (!stopName.empty()) {
                    int sid = masterGraph->findNodeByName(stopName);
                    if (sid != -1) {
                        Coordinate c = masterGraph->getCoord(sid);
                        bus->route.addStop(sid, masterGraph->getName(sid), c.lat, c.lon);
                        stopBuses.addBusToStop(sid, bus->busNo);
                        if (prevStopID != -1) {
                            masterGraph->connect(prevStopID, sid);
                        }
                        prevStopID = sid;
                    }
                    else {
                        cout << "Warning: Stop '" << stopName << "' not found in graph, skipping.\n";
                    }
                }
                temp = "";
                i += 3; // skip the " ->"
            }
            else {
                temp += cleaned[i];
            }
        }

        // Don't forget the last stop after final arrow
        string lastStop = InputValidator::trim(temp);
        if (!lastStop.empty()) {
            int sid = masterGraph->findNodeByName(lastStop);
            if (sid != -1) {
                Coordinate c = masterGraph->getCoord(sid);
                bus->route.addStop(sid, masterGraph->getName(sid), c.lat, c.lon);
                stopBuses.addBusToStop(sid, bus->busNo);
                if (prevStopID != -1) {
                    masterGraph->connect(prevStopID, sid);
                }
            }
            else {
                cout << "Warning: Stop '" << lastStop << "' not found in graph, skipping.\n";
            }
        }
    }

    bool validateBusData(const string& busNo, const string& company,
        const string& model, int capacity, double speed) {
        if (!InputValidator::isValidBusNumber(busNo)) {
            cout << "Error: Invalid bus number\n";
            return false;
        }

        if (!InputValidator::isValidString(company, 1, 50)) {
            cout << "Error: Invalid company name\n";
            return false;
        }

        if (!InputValidator::isValidString(model, 1, 30)) {
            cout << "Error: Invalid model name\n";
            return false;
        }

        if (!InputValidator::isValidCapacity(capacity)) {
            cout << "Error: Invalid capacity (1-200)\n";
            return false;
        }

        if (!InputValidator::isValidSpeed(speed)) {
            cout << "Error: Invalid speed (1-120 km/h)\n";
            return false;
        }

        return true;
    }

public:
    // ================= CONSTRUCTOR & DESTRUCTOR =================
    TransportSector(Graph* graph)
        : masterGraph(graph),
        busTable(101),
        stopBuses(101),
        companies(101),
        totalBuses(0),
        busArrayCapacity(100),
        farePerKm(5.0),
        simulationHour(6),
        simulationMinute(0)
    {
        if (!graph) {
            cout << "Error: TransportSector initialized with null graph\n";
        }

        allBusNumbers = new string[busArrayCapacity];
        for (int i = 0; i < busArrayCapacity; i++) {
            allBusNumbers[i] = "";
        }
    }

    ~TransportSector() {
        // Clean up buses
        for (int i = 0; i < totalBuses; i++) {
            Bus* b = busTable.search(allBusNumbers[i]);
            if (b) delete b;
        }
        delete[] allBusNumbers;
    }

    // ================= SETTERS WITH VALIDATION =================
    void setFarePerKm(double fare) {
        if (InputValidator::isValidFarePerKm(fare)) {
            farePerKm = fare;
            cout << "Fare per km set to: " << farePerKm << "\n";
        }
        else {
            cout << "Error: Invalid fare (0-100 Rs/km)\n";
        }
    }

    // ================= TIME MANAGEMENT =================
    string getCurrentTime() const {
        string hour = (simulationHour < 10 ? "0" : "") + intToString(simulationHour);
        string minute = (simulationMinute < 10 ? "0" : "") + intToString(simulationMinute);
        return hour + ":" + minute;
    }

    void advanceTime(int minutes) {
        if (minutes <= 0) {
            cout << "Error: Cannot advance by non-positive minutes\n";
            return;
        }

        if (minutes > 1440) {
            cout << "Warning: Advancing more than 24 hours at once\n";
        }

        simulationMinute += minutes;
        simulationHour += simulationMinute / 60;
        simulationMinute %= 60;
        simulationHour %= 24;
    }

    // ================= REGISTRATION LOGIC =================
    void registerCompany(const string& name, const string& contact = "N/A", const string& email = "N/A") {
        if (!InputValidator::isValidString(name, 1, 50)) {
            cout << "Error: Invalid company name\n";
            return;
        }

        if (companies.search(name)) {
            cout << "Company " << name << " already exists.\n";
            return;
        }

        string validContact = contact;
        string validEmail = email;

        if (contact != "N/A" && !InputValidator::isValidPhone(contact)) {
            cout << "Warning: Invalid contact format, using 'N/A'\n";
            validContact = "N/A";
        }

        if (email != "N/A" && !InputValidator::isValidEmail(email)) {
            cout << "Warning: Invalid email format, using 'N/A'\n";
            validEmail = "N/A";
        }

        TransportCompany company(name, validContact, validEmail);
        companies.insert(name, company);
        cout << " Registered company: " << name << "\n";
    }

    void registerBus(const string& busNo, const string& company,
        const string& model, int capacity, Schedule schedule,
        double speed, const string& startStop, const string& routeStr) {

        // Validate all inputs
        if (!validateBusData(busNo, company, model, capacity, speed)) {
            return;
        }

        if (busTable.search(busNo)) {
            cout << "Bus " << busNo << " already exists.\n";
            return;
        }

        TransportCompany* comp = companies.search(company);
        if (!comp) {
            cout << "Company not found, registering automatically...\n";
            registerCompany(company, "000-000", "info@" + company + ".com");
            comp = companies.search(company);
            if (!comp) {
                cout << "Error: Failed to register company\n";
                return;
            }
        }

        Bus* bus = new Bus(busNo, company, model, capacity, schedule, speed);

        // Set initial location
        if (!startStop.empty()) {
            int startID = masterGraph->findNodeByName(startStop);
            if (startID != -1) {
                bus->currentStopID = startID;
                bus->currentStopName = masterGraph->getName(startID);
                bus->currentIdx = 0;

                if (!bus->route.containsStop(startID)) {
                    Coordinate c = masterGraph->getCoord(startID);
                    bus->route.addStop(startID, masterGraph->getName(startID), c.lat, c.lon);
                    stopBuses.addBusToStop(startID, busNo);
                }
                bus->pushHistory(startID, bus->currentStopName, getCurrentTime());
            }
        }

        // Parse Route String
        if (!routeStr.empty()) {
            parseRouteString(routeStr, bus);
        }

        if (bus->route.length == 0) {
            cout << "Warning: Bus registered with empty route\n";
        }

        busTable.insert(busNo, bus);

        // Resize array if full
        if (totalBuses >= busArrayCapacity) {
            int newCap = busArrayCapacity * 2;
            string* newArr = new string[newCap];
            for (int i = 0; i < busArrayCapacity; i++) newArr[i] = allBusNumbers[i];
            for (int i = busArrayCapacity; i < newCap; i++) newArr[i] = "";
            delete[] allBusNumbers;
            allBusNumbers = newArr;
            busArrayCapacity = newCap;
        }

        allBusNumbers[totalBuses++] = busNo;
        comp->fleetSize++;
        cout << " Registered Bus " << busNo << " (" << bus->route.length << " stops)\n";
    }

    // Simplified wrapper for interactive mode
    void registerBus(string busID, string companyName, int capacity) {
        Schedule defaultSchedule(6, 0, 22, 0, 30);
        registerBus(busID, companyName, "Standard", capacity, defaultSchedule, 50.0, "", "");
    }

    // ================= OPERATIONAL LOGIC =================
    bool startBus(const string& busNo) {
        if (!InputValidator::isValidBusNumber(busNo)) {
            cout << "Error: Invalid bus number format\n";
            return false;
        }

        Bus* b = busTable.search(busNo);
        if (!b) {
            cout << "Bus " << busNo << " not found.\n";
            return false;
        }
        if (b->isActive) {
            cout << "Bus " << busNo << " is already active.\n";
            return false;
        }
        if (b->route.length == 0) {
            cout << "Error: Cannot start bus with empty route\n";
            return false;
        }

        b->isActive = true;
        cout << " Bus " << busNo << " started at " << getCurrentTime() << "\n";
        return true;
    }

    void stopBus(const string& busNo) {
        if (!InputValidator::isValidBusNumber(busNo)) {
            cout << "Error: Invalid bus number\n";
            return;
        }

        Bus* b = busTable.search(busNo);
        if (!b) {
            cout << "Bus " << busNo << " not found.\n";
            return;
        }

        if (!b->isActive) {
            cout << "Bus " << busNo << " is already inactive.\n";
            return;
        }

        b->isActive = false;
        cout << "Bus " << busNo << " stopped.\n";
    }

    void moveBus(const string& busNo) {
        if (!InputValidator::isValidBusNumber(busNo)) {
            cout << "Error: Invalid bus number\n";
            return;
        }

        Bus* b = busTable.search(busNo);
        if (!b || !b->isActive) {
            return;
        }
        if (b->route.length == 0) {
            cout << "Error: Bus " << busNo << " has no route defined\n";
            b->isActive = false;
            return;
        }

        // Move to next index (circular route)
        b->currentIdx = (b->currentIdx + 1) % b->route.length;

        int newStop = b->route.getStopID(b->currentIdx);
        string newName = b->route.getStopName(b->currentIdx);

        if (newStop == -1 || newName.empty()) {
            cout << "Error: Invalid stop in bus route\n";
            b->isActive = false;
            return;
        }

        // Calculate distance from previous stop
        int prevIdx = (b->currentIdx == 0) ? b->route.length - 1 : b->currentIdx - 1;
        Coordinate prev = b->route.getStopCoord(prevIdx);
        Coordinate curr = b->route.getStopCoord(b->currentIdx);

        double seg = haversine(prev.lat, prev.lon, curr.lat, curr.lon);
        if (seg >= 0 && seg < 1000) {
            b->totalDistance += seg;
        }

        b->currentStopID = newStop;
        b->currentStopName = newName;
        b->pushHistory(newStop, newName, getCurrentTime());
        cout << "Bus " << busNo << " -> " << newName << " at " << getCurrentTime() << "\n";

        // Drop off passengers at this stop
        dropOffPassengers(b);
    }

    void moveBuses() {
        if (totalBuses == 0) {
            return;
        }

        for (int i = 0; i < totalBuses; i++) {
            if (allBusNumbers[i] != "") {
                moveBus(allBusNumbers[i]);
            }
        }
    }

    // ================= PASSENGER LOGIC =================
    bool addPassenger(const string& name, const string& busNo, const string& from, const string& to) {
        // Validate inputs
        if (!InputValidator::isValidName(name)) {
            cout << "Error: Invalid passenger name\n";
            return false;
        }

        if (!InputValidator::isValidBusNumber(busNo)) {
            cout << "Error: Invalid bus number\n";
            return false;
        }

        if (!InputValidator::isValidString(from, 1, 50) || !InputValidator::isValidString(to, 1, 50)) {
            cout << "Error: Invalid stop names\n";
            return false;
        }

        Bus* b = busTable.search(busNo);
        if (!b || !b->isActive) {
            cout << "Bus " << busNo << " is unavailable/inactive.\n";
            return false;
        }

        int fromID = masterGraph->findNodeByName(from);
        int toID = masterGraph->findNodeByName(to);

        if (fromID == -1 || toID == -1) {
            cout << "Invalid stops: " << from << " or " << to << "\n";
            return false;
        }
        if (!b->route.containsStop(fromID) || !b->route.containsStop(toID)) {
            cout << "Bus " << busNo << " doesn't serve these stops.\n";
            return false;
        }
        if (b->passengers.isFull()) {
            cout << "Bus " << busNo << " is full.\n";
            return false;
        }
        if (fromID == toID) {
            cout << "Source and destination cannot be the same\n";
            return false;
        }

        double fare = calculateFare(fromID, toID);
        Passenger p(name, "N/A", to, fromID, toID, fare, getCurrentTime());

        if (b->passengers.enqueue(p)) {
            b->totalServed++;
            b->totalRevenue += fare;
            TransportCompany* comp = companies.search(b->company);
            if (comp) comp->totalRevenue += fare;
            cout << " Passenger " << name << " boarded bus " << busNo << ". Fare: Rs." << fare << "\n";
            return true;
        }
        return false;
    }

    void dropOffPassengers(Bus* b) {
        if (!b) return;

        CircularPassengerQueue tempQueue(b->capacity);
        Passenger p;
        int dropped = 0;

        while (b->passengers.dequeue(p)) {
            if (p.toID == b->currentStopID) {
                cout << " Passenger " << p.name << " reached " << p.destName << "\n";
                dropped++;
            }
            else {
                tempQueue.enqueue(p);
            }
        }
        // Put remaining passengers back
        while (tempQueue.dequeue(p)) b->passengers.enqueue(p);

        if (dropped > 0) {
            b->totalServed += dropped;
        }
    }

    void dropOffAllPassengers() {
        if (totalBuses == 0) return;

        int totalDropped = 0;
        for (int i = 0; i < totalBuses; i++) {
            Bus* b = busTable.search(allBusNumbers[i]);
            if (b && b->isActive) {
                int before = b->passengers.size();
                dropOffPassengers(b);
                totalDropped += (before - b->passengers.size());
            }
        }
        if (totalDropped > 0) cout << " Dropped off " << totalDropped << " passengers across system.\n";
    }

    // ================= SIMULATION CONTROL =================
    void simulateTimeStep(int minutes = 5) {
        if (minutes <= 0) {
            cout << "Error: Time step must be positive\n";
            return;
        }

        if (minutes > 480) {
            cout << "Warning: Simulating more than 8 hours at once\n";
        }

        cout << "\n=== Simulating " << minutes << " minutes ===\n";
        advanceTime(minutes);
        cout << "Current Time: " << getCurrentTime() << "\n";
        moveBuses();
    }

    void simulateTimeStepWithDropOff(int minutes) {
        if (minutes <= 0) {
            cout << "Error: Time step must be positive\n";
            return;
        }

        cout << "\n=== Detailed Simulation (" << minutes << " min) ===\n";
        for (int i = 0; i < minutes; i++) {
            advanceTime(1);
            moveBuses();
        }
        dropOffAllPassengers();
        cout << " Simulation Ended. Time: " << getCurrentTime() << "\n";
    }

    // ================= INTERACTIVE METHODS WITH VALIDATION =================
    void displayBusStatusInteractive() {
        string busNo = InputValidator::getValidBusNumber("Enter Bus Number: ");
        Bus* b = busTable.search(busNo);
        if (b) b->displayStatus();
        else cout << "Bus '" << busNo << "' not found.\n";
    }

    void registerStopInteractive() {
        string name = InputValidator::getValidName("Stop Name: ");
        double lat = InputValidator::getValidDouble("Latitude (-90 to 90): ", -90.0, 90.0);
        double lon = InputValidator::getValidDouble("Longitude (-180 to 180): ", -180.0, 180.0);

        int id = masterGraph->addLocation(name, lat, lon, STOP);
        if (id >= 0) {
            cout << " Stop Registered: " << name << " (ID: " << id << ")\n";
        }
        else {
            cout << "Error: Failed to register stop\n";
        }
    }

    void connectStopsInteractive() {
        int nodeCount = masterGraph->getNodeCount();
        if (nodeCount < 2) {
            cout << "Need at least 2 stops to connect\n";
            return;
        }

        int u = InputValidator::getValidInt("Enter Stop ID 1: ", 0, nodeCount - 1);
        int v = InputValidator::getValidInt("Enter Stop ID 2: ", 0, nodeCount - 1);

        if (u == v) {
            cout << "Cannot connect a stop to itself\n";
            return;
        }

        masterGraph->connect(u, v);
        cout << " Stops connected.\n";
    }

    void registerCompanyInteractive() {
        string name = InputValidator::getValidString("Enter Company Name: ", 1, 50);
        string contact = InputValidator::getValidPhone("Enter Contact (or press Enter for N/A): ");
        if (contact.empty()) contact = "N/A";

        string email = InputValidator::getValidEmail("Enter Email (or press Enter for N/A): ");
        if (email.empty()) email = "N/A";

        registerCompany(name, contact, email);
    }

    void registerBusInteractive() {
        string busID = InputValidator::getValidBusNumber("Enter Bus ID: ");
        string companyName = InputValidator::getValidString("Enter Company Name: ", 1, 50);
        int capacity = InputValidator::getValidInt("Enter Bus Capacity (1-200): ", 1, 200);

        // Get schedule
        int startHour, startMin, endHour, endMin;
        cout << "\nEnter Schedule Start Time:\n";
        InputValidator::getValidTime("  Start", startHour, startMin);
        InputValidator::getValidTime("  End", endHour, endMin);

        int interval = InputValidator::getValidInt("Enter Interval (minutes, 1-240): ", 1, 240);

        string startStop;
        cout << "Enter Starting Stop (press Enter if none): ";
        getline(cin, startStop);
        startStop = InputValidator::trim(startStop);

        string routeStr;
        cout << "Enter Route (e.g., Stop1 -> Stop2 -> Stop3, press Enter if none): ";
        getline(cin, routeStr);
        routeStr = InputValidator::trim(routeStr);

        double speed = InputValidator::getValidDouble("Enter Speed (km/h, 1-120): ", 1.0, 120.0);
        string model = InputValidator::getValidString("Enter Model: ", 1, 30);

        Schedule sch(startHour, startMin, endHour, endMin, interval);
        registerBus(busID, companyName, model, capacity, sch, speed, startStop, routeStr);
    }

    void startBusInteractive() {
        string busID = InputValidator::getValidBusNumber("Enter Bus ID to start: ");
        startBus(busID);
    }

    void addPassengerInteractive() {
        string name = InputValidator::getValidName("Passenger Name: ");
        string busID = InputValidator::getValidBusNumber("Bus ID: ");

        // Display available stops
        cout << "\nAvailable Stops:\n";
        int nodeCount = masterGraph->getNodeCount();
        for (int i = 0; i < nodeCount; i++) {
            if (masterGraph->getType(i) == STOP) {
                cout << "  " << masterGraph->getName(i) << "\n";
            }
        }

        string source = InputValidator::getValidString("Source Stop: ", 1, 50);
        string dest = InputValidator::getValidString("Destination Stop: ", 1, 50);

        addPassenger(name, busID, source, dest);
    }

    void listBusesAtStopInteractive() {
        string stopName = InputValidator::getValidString("Enter Stop Name: ", 1, 50);
        listBusesAtStop(stopName);
    }

    void findShortestPathInteractive() {
        // Display available stops first
        cout << "\nAvailable Stops:\n";
        int nodeCount = masterGraph->getNodeCount();
        bool hasStops = false;
        for (int i = 0; i < nodeCount; i++) {
            if (masterGraph->getType(i) == STOP) {
                cout << "  " << masterGraph->getName(i) << "\n";
                hasStops = true;
            }
        }

        if (!hasStops) {
            cout << "No stops available. Please register stops first.\n";
            return;
        }

        string srcName = InputValidator::getValidString("Source Stop: ", 1, 50);
        string destName = InputValidator::getValidString("Destination Stop: ", 1, 50);

        int srcID = masterGraph->findNodeByName(srcName);
        int destID = masterGraph->findNodeByName(destName);

        if (srcID == -1 || destID == -1) {
            cout << "One or both stops not found.\n";
            return;
        }

        if (srcID == destID) {
            cout << "Source and destination are the same.\n";
            return;
        }

        double* dist = new double[nodeCount];
        int* prev = new int[nodeCount];

        masterGraph->dijkstra(srcID, dist, prev);

        if (dist[destID] >= INF) {
            cout << "No path found between " << masterGraph->getName(srcID)
                << " and " << masterGraph->getName(destID) << ".\n";
            cout << "(Ensure stops are connected via 'Connect Two Stops' or by registering bus routes)\n";
        }
        else {
            cout << "Shortest path (" << fixed << setprecision(2) << dist[destID] << " km):\n  ";

            int pathCapacity = nodeCount;
            int* path = new int[pathCapacity];
            int pathLen = 0;
            int current = destID;

            while (current != -1 && current != srcID) {
                path[pathLen++] = current;
                current = prev[current];
            }
            path[pathLen++] = srcID;

            for (int i = pathLen - 1; i >= 0; i--) {
                cout << masterGraph->getName(path[i]);
                if (i > 0) cout << " -> ";
            }
            cout << "\n";
            delete[] path;
        }

        delete[] dist;
        delete[] prev;
    }

    void findNearestStopInteractive() {
        double lat = InputValidator::getValidDouble("Enter Latitude (-90 to 90): ", -90.0, 90.0);
        double lon = InputValidator::getValidDouble("Enter Longitude (-180 to 180): ", -180.0, 180.0);

        int nodeCount = masterGraph->getNodeCount();
        int nearestID = -1;
        double minDist = INF;

        for (int i = 0; i < nodeCount; i++) {
            if (masterGraph->getType(i) == STOP) {
                Coordinate c = masterGraph->getCoord(i);
                double dist = haversine(lat, lon, c.lat, c.lon);
                if (dist < minDist) {
                    minDist = dist;
                    nearestID = i;
                }
            }
        }

        if (nearestID != -1) {
            Coordinate c = masterGraph->getCoord(nearestID);
            cout << " Nearest Stop: " << masterGraph->getName(nearestID)
                << " (ID: " << nearestID << ")\n";
            cout << " Distance: " << fixed << setprecision(2) << minDist << " km\n";
        }
        else {
            cout << "No stops found in the network.\n";
        }
    }

    // ================= FILE I/O =================
    void loadStopsCSV(const string& filename) {
        if (filename.empty()) {
            cout << "Error: Filename cannot be empty\n";
            return;
        }
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << " Could not open stop file: " << filename << endl;
            return;
        }
        string line;
        int loaded = 0, skipped = 0, lineNum = 0;
        // Skip header if exists
        if (getline(file, line)) {
            lineNum++;
        }
        while (getline(file, line)) {
            lineNum++;
            line = trim(line); // remove leading/trailing whitespace from whole line
            if (line.empty() || line[0] == '#') continue; // skip empty or comments

            string fields[10];
            int count = 0;
            splitCSV(line, fields, count);
            if (count < 3) {
                cout << "Line " << lineNum << ": Insufficient fields, skipping\n";
                skipped++;
                continue;
            }

            // Field 0: ID (optional, ignore)
            string rawName = fields[1];
            string name = trim(removeSurroundingQuotes(rawName));

            if (!InputValidator::isValidString(name, 1, 50)) {
                cout << "Line " << lineNum << ": Invalid stop name '" << name << "', skipping\n";
                skipped++;
                continue;
            }

            double lat = 0.0, lon = 0.0;
            if (count >= 4) {
                // Separate lat and lon fields
                string latStr = trim(removeSurroundingQuotes(fields[2]));
                string lonStr = trim(removeSurroundingQuotes(fields[3]));
                lat = toDouble(latStr);
                lon = toDouble(lonStr);
            }
            else if (count == 3) {
                // Combined coordinate field like "(28.7041, 77.1025)" or "28.7041,77.1025"
                string coord = trim(removeSurroundingQuotes(fields[2]));
                string latStr, lonStr;
                splitCoordinates(coord, latStr, lonStr);
                latStr = trim(latStr);
                lonStr = trim(lonStr);
                lat = toDouble(latStr);
                lon = toDouble(lonStr);
            }

            if (!InputValidator::isValidLatitude(lat) || !InputValidator::isValidLongitude(lon)) {
                cout << "Line " << lineNum << ": Invalid coordinates (" << lat << ", " << lon << "), skipping\n";
                skipped++;
                continue;
            }

            int id = masterGraph->addLocation(name, lat, lon, STOP);
            if (id >= 0) {
                loaded++;
            }
            else {
                cout << "Line " << lineNum << ": Failed to add stop '" << name << "', skipping\n";
                skipped++;
            }
        }
        file.close();
        cout << "\n Loaded " << loaded << " stops successfully.\n";
        if (skipped > 0) {
            cout << " Skipped " << skipped << " invalid entries.\n";
        }
    }

    // Load buses from string format
    void loadBusesFromString() {
        cout << "Loading buses from provided list...\n";

        // BusNo Company CurrentStop Route
        registerBus("B101", "MetroBus", "Standard", 50, Schedule(), 40.0, "Stop3", "Stop1 -> Stop2 -> Stop3 -> Stop4");
        registerBus("B102", "CapitalTrans", "Standard", 50, Schedule(), 40.0, "Stop1", "Stop1 -> Stop5 -> Stop8");
        registerBus("B201", "GreenWheels", "Standard", 50, Schedule(), 40.0, "Stop6", "Stop6 -> Stop2 -> Stop7 -> Stop9");
        registerBus("B305", "NorthFleet", "Standard", 50, Schedule(), 40.0, "Stop4", "Stop4 -> Stop10 -> Stop11");
        registerBus("B402", "SkyLine", "Standard", 50, Schedule(), 40.0, "Stop8", "Stop8 -> Stop6 -> Stop3");

        cout << "5 buses loaded successfully.\n";
    }

    void loadFromFile(const string& filename) {
        if (filename.empty()) {
            cout << "Error: Filename cannot be empty\n";
            return;
        }

        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Cannot open file: " << filename << "\n";
            return;
        }

        string line;
        int lineNum = 0;
        while (getline(file, line)) {
            lineNum++;
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            // Parse CSV line
            string fields[10];
            int count = 0;
            splitCSV(line, fields, count);

            if (count < 4) {
                cout << "Line " << lineNum << ": Invalid format\n";
                continue;
            }

            string busNo = trim(fields[0]);
            string company = trim(fields[1]);
            string currentStop = trim(fields[2]);
            string routeStr = "";

            // Combine remaining fields for route
            for (int i = 3; i < count; i++) {
                routeStr += trim(fields[i]);
                if (i < count - 1) routeStr += " ";
            }

            // Just pass raw routeStr — new parseRouteString handles everything
            string cleanedRoute = routeStr;  // No need to pre-process anymore
            registerBus(busNo, company, "ModelX", 50, Schedule(), 40.0, currentStop, cleanedRoute);
        }
        file.close();
        cout << " Transport data loaded from: " << filename << "\n";
    }

    // ================= SYSTEM CHECKS =================
    bool isSystemReady() const {
        if (totalBuses == 0) {
            cout << "No buses registered in system.\n";
            return false;
        }

        int activeBuses = 0;
        for (int i = 0; i < totalBuses; i++) {
            Bus* b = busTable.search(allBusNumbers[i]);
            if (b && b->isActive) activeBuses++;
        }

        if (activeBuses == 0) {
            cout << "No active buses. Start buses to begin simulation.\n";
            return false;
        }

        return true;
    }

    void displaySystemStats() const {
        cout << "\n TRANSPORT STATISTICS \n";
        cout << "Current Time: " << getCurrentTime() << "\n";
        cout << "Total Buses: " << totalBuses << "\n";

        int activeBuses = 0;
        int totalPassengers = 0;
        for (int i = 0; i < totalBuses; i++) {
            Bus* b = busTable.search(allBusNumbers[i]);
            if (b) {
                if (b->isActive) activeBuses++;
                totalPassengers += b->passengers.size();
            }
        }
        cout << "Active Buses: " << activeBuses << "\n";
        cout << "Total Passengers on Buses: " << totalPassengers << "\n";
        cout << "Total Stops: " << masterGraph->getNodeCount() << "\n";
        cout << "Fare per km: Rs." << farePerKm << "\n";

        cout << "\n--- COMPANIES ---\n";
        companies.displayAll();
    }

    // ================= UTILITY METHODS =================
    void listBusesAtStop(int stopID) {
        BusRefList* list = stopBuses.search(stopID);
        if (!list || list->getCount() == 0) {
            cout << "No buses serve stop ID " << stopID << ".\n";
            return;
        }
        cout << "Buses at " << masterGraph->getName(stopID) << ": ";
        list->print();
        cout << "\n";
    }

    void listBusesAtStop(const string& stopName) {
        int id = masterGraph->findNodeByName(stopName);
        if (id == -1) cout << "Stop '" << stopName << "' not found.\n";
        else listBusesAtStop(id);
    }

    void displayAllStops() {
        cout << "\n=== ALL BUS STOPS ===\n";
        int nodeCount = masterGraph->getNodeCount();
        int count = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (masterGraph->getType(i) == STOP) {
                Coordinate c = masterGraph->getCoord(i);
                cout << "ID: " << i << " | " << masterGraph->getName(i)
                    << " | (" << c.lat << ", " << c.lon << ")\n";
                count++;
            }
        }
        cout << "Total: " << count << " stops\n";
    }
    // Bonus: Real-time route simulation – looks like the bus is actually moving!
    void simulateBusJourney(const string& busNo, int steps = 12) {
        Bus* b = busTable.search(busNo);
        if (!b) {
            cout << "Bus " << busNo << " not found!\n";
            return;
        }
        if (b->route.length == 0) {
            cout << "No route defined for bus " << busNo << "\n";
            return;
        }

        cout << "\n==========================================\n";
        cout << "   Real-Time Bus Journey Simulation\n";
        cout << "   Bus " << busNo << " (" << b->company << ")\n";
        cout << "==========================================\n";

        int totalStops = b->route.length;
        int current = b->currentIdx;

        for (int i = 0; i < steps; i++) {
            // Clear screen (works in all Windows compilers)
            system("cls");

            cout << "\n   Time: " << getCurrentTime() << "   Bus: " << busNo << "\n\n";

            // Show the full route with moving bus marker
            cout << "   ";
            for (int s = 0; s < totalStops; s++) {
                string name = b->route.getStopName(s);
                if (s == current) {
                    cout << " [" << name << "] ";     // Bus is here!
                }
                else {
                    cout << name << " ";
                }
                if (s < totalStops - 1) cout << "→ ";
            }
            cout << "\n\n";

            // Show passengers
            cout << "   Passengers on board: " << b->passengers.size() << "/" << b->capacity;
            cout << "   (" << fixed << setprecision(1) << b->occupancyRate() << "%)\n";

            // Show distance traveled
            cout << "   Distance so far: " << fixed << setprecision(1) << b->totalDistance << " km\n\n";

            // Move bus one step
            moveBus(busNo);
            advanceTime(2);  // every 2 minutes

            // Small delay so it looks alive (remove if teacher doesn't like delay)
            Sleep(800);  // 0.8 seconds – feels real
        }

        cout << "\n   Journey simulation finished!\n";
        cout << "   Bus is now at: " << b->currentStopName << "\n\n";
    }
    // Inside class TransportSector { ... public: ... }

    void displayAllBuses() const {
        if (totalBuses == 0) {
            cout << "No buses registered in the system.\n";
            return;
        }

        cout << "\n=====================================\n";
        cout << "         ALL REGISTERED BUSES        \n";
        cout << "=====================================\n";
        cout << "Total Buses: " << totalBuses << "\n\n";

        for (int i = 0; i < totalBuses; i++) {
            const string& busNo = allBusNumbers[i];
            if (busNo.empty()) continue;

            Bus* b = busTable.search(busNo);
            if (!b) continue;

            cout << "Bus #" << (i + 1) << ": " << busNo << "\n";
            cout << "  Company: " << b->company << "\n";
            cout << "  Model: " << b->model << "\n";
            cout << "  Capacity: " << b->capacity << "\n";
            cout << "  Speed: " << b->speed << " km/h\n";
            cout << "  Schedule: ";
            b->schedule.display();
            cout << "\n";
            cout << "  Route Stops: " << b->route.length << "\n";
            cout << "  Current Stop: " << (b->currentStopName.empty() ? "Not started" : b->currentStopName) << "\n";
            cout << "  Passengers: " << b->passengers.size() << "/" << b->capacity
                << " (" << fixed << setprecision(1) << b->occupancyRate() << "%)\n";
            cout << "  Total Served: " << b->totalServed << "\n";
            cout << "  Revenue: Rs." << fixed << setprecision(2) << b->totalRevenue << "\n";
            cout << "  Distance: " << fixed << setprecision(1) << b->totalDistance << " km\n";
            cout << "  Status: " << (b->isActive ? "ACTIVE" : "INACTIVE") << "\n";
            cout << "  Route: ";
            b->route.displayRoute();
            cout << "-------------------------------------\n";
        }
    }
    // Getter methods
    int getTotalBuses() const { return totalBuses; }
    double getFarePerKm() const { return farePerKm; }
    string getCurrentTimeStr() const { return getCurrentTime(); }
};