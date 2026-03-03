#pragma once
#include <iostream>
#include <cmath>
#include <limits>
#include <cctype>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"
using namespace std;

const int HASH_SIZE = 101;

// ==================== SAFE STRING COPY FUNCTION ====================
char* copyString(const char* src) {
    if (!src) return nullptr;
    int len = 0;
    while (src[len]) len++;
    char* dest = new char[len + 1];
    for (int i = 0; i <= len; i++) dest[i] = src[i];
    return dest;
}

int stringLength(const char* str) {
    if (!str) return 0;
    int len = 0;
    while (str[len]) len++;
    return len;
}


// ==================== PRODUCT ====================
struct Product {
    char* name;
    char* category;
    double price;
    Product* next;

    Product(const char* n = "", const char* c = "", double p = 0.0) : next(nullptr) {
        name = copyString(n);
        category = copyString(c); // use copying to name them
        price = p;
    }

    ~Product() {
        delete[] name;
        delete[] category;
    }
};

// ==================== HASH TABLE ====================
class ProductHashTable {
private:
    struct Bucket {
        Product* head;
        Bucket() : head(nullptr) {} // bucket struct have head ptr
    } table[HASH_SIZE];

    unsigned long hashFunction(const char* str) {
        if (!str || !str[0]) return 0;
        unsigned long h = 5381;
        int c;
        while ((c = *str++)) h = ((h << 5) + h) + c;
        return h % HASH_SIZE;
    }

public:
    ~ProductHashTable() {
        for (int i = 0; i < HASH_SIZE; i++) {
            Product* cur = table[i].head;
            while (cur) {
                Product* temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
    }

    void insert(const char* name, const char* category, double price) {
        if (!isValidString(name) || !isValidString(category) || !isValidPrice(price)) {
            cout << " Error: Invalid product data!\n";
            return;
        }

        unsigned long idx = hashFunction(name);
        Product* newProd = new Product(name, category, price);
        newProd->next = table[idx].head;
        table[idx].head = newProd;
    }

    Product* search(const char* name) {
        if (!isValidString(name)) return nullptr;

        unsigned long idx = hashFunction(name);
        Product* cur = table[idx].head;
        while (cur) {
            if (stringCompare(cur->name, name)) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Product* getBucketHead(int idx) const {
        if (idx < 0 || idx >= HASH_SIZE) return nullptr;
        return table[idx].head;
    }
};

// ==================== SHOP ====================
class Shop {
public:
    char* name;
    ProductHashTable products;
    Shop* next;

    Shop(const char* n) : next(nullptr) {
        if (isValidString(n)) {
            name = copyString(n);
        }
        else {
            name = copyString("Unknown Shop");
            cout << " Warning: Invalid shop name provided, using default.\n";
        }
    }

    ~Shop() { delete[] name; }
};

// ==================== FLOOR ====================
class Floor {
public:
    char* name;
    Shop* firstShop;
    Floor* next;

    Floor(const char* n) : firstShop(nullptr), next(nullptr) {
        if (isValidString(n)) {
            name = copyString(n);
        }
        else {
            name = copyString("Unknown Floor");
            cout << "? Warning: Invalid floor name provided, using default.\n";
        }
    }

    ~Floor() {
        delete[] name;
        Shop* s = firstShop;
        while (s) {
            Shop* t = s;
            s = s->next;
            delete t;
        }
    }

    void addShop(Shop* shop) {
        if (!shop) {
            cout << "? Error: Cannot add null shop!\n";
            return;
        }
        shop->next = firstShop;
        firstShop = shop;
    }
};

// ==================== MALL ====================
class Mall {
public:
    char* id;
    char* name;
    int locationNodeId;
    Floor* firstFloor;
    Mall* next;

    Mall(const char* i, const char* n, int loc)
        : locationNodeId(loc), firstFloor(nullptr), next(nullptr) {
        id = copyString(i);
        name = copyString(n);
    }

    ~Mall() {
        delete[] id;
        delete[] name;
        Floor* f = firstFloor;
        while (f) {
            Floor* t = f;
            f = f->next;
            delete t;
        }
    }

    void addFloor(Floor* floor) {
        if (!floor) {
            cout << "? Error: Cannot add null floor!\n";
            return;
        }
        floor->next = firstFloor;
        firstFloor = floor;
    }
};

// ==================== COMMERCIAL SECTOR ====================
class CommercialSector {
private:
    Mall* firstMall;
    Graph* cityGraph;

    Mall* findMallById(const char* id) {
        if (!isValidString(id)) return nullptr;

        Mall* m = firstMall;
        while (m) {
            if (stringCompare(m->id, id)) return m;
            m = m->next;
        }
        return nullptr;
    }

    Floor* findFloorInMall(Mall* mall, const char* name) {
        if (!mall || !isValidString(name)) return nullptr;

        Floor* f = mall->firstFloor;
        while (f) {
            if (stringCompare(f->name, name)) return f;
            f = f->next;
        }
        return nullptr;
    }

    Shop* findShopInFloor(Floor* floor, const char* name) {
        if (!floor || !isValidString(name)) return nullptr;

        Shop* s = floor->firstShop;
        while (s) {
            if (stringCompare(s->name, name)) return s;
            s = s->next;
        }
        return nullptr;
    }

    // Helper function to validate if a node exists in the graph
    bool validateNodeExists(int nodeId) {
        if (!cityGraph) return false;

        // check if node is within valid range
        int nodeCount = cityGraph->getNodeCount();
        if (nodeId < 0 || nodeId >= nodeCount) {
            return false;
        }

        // check if node has valid coordinates (not 0,0)
        Coordinate coord = cityGraph->getCoord(nodeId);
        return !(coord.lat == 0 && coord.lon == 0);
    }

public:
    CommercialSector(Graph* g) : firstMall(nullptr), cityGraph(g) {
        if (!g) {
            cout << "? Warning: CommercialSector initialized with null graph!\n";
        }
    }

    ~CommercialSector() {
        Mall* m = firstMall;
        while (m) {
            Mall* t = m;
            m = m->next;
            delete t;
        }
    }

    // Updated registerMall function that automatically creates graph nodes
    void registerMall(const char* id, const char* name, int loc) {
        if (!isValidString(id, 50)) {
            cout << " Error: Invalid mall ID!\n";
            return;
        }

        if (!isValidString(name, 100)) {
            cout << " Error: Invalid mall name!\n";
            return;
        }

        // Check if mall with same ID already exists
        if (findMallById(id)) {
            cout << " Error: Mall with ID '" << id << "' already exists!\n";
            return;
        }

        // Check if we need to create this node in the graph
        if (!validateNodeExists(loc)) {
            // Node doesn't exist, so create it with default coordinates
            double lat = 33.6 + (rand() % 1000) / 1000.0;
            double lon = 73.0 + (rand() % 1000) / 1000.0;
            cout << "Note: Creating new graph node for mall location..." << endl;
            loc = cityGraph->addLocation(name, lat, lon, MALL);
        }

        Mall* newMall = new Mall(id, name, loc);
        newMall->next = firstMall;
        firstMall = newMall;
        cout << "? Mall registered: " << name << " (Node ID: " << loc << ")" << endl;
    }

    // Alternative: register mall with coordinates
    void registerMallWithCoords(const char* id, const char* name, double lat, double lon) {
        if (!isValidString(id, 50)) {
            cout << "? Error: Invalid mall ID!\n";
            return;
        }

        if (!isValidString(name, 100)) {
            cout << "? Error: Invalid mall name!\n";
            return;
        }

        if (!isValidCoordinate(lat, lon)) {
            cout << "? Error: Invalid coordinates!\n";
            return;
        }

        // Check if mall with same ID already exists
        if (findMallById(id)) {
            cout << "? Error: Mall with ID '" << id << "' already exists!\n";
            return;
        }

        // Add location to graph first
        int nodeId = cityGraph->addLocation(name, lat, lon, MALL);

        Mall* newMall = new Mall(id, name, nodeId);
        newMall->next = firstMall;
        firstMall = newMall;
        cout << "? Mall registered: " << name << " (Node ID: " << nodeId << ")" << endl;
    }

    void addFloor(const char* mallId, const char* floorName) {
        if (!isValidString(mallId) || !isValidString(floorName)) {
            cout << "? Error: Invalid input!\n";
            return;
        }

        Mall* mall = findMallById(mallId);
        if (!mall) {
            cout << "? Error: Mall '" << mallId << "' not found!\n";
            return;
        }

        // Check if floor with same name already exists in this mall
        if (findFloorInMall(mall, floorName)) {
            cout << "? Error: Floor '" << floorName << "' already exists in mall '" << mallId << "'!\n";
            return;
        }

        mall->addFloor(new Floor(floorName));
        cout << "? Floor added: " << floorName << endl;
    }

    void addShop(const char* mallId, const char* floorName, const char* shopName) {
        if (!isValidString(mallId) || !isValidString(floorName) || !isValidString(shopName)) {
            cout << "? Error: Invalid input!\n";
            return;
        }

        Mall* mall = findMallById(mallId);
        if (!mall) {
            cout << "? Error: Mall '" << mallId << "' not found!\n";
            return;
        }

        Floor* floor = findFloorInMall(mall, floorName);
        if (!floor) {
            cout << "? Error: Floor '" << floorName << "' not found in mall '" << mallId << "'!\n";
            return;
        }

        // Check if shop with same name already exists on this floor
        if (findShopInFloor(floor, shopName)) {
            cout << "? Error: Shop '" << shopName << "' already exists on floor '" << floorName << "'!\n";
            return;
        }

        floor->addShop(new Shop(shopName));
        cout << "? Shop added: " << shopName << endl;
    }

    void addProduct(const char* mallId, const char* floorName, const char* shopName,
        const char* prodName, const char* category, double price) {
        if (!isValidString(mallId) || !isValidString(floorName) || !isValidString(shopName) ||
            !isValidString(prodName) || !isValidString(category) || !isValidPrice(price)) {
            cout << "? Error: Invalid input data!\n";
            return;
        }

        if (!isValidCategory(category)) {
            cout << "? Warning: Category '" << category << "' may not be valid.\n";
        }

        Mall* mall = findMallById(mallId);
        if (!mall) {
            cout << "? Error: Mall '" << mallId << "' not found!\n";
            return;
        }

        Floor* floor = findFloorInMall(mall, floorName);
        if (!floor) {
            cout << "? Error: Floor '" << floorName << "' not found in mall '" << mallId << "'!\n";
            return;
        }

        Shop* shop = findShopInFloor(floor, shopName);
        if (!shop) {
            cout << "? Error: Shop '" << shopName << "' not found on floor '" << floorName << "'!\n";
            return;
        }

        shop->products.insert(prodName, category, price);
        cout << "? Product added: " << prodName << endl;
    }

    void searchByCategory(const char* category) {
        if (!isValidString(category)) {
            cout << "? Error: Invalid category name!\n";
            return;
        }

        cout << "\n=== SEARCH RESULTS FOR CATEGORY: " << category << " ===\n";
        bool found = false;

        Mall* m = firstMall;
        while (m) {
            Floor* f = m->firstFloor;
            while (f) {
                Shop* s = f->firstShop;
                while (s) {
                    for (int i = 0; i < HASH_SIZE; i++) {
                        Product* p = s->products.getBucketHead(i);
                        while (p) {
                            if (stringCompare(p->category, category)) {
                                cout << m->name << " > " << f->name << " > " << s->name
                                    << " -> " << p->name << " (Rs." << p->price << ")\n";
                                found = true;
                            }
                            p = p->next;
                        }
                    }
                    s = s->next;
                }
                f = f->next;
            }
            m = m->next;
        }

        if (!found) cout << "No products found.\n";
    }

    void registerMallInteractive() {
        char id[100], name[100];
        int choice;

        cout << "\n=== Register Mall ===\n";
        cout << "1. Use existing node ID\n";
        cout << "2. Create new location with coordinates\n";
        cout << "Choice: ";

        if (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "? Error: Invalid choice!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore();

        // Get Mall ID
        cout << "Enter Mall ID (max 50 chars): ";
        cin.getline(id, 100);
        if (!isValidString(id, 50)) {
            cout << "? Error: Invalid mall ID!\n";
            return;
        }

        // Check if mall ID already exists
        if (findMallById(id)) {
            cout << "? Error: Mall with ID '" << id << "' already exists!\n";
            return;
        }

        // Get Mall Name
        cout << "Enter Mall Name (max 100 chars): ";
        cin.getline(name, 100);
        if (!isValidString(name, 100)) {
            cout << "? Error: Invalid mall name!\n";
            return;
        }

        if (choice == 1) {
            int loc;
            cout << "Enter Location Node ID: ";
            if (!(cin >> loc)) {
                cout << "? Error: Invalid node ID!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
            cin.ignore();

            if (!isValidNodeId(loc, 1000)) {
                cout << "? Warning: Node ID " << loc << " may be out of range.\n";
            }

            registerMall(id, name, loc);
        }
        else {
            double lat, lon;
            cout << "Enter Latitude (-90 to 90): ";
            if (!(cin >> lat) || lat < -90.0 || lat > 90.0) {
                cout << "? Error: Invalid latitude!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }

            cout << "Enter Longitude (-180 to 180): ";
            if (!(cin >> lon) || lon < -180.0 || lon > 180.0) {
                cout << "? Error: Invalid longitude!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
            cin.ignore();

            registerMallWithCoords(id, name, lat, lon);
        }
    }

    void addFloorInteractive() {
        char mallId[100], floorName[100];

        cout << "\n=== Add Floor ===\n";
        cout << "Enter Mall ID: ";
        cin.getline(mallId, 100);
        if (!isValidString(mallId)) {
            cout << "? Error: Invalid mall ID!\n";
            return;
        }

        cout << "Enter Floor Name: ";
        cin.getline(floorName, 100);
        if (!isValidString(floorName)) {
            cout << "? Error: Invalid floor name!\n";
            return;
        }

        addFloor(mallId, floorName);
    }

    void addShopInteractive() {
        char mallId[100], floorName[100], shopName[100];

        cout << "\n=== Add Shop ===\n";
        cout << "Enter Mall ID: ";
        cin.getline(mallId, 100);
        if (!isValidString(mallId)) {
            cout << "? Error: Invalid mall ID!\n";
            return;
        }

        cout << "Enter Floor Name: ";
        cin.getline(floorName, 100);
        if (!isValidString(floorName)) {
            cout << "? Error: Invalid floor name!\n";
            return;
        }

        cout << "Enter Shop Name: ";
        cin.getline(shopName, 100);
        if (!isValidString(shopName)) {
            cout << "? Error: Invalid shop name!\n";
            return;
        }

        addShop(mallId, floorName, shopName);
    }

    void addProductInteractive() {
        char mallId[100], floorName[100], shopName[100], prodName[100], category[100];
        double price;

        cout << "\n=== Add Product ===\n";
        cout << "Enter Mall ID: ";
        cin.getline(mallId, 100);
        if (!isValidString(mallId)) {
            cout << "? Error: Invalid mall ID!\n";
            return;
        }

        cout << "Enter Floor Name: ";
        cin.getline(floorName, 100);
        if (!isValidString(floorName)) {
            cout << "? Error: Invalid floor name!\n";
            return;
        }

        cout << "Enter Shop Name: ";
        cin.getline(shopName, 100);
        if (!isValidString(shopName)) {
            cout << "? Error: Invalid shop name!\n";
            return;
        }

        cout << "Enter Product Name: ";
        cin.getline(prodName, 100);
        if (!isValidString(prodName)) {
            cout << "? Error: Invalid product name!\n";
            return;
        }

        cout << "Enter Category: ";
        cin.getline(category, 100);
        if (!isValidString(category)) {
            cout << "? Error: Invalid category!\n";
            return;
        }

        cout << "Enter Price (0 - 1000000): ";
        if (!(cin >> price) || !isValidPrice(price)) {
            cout << "? Error: Invalid price!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore();

        addProduct(mallId, floorName, shopName, prodName, category, price);
    }

    void searchByCategoryInteractive() {
        char category[100];

        cout << "\n=== Search Products by Category ===\n";
        cout << "Enter Category to search: ";
        cin.getline(category, 100);

        if (!isValidString(category)) {
            cout << "? Error: Invalid category name!\n";
            return;
        }

        searchByCategory(category);
    }

    void findNearestMallInteractive() {
        int currentLoc;

        cout << "\n=== Find Nearest Mall ===\n";
        cout << "Enter your current location node ID: ";
        if (!(cin >> currentLoc)) {
            cout << "? Error: Invalid node ID!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore();

        if (!isValidNodeId(currentLoc, 1000)) {
            cout << "? Warning: Node ID " << currentLoc << " may be out of range.\n";
        }

        findNearestMall(currentLoc);
    }

    void findNearestMall(int currentLocationNode) {
        if (!cityGraph) {
            cout << "? Error: City graph not initialized!\n";
            return;
        }

        // Check if current location node exists
        if (!validateNodeExists(currentLocationNode)) {
            cout << "? Error: Current location node " << currentLocationNode
                << " does not exist in the graph!\n";
            cout << "   Available nodes: 0 to " << cityGraph->getNodeCount() - 1 << endl;
            return;
        }

        if (!firstMall) {
            cout << "? Error: No malls registered in the system!\n";
            return;
        }

        cout << "\n=== Finding nearest mall from node " << currentLocationNode << " ===\n";

        // Get current location coordinates
        Coordinate currentCoord = cityGraph->getCoord(currentLocationNode);
        cout << "Your location: (" << currentCoord.lat << ", " << currentCoord.lon << ")\n";

        // First try Dijkstra for road distance
        int nodeCount = cityGraph->getNodeCount();
        double* dist = new double[nodeCount];
        int* prev = new int[nodeCount];

        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INF;
            prev[i] = -1;
        }

        cityGraph->dijkstra(currentLocationNode, dist, prev);

        Mall* nearestByRoad = nullptr;
        double bestRoadDist = INF;

        Mall* nearestByAir = nullptr;
        double bestAirDist = INF;

        cout << "\nAvailable malls:\n";
        Mall* m = firstMall;
        while (m) {
            int mallNodeId = m->locationNodeId;

            if (validateNodeExists(mallNodeId)) {
                Coordinate mallCoord = cityGraph->getCoord(mallNodeId);
                double airDist = haversine(currentCoord.lat, currentCoord.lon,
                    mallCoord.lat, mallCoord.lon);
                double roadDist = dist[mallNodeId];

                cout << "  " << m->name << " (Node " << mallNodeId << "):\n";
                cout << "    Air distance: " << airDist << " km\n";
                cout << "    Road distance: " << (roadDist < INF ? to_string(roadDist) + " km" : "Unreachable") << "\n";

                // Track nearest by air
                if (airDist < bestAirDist) {
                    bestAirDist = airDist;
                    nearestByAir = m;
                }

                // Track nearest by road (if reachable)
                if (roadDist < INF && roadDist < bestRoadDist) {
                    bestRoadDist = roadDist;
                    nearestByRoad = m;
                }
            }
            else {
                cout << "  " << m->name << " (Node " << mallNodeId << "): ERROR - Node not in graph!\n";
            }
            m = m->next;
        }

        cout << "\n=== RESULTS ===\n";

        if (nearestByRoad && bestRoadDist < INF) {
            cout << "? Nearest Mall (by road): " << nearestByRoad->name;
            cout << " | Distance: " << bestRoadDist << " km\n";

            // Show path if available
            if (bestRoadDist < INF) {
                int path[MAX_LOC];
                int pathLen;
                cityGraph->getPath(nearestByRoad->locationNodeId, prev, path, pathLen);

                if (pathLen > 1) {
                    cout << "   Path: ";
                    for (int i = 0; i < pathLen; i++) {
                        if (i > 0) cout << " -> ";
                        cout << cityGraph->getName(path[i]);
                    }
                    cout << endl;
                }
            }
        }
        else {
            cout << "? No malls reachable by road from your location.\n";
        }

        if (nearestByAir && bestAirDist < INF) {
            cout << "? Nearest Mall (straight line): " << nearestByAir->name;
            cout << " | Distance: " << bestAirDist << " km\n";
        }

        if (!nearestByRoad && !nearestByAir) {
            cout << "? No valid malls found.\n";
            cout << "   Please ensure:\n";
            cout << "   1. Malls are registered with valid node IDs\n";
            cout << "   2. Nodes exist in the transport graph\n";
            cout << "   3. Nodes are connected with edges\n";
        }

        delete[] dist;
        delete[] prev;
    }

    // Simple alternative using only air distance
    void findNearestMallSimple(int currentLocationNode) {
        if (!cityGraph) {
            cout << "? Error: City graph not initialized!\n";
            return;
        }

        if (!validateNodeExists(currentLocationNode)) {
            cout << "? Error: Current location node does not exist!\n";
            return;
        }

        if (!firstMall) {
            cout << "? Error: No malls registered!\n";
            return;
        }

        Coordinate currentCoord = cityGraph->getCoord(currentLocationNode);
        Mall* nearest = nullptr;
        double bestDist = INF;

        cout << "\n=== Finding nearest mall ===\n";
        cout << "Your location: (" << currentCoord.lat << ", " << currentCoord.lon << ")\n\n";

        Mall* m = firstMall;
        while (m) {
            if (validateNodeExists(m->locationNodeId)) {
                Coordinate mallCoord = cityGraph->getCoord(m->locationNodeId);
                double dist = haversine(currentCoord.lat, currentCoord.lon,
                    mallCoord.lat, mallCoord.lon);

                cout << "  " << m->name << ": " << dist << " km\n";

                if (dist < bestDist) {
                    bestDist = dist;
                    nearest = m;
                }
            }
            m = m->next;
        }

        if (nearest && bestDist < INF) {
            cout << "\n? Nearest Mall: " << nearest->name;
            cout << " | Distance: " << bestDist << " km\n";
        }
        else {
            cout << "\n? No malls found.\n";
        }
    }

    void displayAll() {
        cout << "\n========== COMMERCIAL SECTOR ==========\n";

        if (!firstMall) {
            cout << "No malls registered yet.\n";
            return;
        }

        Mall* m = firstMall;
        while (m) {
            cout << "\nMall: " << m->name << " (ID: " << m->id << ", Node: " << m->locationNodeId << ")\n";
            cout << "-------------------------------------\n";

            Floor* f = m->firstFloor;
            if (!f) {
                cout << "  No floors added.\n";
            }
            else {
                while (f) {
                    cout << "  Floor: " << f->name << "\n";

                    Shop* s = f->firstShop;
                    if (!s) {
                        cout << "    No shops on this floor.\n";
                    }
                    else {
                        while (s) {
                            cout << "    Shop: " << s->name << "\n";

                            bool hasProducts = false;
                            for (int i = 0; i < HASH_SIZE; i++) {
                                Product* p = s->products.getBucketHead(i);
                                while (p) {
                                    if (!hasProducts) {
                                        cout << "      Products:\n";
                                        hasProducts = true;
                                    }
                                    cout << "        - " << p->name
                                        << " | " << p->category
                                        << " | Rs." << p->price << "\n";
                                    p = p->next;
                                }
                            }
                            if (!hasProducts) {
                                cout << "      No products in this shop.\n";
                            }
                            s = s->next;
                        }
                    }
                    f = f->next;
                }
            }

            cout << "-------------------------------------\n";
            m = m->next;
        }
    }

    // New function to display graph connectivity
    void displayGraphInfo() {
        if (!cityGraph) {
            cout << "Graph not initialized.\n";
            return;
        }

        cout << "\n=== GRAPH INFORMATION ===\n";
        cout << "Total nodes in graph: " << cityGraph->getNodeCount() << endl;

        // Show all malls and their node status
        cout << "\nMall locations:\n";
        Mall* m = firstMall;
        while (m) {
            cout << "  " << m->name << ": Node " << m->locationNodeId;
            if (validateNodeExists(m->locationNodeId)) {
                Coordinate coord = cityGraph->getCoord(m->locationNodeId);
                cout << " (Valid - " << coord.lat << ", " << coord.lon << ")";
            }
            else {
                cout << " (INVALID - not in graph)";
            }
            cout << endl;
            m = m->next;
        }
    }
};