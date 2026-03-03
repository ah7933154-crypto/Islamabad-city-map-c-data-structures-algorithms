#pragma once
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>    // For sqrt, pow (Distance calculation)
#include <iomanip>  
#include <limits>
#include <cctype>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"
#include"Smartcity.h"
using namespace std;

bool isSpaceManual(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

char toLowerChar(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

// Manual string trim (removes leading/trailing spaces)
string manualTrim(const string& s) {
    if (s.empty()) return "";
    int start = 0;
    while (start < (int)s.length() && isSpaceManual(s[start])) start++;

    int end = (int)s.length() - 1;
    while (end >= start && isSpaceManual(s[end])) end--;

    if (start > end) return "";
    return s.substr(start, end - start + 1);
}

// Manual string to lowercase
string manualToLower(string s) {
    for (size_t i = 0; i < s.length(); ++i) {
        s[i] = toLowerChar(s[i]);
    }
    return s;
}

// Manual Case-Insensitive Substring Search
// Returns true if 'pattern' is found inside 'text'
bool manualContains(const string& text, const string& pattern) {
    if (pattern.length() > text.length()) return false;
    if (pattern.empty()) return true;

    for (size_t i = 0; i <= text.length() - pattern.length(); ++i) {
        size_t j = 0;
        while (j < pattern.length()) {
            if (toLowerChar(text[i + j]) != toLowerChar(pattern[j])) {
                break;
            }
            j++;
        }
        if (j == pattern.length()) return true;
    }
    return false;
}




// ==========================================
// 1. DATA STRUCTURES (N-ARY TREE HERARCHY)
// ==========================================

// Level 4: Student (Leaf Node)
struct StudentNode {
    int id;
    string name;
    StudentNode* next;

    StudentNode(int sID, const string& sName) : id(sID), name(sName), next(nullptr) {}
};

// Level 3: Class (Child of Department)
struct ClassNode {
    string className;
    string teacher; // Faculty Member
    StudentNode* students;
    ClassNode* next;

    ClassNode(const string& cName, const string& tName)
        : className(cName), teacher(tName), students(nullptr), next(nullptr) {
    }
};

// Level 2: Department (Child of School)
struct DepartmentNode {
    string deptName;
    ClassNode* classes;
    DepartmentNode* next;

    DepartmentNode(const string& dName) : deptName(dName), classes(nullptr), next(nullptr) {}
};

// Level 1: School (Root)
struct School {
    string schoolID;
    string name;
    string sector;
    double rating;
    double lat, lon; // Coordinates for "Nearest School"

    // Subjects array (Fixed size for simplicity)
    string subjects[10];
    int subjCount;

    DepartmentNode* departments; // Tree Children

    School* next;       // Hash Table Chain for school-id table
    School* masterNext; // Master List for iteration (linked list of all schools)

    School()
        : schoolID(""), name(""), sector(""), rating(0.0), lat(0.0), lon(0.0),
        subjCount(0), departments(nullptr), next(nullptr), masterNext(nullptr) {
    }

    void setCoordinatesFromSector() {
        if (sector == "G-10") { lat = 33.684; lon = 73.025; }
        else if (sector == "F-10") { lat = 33.691; lon = 73.019; }
        else if (sector == "F-8") { lat = 33.709; lon = 73.037; }
        else if (sector == "G-9") { lat = 33.700; lon = 73.030; }
        else if (sector == "F-7") { lat = 33.721; lon = 73.052; }
        else if (sector == "F-6") { lat = 33.727; lon = 73.075; }
        else { lat = 33.700; lon = 73.000; } // Default center
    }
};

// ==========================================
// 2. SCHOOL HASH TABLE (By schoolID for O(1) lookup)
// ==========================================
class SchoolHashTable {
private:
    static const int SIZE = 211; // prime-ish table size
    School* table[SIZE];

    unsigned long hash(const string& key) {
        unsigned long h = 5381;
        for (char c : key) h = ((h << 5) + h) + (unsigned char)c;
        return h % SIZE;
    }

public:
    SchoolHashTable() { for (int i = 0; i < SIZE; ++i) table[i] = nullptr; }

    void insert(School* s) {
        if (!s) return;
        unsigned long idx = hash(s->schoolID);
        s->next = table[idx];
        table[idx] = s;
    }

    School* find(const string& id) {
        if (!isValidSchoolID(id)) return nullptr;
        unsigned long idx = hash(id);
        School* p = table[idx];
        while (p) {
            if (p->schoolID == id) return p;
            p = p->next;
        }
        return nullptr;
    }
};

// ==========================================
// 3. SUBJECT -> Schools Hash Table
//    For fast search by subject (secondary index).
// ==========================================
struct SchoolListNode {
    School* school;
    SchoolListNode* next;
    SchoolListNode(School* s) : school(s), next(nullptr) {}
};

struct SubjectNode {
    string subjectName;      // normalized lowercase
    SchoolListNode* schools; // chain of schools offering this subject
    SubjectNode* next;       // chain in bucket
    SubjectNode(const string& name) : subjectName(name), schools(nullptr), next(nullptr) {}
};

class SubjectHashTable {
private:
    static const int SIZE = 137; // prime-ish
    SubjectNode* table[SIZE];

    unsigned long hash(const string& key) {
        unsigned long h = 2166136261u;
        for (char c : key) h = (h ^ (unsigned char)c) * 16777619u;
        return h % SIZE;
    }

public:
    SubjectHashTable() { for (int i = 0; i < SIZE; ++i) table[i] = nullptr; }

    // Insert school into the subject's linked-list
    void insertSchoolForSubject(const string& subjectRaw, School* s) {
        if (!s) return;
        // Use Manual Trim and ToLower
        string subject = manualToLower(manualTrim(subjectRaw));
        if (subject.empty() || !isValidSubject(subject)) return;

        unsigned long idx = hash(subject);
        SubjectNode* cur = table[idx];
        while (cur) {
            if (cur->subjectName == subject) break;
            cur = cur->next;
        }
        if (!cur) {
            cur = new SubjectNode(subject);
            cur->next = table[idx];
            table[idx] = cur;
        }
        // add school to school's list if not already present
        SchoolListNode* sl = cur->schools;
        while (sl) {
            if (sl->school == s) return; // already present
            sl = sl->next;
        }
        SchoolListNode* newNode = new SchoolListNode(s);
        newNode->next = cur->schools;
        cur->schools = newNode;
    }

    // Find subject node (by raw subject string)
    SubjectNode* findSubject(const string& subjectRaw) {
        string subject = manualToLower(manualTrim(subjectRaw));
        if (subject.empty() || !isValidSubject(subject)) return nullptr;
        unsigned long idx = hash(subject);
        SubjectNode* cur = table[idx];
        while (cur) {
            if (cur->subjectName == subject) return cur;
            cur = cur->next;
        }
        return nullptr;
    }
};

// ==========================================
// 4. MAX HEAP (Priority Queue for Ranking)
// ==========================================
class SchoolMaxHeap {
public:
    School* heap[500]; // capacity; adjust if large datasets expected
    int size;

    SchoolMaxHeap() : size(0) {}

    void insert(School* s) {
        if (!s || size >= 500) return;
        heap[size] = s;
        int i = size++;
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[i]->rating > heap[p]->rating) {
                School* tmp = heap[i]; heap[i] = heap[p]; heap[p] = tmp;
                i = p;
            }
            else break;
        }
    }

    void displayTop(int n) {
        if (n <= 0) {
            cout << "Error: Number of schools to display must be positive.\n";
            return;
        }

        if (size == 0) {
            cout << "No schools to rank.\n";
            return;
        }

        if (n > size) {
            cout << "Note: Only " << size << " schools available.\n";
            n = size;
        }

        // Make a shallow copy of heap pointers
        School* tmpArr[500];
        for (int i = 0; i < size; ++i) tmpArr[i] = heap[i];
        int tmpSize = size;

        cout << "\n===== TOP RATED SCHOOLS (Priority Queue) =====\n";
        for (int k = 0; k < n && tmpSize > 0; ++k) {
            School* maxS = tmpArr[0];
            cout << (k + 1) << ". " << maxS->name << " (ID: " << maxS->schoolID << ", Rating: " << maxS->rating << ")\n";

            tmpArr[0] = tmpArr[tmpSize - 1];
            tmpSize--;
            int i = 0;
            while (true) {
                int l = 2 * i + 1, r = 2 * i + 2, largest = i;
                if (l < tmpSize && tmpArr[l]->rating > tmpArr[largest]->rating) largest = l;
                if (r < tmpSize && tmpArr[r]->rating > tmpArr[largest]->rating) largest = r;
                if (largest != i) {
                    School* sw = tmpArr[i]; tmpArr[i] = tmpArr[largest]; tmpArr[largest] = sw;
                    i = largest;
                }
                else break;
            }
        }
    }
};

// ==========================================
// 5. EDUCATION SECTOR - main controller
// ==========================================
class EducationSector {
private:
    School* allSchoolsHead; // master linked list
    SchoolHashTable hashTable; // by schoolID
    SubjectHashTable subjectIndex; // subject -> schools
    SchoolMaxHeap rankingHeap;
    Graph* graph;

    // CSV parsing helper - fills destFields[] and returns column count
    void parseCSVLine(const string& line, string destFields[], int& count) {
        count = 0;
        string field;
        bool inQuotes = false;
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (c == '"') {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) {
                if (count < 10) destFields[count++] = manualTrim(field); // Used Manual Trim
                field.clear();
            }
            else {
                field += c;
            }
        }
        if (count < 10) destFields[count++] = manualTrim(field); // Used Manual Trim
    }

public:
    EducationSector() : allSchoolsHead(nullptr), graph(nullptr) {}
    EducationSector(Graph* masterGraph) : allSchoolsHead(nullptr), graph(masterGraph) {}

    // Rebuilds heap (used after rating updates)
    void rebuildRankingHeap() {
        rankingHeap.size = 0;
        School* temp = allSchoolsHead;
        while (temp) {
            rankingHeap.insert(temp);
            temp = temp->masterNext;
        }
    }

    void updateSchoolRating() {
        string id;
        double newRating;

        cout << "\n=== Update School Rating ===\n";
        cout << "Enter School ID to update rating: ";
        cin >> id;

        if (!isValidSchoolID(id)) {
            cout << "Error: Invalid school ID format.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        School* s = hashTable.find(id);
        if (!s) {
            cout << "Error: School not found.\n";
            return;
        }

        cout << "Current rating of " << s->name << " is: " << s->rating << "\n";
        cout << "Enter new rating (0 to 5): ";

        if (!(cin >> newRating)) {
            cout << "Error: Invalid rating value.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (!isValidRating(newRating)) {
            cout << "Error: Rating must be between 0 and 5.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        s->rating = newRating;
        rebuildRankingHeap();
        cout << "Rating updated and rankings refreshed successfully.\n";
    }

    void loadFromCSV(const string& filename) {
        if (filename.empty()) {
            cout << "Error: Filename cannot be empty.\n";
            return;
        }

        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Warning: " << filename << " not found. Starting empty.\n";
            return;
        }

        string line;
        // Check Header
        if (!getline(file, line)) {
            cout << "CSV file is empty.\n";
            file.close();
            return;
        }

        bool hasHeader = false;
        string low = manualToLower(line); // Used Manual Lower
        // Manual search for keywords
        if (manualContains(low, "id") || manualContains(low, "school") || manualContains(low, "name")) {
            hasHeader = true;
        }

        if (!hasHeader) {
            // Re-open and process first line as data
            file.close();
            file.open(filename.c_str());
            if (!file.is_open()) {
                cout << "Error: Could not reopen file.\n";
                return;
            }
        }

        int loadedCount = 0;
        int errorCount = 0;

        while (getline(file, line)) {
            if (line.empty()) continue;
            string fields[10];
            int cnt = 0;
            parseCSVLine(line, fields, cnt);

            if (cnt < 4) {
                errorCount++;
                continue;
            }

            // Validate required fields
            if (!isValidSchoolID(fields[0]) || !isValidName(fields[1]) ||
                !isValidSector(fields[2])) {
                errorCount++;
                continue;
            }

            School* s = new School();
            s->schoolID = fields[0];
            s->name = fields[1];
            s->sector = fields[2];

            // Safe double conversion
            double r = 0.0;
            if (cnt > 3 && !fields[3].empty()) {
                try {
                    r = stod(fields[3]);
                    if (!isValidRating(r)) {
                        r = 0.0; // Default if invalid
                    }
                }
                catch (...) {
                    r = 0.0;
                }
            }
            s->rating = r;

            s->subjCount = 0;
            if (cnt > 4) {
                string subsField = fields[4];
                string curSub;
                for (size_t i = 0; i <= subsField.size(); ++i) {
                    if (i == subsField.size() || subsField[i] == ',') {
                        string t = manualTrim(curSub);
                        if (!t.empty() && isValidSubject(t) && s->subjCount < 10) {
                            s->subjects[s->subjCount++] = t;
                        }
                        curSub.clear();
                    }
                    else curSub += subsField[i];
                }
            }

            s->setCoordinatesFromSector();

            // Linkages
            s->masterNext = allSchoolsHead;
            allSchoolsHead = s;
            hashTable.insert(s);
            rankingHeap.insert(s);

            // Indexing subjects
            for (int i = 0; i < s->subjCount; ++i) {
                subjectIndex.insertSchoolForSubject(s->subjects[i], s);
            }

            loadedCount++;
        }

        cout << "Data successfully loaded from " << filename << ".\n";
        cout << "  Loaded: " << loadedCount << " schools\n";
        if (errorCount > 0) {
            cout << "  Skipped: " << errorCount << " invalid records\n";
        }

        file.close();
    }

    void addDepartment() {
        string id, dName;
        cout << "\n=== [Admin] Add Department ===\n";
        cout << "School ID: ";
        cin >> id;

        if (!isValidSchoolID(id)) {
            cout << "Error: Invalid school ID format.\n";
            return;
        }

        School* s = hashTable.find(id);
        if (!s) {
            cout << "Error: School not found.\n";
            return;
        }

        cout << "Department Name: ";
        cin.ignore();
        getline(cin, dName);

        if (!isValidDepartmentName(dName)) {
            cout << "Error: Invalid department name.\n";
            return;
        }

        // Check if department already exists
        DepartmentNode* existing = s->departments;
        while (existing) {
            if (existing->deptName == dName) {
                cout << "Error: Department '" << dName << "' already exists in this school.\n";
                return;
            }
            existing = existing->next;
        }

        DepartmentNode* newDept = new DepartmentNode(dName);
        newDept->next = s->departments;
        s->departments = newDept;
        cout << "Department registered successfully.\n";
    }

    void registerFacultyAndClass() {
        string id, dName, cName, tName;
        cout << "\n=== [Admin] Faculty & Class Registration ===\n";
        cout << "School ID: ";
        cin >> id;

        if (!isValidSchoolID(id)) {
            cout << "Error: Invalid school ID format.\n";
            return;
        }

        School* s = hashTable.find(id);
        if (!s) {
            cout << "Error: School not found.\n";
            return;
        }

        cout << "Department Name: ";
        cin.ignore();
        getline(cin, dName);

        if (!isValidDepartmentName(dName)) {
            cout << "Error: Invalid department name.\n";
            return;
        }

        DepartmentNode* d = s->departments;
        while (d) {
            if (d->deptName == dName) break;
            d = d->next;
        }
        if (!d) {
            cout << "Error: Department not found.\n";
            return;
        }

        cout << "Class Name: ";
        getline(cin, cName);

        if (!isValidClassName(cName)) {
            cout << "Error: Invalid class name.\n";
            return;
        }

        // Check if class already exists in this department
        ClassNode* existingClass = d->classes;
        while (existingClass) {
            if (existingClass->className == cName) {
                cout << "Error: Class '" << cName << "' already exists in this department.\n";
                return;
            }
            existingClass = existingClass->next;
        }

        cout << "Assign Faculty (Teacher Name): ";
        getline(cin, tName);

        if (!isValidName(tName)) {
            cout << "Error: Invalid teacher name.\n";
            return;
        }

        ClassNode* newClass = new ClassNode(cName, tName);
        newClass->next = d->classes;
        d->classes = newClass;
        cout << "Class added and Faculty assigned successfully.\n";
    }

    void registerStudent() {
        string id, dName, cName, sName;
        int sID;

        cout << "\n=== [Admin] Student Registration ===\n";
        cout << "School ID: ";
        cin >> id;

        if (!isValidSchoolID(id)) {
            cout << "Error: Invalid school ID format.\n";
            return;
        }

        School* s = hashTable.find(id);
        if (!s) {
            cout << "Error: School not found.\n";
            return;
        }

        cout << "Department Name: ";
        cin.ignore();
        getline(cin, dName);

        if (!isValidDepartmentName(dName)) {
            cout << "Error: Invalid department name.\n";
            return;
        }

        DepartmentNode* d = s->departments;
        while (d) {
            if (d->deptName == dName) break;
            d = d->next;
        }
        if (!d) {
            cout << "Error: Department not found.\n";
            return;
        }

        cout << "Class Name: ";
        getline(cin, cName);

        if (!isValidClassName(cName)) {
            cout << "Error: Invalid class name.\n";
            return;
        }

        ClassNode* c = d->classes;
        while (c) {
            if (c->className == cName) break;
            c = c->next;
        }
        if (!c) {
            cout << "Error: Class not found.\n";
            return;
        }

        cout << "Student ID: ";
        if (!(cin >> sID) || !isValidStudentID(sID)) {
            cout << "Error: Invalid student ID. Must be a positive number up to 999999.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        // Check if student ID already exists in this class
        StudentNode* existingStudent = c->students;
        while (existingStudent) {
            if (existingStudent->id == sID) {
                cout << "Error: Student ID " << sID << " already exists in this class.\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
            existingStudent = existingStudent->next;
        }

        cout << "Student Name: ";
        cin.ignore();
        getline(cin, sName);

        if (!isValidName(sName)) {
            cout << "Error: Invalid student name.\n";
            return;
        }

        StudentNode* st = new StudentNode(sID, sName);
        st->next = c->students;
        c->students = st;
        cout << "Student registered successfully.\n";
    }

    void searchBySubject() {
        string query;
        cout << "\n=== Search Schools by Subject ===\n";
        cout << "Enter Subject to search (e.g., Math): ";
        cin.ignore();
        getline(cin, query);

        query = manualTrim(query);
        if (query.empty()) {
            cout << "Error: Search query cannot be empty.\n";
            return;
        }

        if (!isValidSubject(query)) {
            cout << "Warning: Subject name may contain invalid characters.\n";
        }

        bool foundAny = false;
        cout << "\nSearching for subjects similar to: '" << query << "'\n";

        // 1. Exact/Hash Match
        SubjectNode* exact = subjectIndex.findSubject(query);
        if (exact) {
            cout << "\n=== Exact matches ===\n";
            SchoolListNode* cur = exact->schools;
            while (cur) {
                cout << "- " << cur->school->name << " (Subject: exact match)\n";
                cur = cur->next;
            }
            foundAny = true;
        }

        // 2. Fuzzy Match (Manual Loop)
        cout << "\n=== Fuzzy / Partial Matches ===\n";
        School* temp = allSchoolsHead;
        while (temp) {
            for (int i = 0; i < temp->subjCount; i++) {
                string subject = temp->subjects[i];
                // Use Manual Contains
                if (manualContains(subject, query)) {
                    cout << "- " << temp->name << " (Matched: " << subject << ")\n";
                    foundAny = true;
                }
            }
            temp = temp->masterNext;
        }

        if (!foundAny) {
            cout << "No schools found for fuzzy or exact match.\n";
        }
    }

    void findNearestSchool() {
        double userLat, userLon;
        cout << "\n=== Find Nearest School ===\n";
        cout << "Enter your coordinates (Latitude Longitude): ";

        if (!(cin >> userLat >> userLon)) {
            cout << "Error: Invalid coordinates format.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (!isValidCoordinate(userLat, true) || !isValidCoordinate(userLon, false)) {
            cout << "Error: Invalid coordinates. Latitude must be between -90 and 90, Longitude between -180 and 180.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (!allSchoolsHead) {
            cout << "Error: No schools data available.\n";
            return;
        }

        School* nearest = nullptr;
        double minDist = 1e18; // Large number

        School* temp = allSchoolsHead;
        while (temp) {
            double dx = temp->lat - userLat;
            double dy = temp->lon - userLon;
            double dist = sqrt(dx * dx + dy * dy);
            if (dist < minDist) {
                minDist = dist;
                nearest = temp;
            }
            temp = temp->masterNext;
        }

        if (nearest) {
            cout << "\n=== Nearest School Found ===\n";
            cout << "School: " << nearest->name << "\n";
            cout << "ID: " << nearest->schoolID << "\n";
            cout << "Sector: " << nearest->sector << "\n";
            cout << "Rating: " << nearest->rating << "\n";
            cout << "Coordinates: " << fixed << setprecision(6)
                << nearest->lat << ", " << nearest->lon << "\n";
            cout << "Approximate Distance: " << minDist << " degrees\n";
        }
        else {
            cout << "No schools data available.\n";
        }
    }

    void displayHierarchy() {
        string id;
        cout << "\n=== Display School Organogram ===\n";
        cout << "Enter School ID for Organogram: ";
        cin >> id;

        if (!isValidSchoolID(id)) {
            cout << "Error: Invalid school ID format.\n";
            return;
        }

        School* s = hashTable.find(id);
        if (!s) {
            cout << "Error: School not found.\n";
            return;
        }

        cout << "\n=== ORGANOGRAM: " << s->name << " ===\n";
        cout << "School ID: " << s->schoolID << "\n";
        cout << "Sector: " << s->sector << "\n";
        cout << "Rating: " << s->rating << "\n\n";

        DepartmentNode* d = s->departments;
        if (!d) {
            cout << "No Departments registered for this school.\n";
            return;
        }

        int deptCount = 0;
        while (d) {
            deptCount++;
            cout << "|-- [Department " << deptCount << "] " << d->deptName << "\n";
            ClassNode* c = d->classes;
            if (!c) {
                cout << "    |-- (No Classes in this Department)\n";
            }
            else {
                int classCount = 0;
                while (c) {
                    classCount++;
                    cout << "    |-- [Class " << classCount << "] " << c->className
                        << " (Faculty: " << c->teacher << ")\n";
                    StudentNode* st = c->students;
                    if (!st) {
                        cout << "        |-- (No Students in this Class)\n";
                    }
                    else {
                        int studentCount = 0;
                        while (st) {
                            studentCount++;
                            cout << "        |-- [Student " << studentCount << "] "
                                << st->name << " (ID: " << st->id << ")\n";
                            st = st->next;
                        }
                    }
                    c = c->next;
                }
            }
            d = d->next;
        }
    }

    // ==========================================
// COMPREHENSIVE DISPLAY FUNCTION
// ==========================================
    void displayEverything() {
        cout << "\n" << string(60, '=') << "\n";
        cout << "          EDUCATION SECTOR - COMPLETE OVERVIEW\n";
        cout << string(60, '=') << "\n\n";

        // Check if any data exists
        if (!allSchoolsHead) {
            cout << "No schools data available in the system.\n";
            cout << "Please load data from CSV file or add schools manually.\n";
            return;
        }

        // ==========================================
        // 1. GENERAL STATISTICS
        // ==========================================
        cout << "1. GENERAL STATISTICS\n";
        cout << string(40, '-') << "\n";

        int totalSchools = 0;
        int totalDepartments = 0;
        int totalClasses = 0;
        int totalStudents = 0;
        int totalSubjects = 0;

        School* schoolTemp = allSchoolsHead;
        while (schoolTemp) {
            totalSchools++;

            // Count subjects for this school
            totalSubjects += schoolTemp->subjCount;

            // Count departments, classes, and students
            DepartmentNode* deptTemp = schoolTemp->departments;
            while (deptTemp) {
                totalDepartments++;

                ClassNode* classTemp = deptTemp->classes;
                while (classTemp) {
                    totalClasses++;

                    StudentNode* studentTemp = classTemp->students;
                    while (studentTemp) {
                        totalStudents++;
                        studentTemp = studentTemp->next;
                    }
                    classTemp = classTemp->next;
                }
                deptTemp = deptTemp->next;
            }
            schoolTemp = schoolTemp->masterNext;
        }

        cout << "   Total Schools: " << totalSchools << "\n";
        cout << "   Total Departments: " << totalDepartments << "\n";
        cout << "   Total Classes: " << totalClasses << "\n";
        cout << "   Total Students: " << totalStudents << "\n";
        cout << "   Total Subjects Offered: " << totalSubjects << "\n\n";

        // ==========================================
        // 2. ALL SCHOOLS DETAILED LIST
        // ==========================================
        cout << "2. ALL SCHOOLS IN THE SYSTEM\n";
        cout << string(40, '-') << "\n";

        schoolTemp = allSchoolsHead;
        int schoolNumber = 1;
        while (schoolTemp) {
            cout << "\n   " << schoolNumber << ". SCHOOL: " << schoolTemp->name << "\n";
            cout << "      ID: " << schoolTemp->schoolID << "\n";
            cout << "      Sector: " << schoolTemp->sector << "\n";
            cout << "      Rating: " << fixed << setprecision(1) << schoolTemp->rating << "/5.0\n";
            cout << "      Coordinates: (" << fixed << setprecision(6)
                << schoolTemp->lat << ", " << schoolTemp->lon << ")\n";

            // display subjects
            if (schoolTemp->subjCount > 0) {
                cout << "      Subjects Offered (" << schoolTemp->subjCount << "): ";
                for (int i = 0; i < schoolTemp->subjCount; i++) {
                    cout << schoolTemp->subjects[i];
                    if (i < schoolTemp->subjCount - 1) cout << ", ";
                }
                cout << "\n";
            }
            else {
                cout << "      Subjects Offered: None\n";
            }

            // sisplay departments hierarchy ->
            DepartmentNode* deptTemp = schoolTemp->departments;
            if (deptTemp) {
                cout << "      Departments:\n";
                int deptCount = 1;
                while (deptTemp) {
                    cout << "        " << deptCount << ". " << deptTemp->deptName << "\n";

                    // display classes in this department
                    ClassNode* classTemp = deptTemp->classes;
                    if (classTemp) {
                        int classCount = 1;
                        while (classTemp) {
                            cout << "           " << classCount << ". Class: " << classTemp->className
                                << " (Teacher: " << classTemp->teacher << ")\n";

                            // display students in this class
                            StudentNode* studentTemp = classTemp->students;
                            if (studentTemp) {
                                int studentCount = 1;
                                cout << "              Students: ";
                                while (studentTemp) {
                                    cout << studentTemp->name << " (ID: " << studentTemp->id << ")";
                                    if (studentTemp->next) cout << ", ";
                                    studentCount++;
                                    studentTemp = studentTemp->next;
                                }
                                cout << "\n";
                            }
                            else {
                                cout << "              Students: None\n";
                            }
                            classTemp = classTemp->next;
                            classCount++;
                        }
                    }
                    else {
                        cout << "           No classes in this department\n";
                    }
                    deptTemp = deptTemp->next;
                    deptCount++;
                }
            }
            else {
                cout << "      Departments: None registered\n";
            }

            cout << string(40, '~') << "\n";
            schoolTemp = schoolTemp->masterNext;
            schoolNumber++;
        }

        // ==========================================
        // 3. SCHOOL RANKINGS (TOP 10)
        // ==========================================
        cout << "\n3. SCHOOL RANKINGS (Top 10)\n";
        cout << string(40, '-') << "\n";
        rankingHeap.displayTop(10);

        // ==========================================
        // 4. SUBJECT INDEX SUMMARY
        // ==========================================
        cout << "\n4. SUBJECT INDEX SUMMARY\n";
        cout << string(40, '-') << "\n";

        // We need to traverse the subject hash table
        // Since we can't directly access the private table, we'll use an alternative approach
        // Collect all unique subjects from schools
        cout << "   Schools grouped by popular subjects:\n\n";

        // Create an array to store subjects and their counts
        const int MAX_SUBJECTS = 100;
        string uniqueSubjects[MAX_SUBJECTS];
        int subjectSchoolCount[MAX_SUBJECTS] = { 0 };
        int uniqueSubjectCount = 0;

        schoolTemp = allSchoolsHead;
        while (schoolTemp) {
            for (int i = 0; i < schoolTemp->subjCount; i++) {
                string subject = schoolTemp->subjects[i];

                // Check if subject already exists in our array
                bool found = false;
                for (int j = 0; j < uniqueSubjectCount; j++) {
                    if (manualToLower(manualTrim(uniqueSubjects[j])) == manualToLower(manualTrim(subject))) {
                        subjectSchoolCount[j]++;
                        found = true;
                        break;
                    }
                }

                // If not found, add it
                if (!found && uniqueSubjectCount < MAX_SUBJECTS) {
                    uniqueSubjects[uniqueSubjectCount] = subject;
                    subjectSchoolCount[uniqueSubjectCount] = 1;
                    uniqueSubjectCount++;
                }
            }
            schoolTemp = schoolTemp->masterNext;
        }

        // Display subjects and school counts
        for (int i = 0; i < uniqueSubjectCount; i++) {
            cout << "   - " << setw(20) << left << uniqueSubjects[i]
                << ": Offered by " << subjectSchoolCount[i] << " school(s)\n";
        }

        if (uniqueSubjectCount == 0) {
            cout << "   No subjects indexed yet.\n";
        }

        // ==========================================
        // 5. SECTOR DISTRIBUTION
        // ==========================================
        cout << "\n5. SCHOOLS BY SECTOR\n";
        cout << string(40, '-') << "\n";

        // Count schools per sector
        const int MAX_SECTORS = 20;
        string sectors[MAX_SECTORS];
        int sectorCounts[MAX_SECTORS] = { 0 };
        int sectorIndex = 0;

        schoolTemp = allSchoolsHead;
        while (schoolTemp) {
            string sector = schoolTemp->sector;

            // Check if sector already exists
            bool found = false;
            for (int i = 0; i < sectorIndex; i++) {
                if (sectors[i] == sector) {
                    sectorCounts[i]++;
                    found = true;
                    break;
                }
            }

            // If not found, add it
            if (!found && sectorIndex < MAX_SECTORS) {
                sectors[sectorIndex] = sector;
                sectorCounts[sectorIndex] = 1;
                sectorIndex++;
            }

            schoolTemp = schoolTemp->masterNext;
        }

        // Display sector distribution
        if (sectorIndex > 0) {
            for (int i = 0; i < sectorIndex; i++) {
                cout << "   - Sector " << setw(10) << left << sectors[i]
                    << ": " << sectorCounts[i] << " school(s)\n";
            }
        }
        else {
            cout << "   No sector information available.\n";
        }

        // ==========================================
        // 6. RATING DISTRIBUTION
        // ==========================================
        cout << "\n6. RATING DISTRIBUTION\n";
        cout << string(40, '-') << "\n";

        int ratingRanges[6] = { 0 }; // 0-1, 1-2, 2-3, 3-4, 4-5, exactly 5
        schoolTemp = allSchoolsHead;
        while (schoolTemp) {
            double rating = schoolTemp->rating;

            if (rating >= 0 && rating < 1) ratingRanges[0]++;
            else if (rating >= 1 && rating < 2) ratingRanges[1]++;
            else if (rating >= 2 && rating < 3) ratingRanges[2]++;
            else if (rating >= 3 && rating < 4) ratingRanges[3]++;
            else if (rating >= 4 && rating < 5) ratingRanges[4]++;
            else if (rating == 5) ratingRanges[5]++;

            schoolTemp = schoolTemp->masterNext;
        }

        cout << "   Rating Range    | Number of Schools\n";
        cout << "   " << string(35, '-') << "\n";
        cout << "   0.0 - 1.0       | " << ratingRanges[0] << "\n";
        cout << "   1.0 - 2.0       | " << ratingRanges[1] << "\n";
        cout << "   2.0 - 3.0       | " << ratingRanges[2] << "\n";
        cout << "   3.0 - 4.0       | " << ratingRanges[3] << "\n";
        cout << "   4.0 - 5.0       | " << ratingRanges[4] << "\n";
        cout << "   5.0 (Excellent) | " << ratingRanges[5] << "\n";

        // ==========================================
        // 7. TOP 3 SCHOOLS BY STUDENT COUNT
        // ==========================================
        cout << "\n7. TOP 3 SCHOOLS BY STUDENT POPULATION\n";
        cout << string(40, '-') << "\n";

        // Create array to store school-student count pairs
        struct SchoolStudentCount {
            School* school;
            int studentCount;
        };

        SchoolStudentCount schoolStudentCounts[500]; // Assuming max 500 schools
        int schoolCount = 0;

        schoolTemp = allSchoolsHead;
        while (schoolTemp && schoolCount < 500) {
            int studentCount = 0;

            // Count all students in this school
            DepartmentNode* deptTemp = schoolTemp->departments;
            while (deptTemp) {
                ClassNode* classTemp = deptTemp->classes;
                while (classTemp) {
                    StudentNode* studentTemp = classTemp->students;
                    while (studentTemp) {
                        studentCount++;
                        studentTemp = studentTemp->next;
                    }
                    classTemp = classTemp->next;
                }
                deptTemp = deptTemp->next;
            }

            schoolStudentCounts[schoolCount].school = schoolTemp;
            schoolStudentCounts[schoolCount].studentCount = studentCount;
            schoolCount++;

            schoolTemp = schoolTemp->masterNext;
        }

        // Simple bubble sort to get top 3
        for (int i = 0; i < min(3, schoolCount); i++) {
            for (int j = i + 1; j < schoolCount; j++) {
                if (schoolStudentCounts[j].studentCount > schoolStudentCounts[i].studentCount) {
                    SchoolStudentCount temp = schoolStudentCounts[i];
                    schoolStudentCounts[i] = schoolStudentCounts[j];
                    schoolStudentCounts[j] = temp;
                }
            }
        }

        // Display top 3
        if (schoolCount > 0) {
            for (int i = 0; i < min(3, schoolCount); i++) {
                cout << "   " << (i + 1) << ". " << schoolStudentCounts[i].school->name
                    << " (ID: " << schoolStudentCounts[i].school->schoolID << ")\n";
                cout << "      Students: " << schoolStudentCounts[i].studentCount
                    << " | Rating: " << fixed << setprecision(1) << schoolStudentCounts[i].school->rating << "/5.0\n";
            }
        }
        else {
            cout << "   No student data available.\n";
        }

        // ==========================================
        // 8. SYSTEM INFORMATION
        // ==========================================
        cout << "\n8. SYSTEM INFORMATION\n";
        cout << string(40, '-') << "\n";
        cout << "   Hash Table Size (School ID): 211 buckets\n";
        cout << "   Hash Table Size (Subject): 137 buckets\n";
        cout << "   Max Heap Capacity: 500 schools\n";
        cout << "   Max Subjects per School: 10\n";
        cout << "   Max Departments/Classes/Students: Unlimited (dynamic)\n";
        cout << "   Data Structure: N-ary Tree Hierarchy\n";

        cout << "\n" << string(60, '=') << "\n";
        cout << "                 END OF REPORT\n";
        cout << string(60, '=') << "\n\n";
    }

    // ==========================================
    // SIMPLIFIED DISPLAY FUNCTION (Alternative)
    // ==========================================
    void displaySummary() {
        cout << "\n" << string(50, '=') << "\n";
        cout << "     EDUCATION SECTOR - QUICK SUMMARY\n";
        cout << string(50, '=') << "\n\n";

        if (!allSchoolsHead) {
            cout << "No schools data available.\n";
            return;
        }

        // Count totals
        int schoolCount = 0;
        int deptCount = 0;
        int classCount = 0;
        int studentCount = 0;

        School* temp = allSchoolsHead;
        while (temp) {
            schoolCount++;

            DepartmentNode* dept = temp->departments;
            while (dept) {
                deptCount++;

                ClassNode* cls = dept->classes;
                while (cls) {
                    classCount++;

                    StudentNode* stu = cls->students;
                    while (stu) {
                        studentCount++;
                        stu = stu->next;
                    }
                    cls = cls->next;
                }
                dept = dept->next;
            }
            temp = temp->masterNext;
        }

        cout << "Total Schools:      " << schoolCount << "\n";
        cout << "Total Departments:  " << deptCount << "\n";
        cout << "Total Classes:      " << classCount << "\n";
        cout << "Total Students:     " << studentCount << "\n\n";

        // List all schools
        cout << "LIST OF ALL SCHOOLS:\n";
        cout << string(40, '-') << "\n";

        temp = allSchoolsHead;
        int i = 1;
        while (temp) {
            cout << i << ". " << temp->name
                << " (ID: " << temp->schoolID
                << ", Sector: " << temp->sector
                << ", Rating: " << fixed << setprecision(1) << temp->rating << ")\n";
            temp = temp->masterNext;
            i++;
        }

        cout << "\n" << string(50, '=') << "\n";
    }

    // ==========================================
    // DISPLAY SCHOOL BY ID FUNCTION
    // ==========================================
    void displaySchoolDetails(const string& schoolID) {
        if (!isValidSchoolID(schoolID)) {
            cout << "Error: Invalid school ID format.\n";
            return;
        }

        School* school = hashTable.find(schoolID);
        if (!school) {
            cout << "Error: School with ID '" << schoolID << "' not found.\n";
            return;
        }

        cout << "\n" << string(60, '=') << "\n";
        cout << "          SCHOOL DETAILS: " << school->name << "\n";
        cout << string(60, '=') << "\n\n";

        cout << "BASIC INFORMATION:\n";
        cout << string(40, '-') << "\n";
        cout << "School ID:      " << school->schoolID << "\n";
        cout << "Name:           " << school->name << "\n";
        cout << "Sector:         " << school->sector << "\n";
        cout << "Rating:         " << fixed << setprecision(1) << school->rating << "/5.0\n";
        cout << "Coordinates:    " << fixed << setprecision(6)
            << school->lat << ", " << school->lon << "\n\n";

        cout << "SUBJECTS OFFERED:\n";
        cout << string(40, '-') << "\n";
        if (school->subjCount > 0) {
            for (int i = 0; i < school->subjCount; i++) {
                cout << "  - " << school->subjects[i] << "\n";
            }
        }
        else {
            cout << "  No subjects registered.\n";
        }
        cout << "\n";

        cout << "DEPARTMENT STRUCTURE:\n";
        cout << string(40, '-') << "\n";

        DepartmentNode* dept = school->departments;
        if (!dept) {
            cout << "  No departments registered.\n";
        }
        else {
            int deptNum = 1;
            while (dept) {
                cout << deptNum << ". DEPARTMENT: " << dept->deptName << "\n";

                ClassNode* cls = dept->classes;
                if (!cls) {
                    cout << "   No classes in this department.\n";
                }
                else {
                    int classNum = 1;
                    while (cls) {
                        cout << "   " << classNum << ". CLASS: " << cls->className
                            << " (Teacher: " << cls->teacher << ")\n";

                        StudentNode* stu = cls->students;
                        if (!stu) {
                            cout << "      No students in this class.\n";
                        }
                        else {
                            cout << "      Students (" << countStudentsInClass(cls) << "):\n";
                            int stuNum = 1;
                            while (stu) {
                                cout << "      " << stuNum << ". " << stu->name
                                    << " (ID: " << stu->id << ")\n";
                                stu = stu->next;
                                stuNum++;
                            }
                        }
                        cls = cls->next;
                        classNum++;
                    }
                }
                cout << "\n";
                dept = dept->next;
                deptNum++;
            }
        }

        // Statistics for this school
        cout << "SCHOOL STATISTICS:\n";
        cout << string(40, '-') << "\n";

        int deptCount = 0, classCount = 0, studentCount = 0;
        dept = school->departments;
        while (dept) {
            deptCount++;
            ClassNode* cls = dept->classes;
            while (cls) {
                classCount++;
                StudentNode* stu = cls->students;
                while (stu) {
                    studentCount++;
                    stu = stu->next;
                }
                cls = cls->next;
            }
            dept = dept->next;
        }

        cout << "Departments:  " << deptCount << "\n";
        cout << "Classes:      " << classCount << "\n";
        cout << "Students:     " << studentCount << "\n";
        cout << "Subjects:     " << school->subjCount << "\n";

        cout << "\n" << string(60, '=') << "\n";
    }

    // Helper function to count students in a class
    int countStudentsInClass(ClassNode* cls) {
        if (!cls) return 0;
        int count = 0;
        StudentNode* stu = cls->students;
        while (stu) {
            count++;
            stu = stu->next;
        }
        return count;
    }

    void showRankings() {
        if (!allSchoolsHead) {
            cout << "\nError: No schools data available. Please load data first.\n";
            return;
        }
        rankingHeap.displayTop(5);
    }
};