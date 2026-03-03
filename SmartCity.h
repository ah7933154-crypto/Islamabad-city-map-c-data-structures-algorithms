#pragma once
#define _USE_MATH_DEFINES 

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include "C:\\Users\\OCS\\OneDrive\\Desktop\\DS FINAL PROJECT\\DS FINAL PROJECT\\validation.h"
using namespace std;

const double INF = 1e18;
const int MAX_LOC = 1000;

enum LocationType {
    STOP,
    SCHOOL,
    HOSPITAL,
    PHARMACY,
    HOUSE,
    MOSQUE,
    PARK,
    WATER_COOLER,
    MALL,
    OTHER
};

struct Coordinate {
    double lat, lon;
};

// ========================= INPUT VALIDATION FUNCTIONS =========================

bool isValidLocationType(int type) {
    return type >= STOP && type <= OTHER;
}




// Helper function to check if string can be converted to double

// ========================= UTILITY FUNCTIONS =========================

inline double haversine(double lat1, double lon1, double lat2, double lon2) {
    // Validate input coordinates
    if (!isValidCoordinate(lat1, true) || !isValidCoordinate(lon1, false) ||
        !isValidCoordinate(lat2, true) || !isValidCoordinate(lon2, false)) {
        return INF; // Return INF for invalid coordinates
    }

    const double PI = 3.14159265358979323846;
    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLon = (lon2 - lon1) * PI / 180.0;
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1) * cos(lat2) *
        sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return 6371.0 * c; // Earth radius in kilometers
}

// ========================= GRAPH CLASS =========================

class Graph {
private:
    struct Edge {
        int to;
        double weight;
        Edge* next;
        Edge(int t, double w) : to(t), weight(w), next(nullptr) {}
    };

    struct Node {
        int id;
        string name;
        Coordinate coord;
        LocationType type;
        Edge* adj;

        // Initialize coord to {0,0} to prevent warnings
        Node() : id(-1), type(OTHER), adj(nullptr), coord{ 0,0 } {}

        ~Node() {
            Edge* e = adj;
            while (e) { Edge* tmp = e; e = e->next; delete tmp; }
        }
    };

    Node nodes[MAX_LOC];
    int nodeCount;

    bool nodeExists(int id) const {
        return id >= 0 && id < nodeCount && nodes[id].id == id;
    }

public:
    Graph() : nodeCount(0) {}
    ~Graph() {}

    // --- Core Functions ---
    int addLocation(const string& name, double lat, double lon, LocationType type);
    int addLocationWithConnections(const string& name, double lat, double lon, LocationType type, const int* connectedIds, int count);

    // Connect function for manual connections
    void connect(int u, int v);

    // --- Algorithms ---
    void dijkstra(int src, double dist[], int prev[]) const;
    void getPath(int dest, const int prev[], int path[], int& pathLen) const;

    // --- Search & Queries ---
    int findNearestNodeOfType(double lat, double lon, LocationType type) const;
    int findNearestNodeOfTypeFromNode(int srcNode, LocationType type) const;

    int findNodeByName(const string& name) const;

    double getDistance(int u, int v) const;
    int* getNodesByType(LocationType type, int& count) const;
    int* getNodesWithinRadius(double lat, double lon, double radius, int& count) const;

    // --- Getters & Display ---
    string getTypeName(LocationType type) const;
    string getName(int id) const { return nodeExists(id) ? nodes[id].name : ""; }
    Coordinate getCoord(int id) const { return nodeExists(id) ? nodes[id].coord : Coordinate{ 0,0 }; }
    LocationType getType(int id) const { return nodeExists(id) ? nodes[id].type : OTHER; }

    int getNodeCount() const { return nodeCount; }

    void displayNode(int id) const;
    void displayNodesByType(LocationType type) const;
    void displayAllNodes() const;

    // --- Validation Helpers ---
    bool validateNodeId(int id) const { return nodeExists(id); }
    bool hasEdges(int id) const { return nodeExists(id) && nodes[id].adj != nullptr; }
    int countEdges(int id) const;
};

// ========================= IMPLEMENTATIONS =========================

inline int Graph::addLocation(const string& name, double lat, double lon, LocationType type) {
    // Input validation
    if (!isValidLocationName(name)) {
        cerr << "Error: Invalid location name.\n";
        return -1;
    }

    if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
        cerr << "Error: Invalid coordinates. Lat: -90 to 90, Lon: -180 to 180.\n";
        return -1;
    }

    if (!isValidLocationType(type)) {
        cerr << "Error: Invalid location type.\n";
        return -1;
    }

    if (nodeCount >= MAX_LOC) {
        cerr << "Error: Maximum location limit reached (" << MAX_LOC << ").\n";
        return -1;
    }

    // Check for duplicate location name
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].name == name) {
            cerr << "Error: Location with name '" << name << "' already exists.\n";
            return -1;
        }
    }

    int id = nodeCount++;
    nodes[id].id = id;
    nodes[id].name = name;
    nodes[id].coord = { lat, lon };
    nodes[id].type = type;

    // Connect to nearby nodes within radius
    const double CONNECTION_RADIUS = 5.0; // 5km radius
    int connectionsMade = 0;
    for (int i = 0; i < id; i++) {
        double d = haversine(lat, lon, nodes[i].coord.lat, nodes[i].coord.lon);
        if (d <= CONNECTION_RADIUS) {
            // Create bidirectional edge
            Edge* e1 = new Edge(i, d);
            e1->next = nodes[id].adj;
            nodes[id].adj = e1;

            Edge* e2 = new Edge(id, d);
            e2->next = nodes[i].adj;
            nodes[i].adj = e2;

            connectionsMade++;
        }
    }

    return id;
}

inline int Graph::addLocationWithConnections(const string& name, double lat, double lon,
    LocationType type, const int* connectedIds, int count) {
    // Input validation
    if (!isValidConnectionCount(count)) {
        cerr << "Error: Invalid number of connections.\n";
        return -1;
    }

    // Validate connection IDs
    for (int i = 0; i < count; i++) {
        if (!isValidNodeId(connectedIds[i], nodeCount)) {
            cerr << "Error: Invalid connection ID at index " << i << ".\n";
            return -1;
        }
    }

    int id = addLocation(name, lat, lon, type);
    if (id == -1) return -1;

    int successfulConnections = 0;
    for (int i = 0; i < count; i++) {
        int to = connectedIds[i];
        if (nodeExists(to)) {
            double d = haversine(nodes[id].coord.lat, nodes[id].coord.lon,
                nodes[to].coord.lat, nodes[to].coord.lon);

            // Avoid self-connection
            if (id == to) {
                cerr << "Warning: Skipping self-connection for node " << id << ".\n";
                continue;
            }

            // Check if connection already exists
            bool connectionExists = false;
            for (Edge* e = nodes[id].adj; e; e = e->next) {
                if (e->to == to) {
                    connectionExists = true;
                    break;
                }
            }

            if (!connectionExists) {
                Edge* e1 = new Edge(to, d);
                e1->next = nodes[id].adj;
                nodes[id].adj = e1;

                Edge* e2 = new Edge(id, d);
                e2->next = nodes[to].adj;
                nodes[to].adj = e2;

                successfulConnections++;
            }
        }
    }

    return id;
}

inline void Graph::connect(int u, int v) {
    if (!nodeExists(u) || !nodeExists(v)) {
        cerr << "Error: One or both node IDs are invalid.\n";
        return;
    }

    if (u == v) {
        cerr << "Error: Cannot connect a node to itself.\n";
        return;
    }

    // Check if connection already exists
    for (Edge* e = nodes[u].adj; e; e = e->next) {
        if (e->to == v) {
            cerr << "Warning: Connection already exists between nodes " << u << " and " << v << ".\n";
            return;
        }
    }

    double d = haversine(
        nodes[u].coord.lat, nodes[u].coord.lon,
        nodes[v].coord.lat, nodes[v].coord.lon
    );

    // Validate distance
    if (d == INF) {
        cerr << "Error: Invalid coordinates for distance calculation.\n";
        return;
    }

    if (d > 1000.0) { // Arbitrary large distance limit
        cerr << "Warning: Distance " << d << "km is unusually large for a connection.\n";
    }

    // Add U -> V
    Edge* e1 = new Edge(v, d);
    e1->next = nodes[u].adj;
    nodes[u].adj = e1;

    // Add V -> U (Undirected)
    Edge* e2 = new Edge(u, d);
    e2->next = nodes[v].adj;
    nodes[v].adj = e2;

    // Success message
    // Optionally remove or comment out for silent operation
    // cout << "Connected nodes " << u << " and " << v << " (distance: " << d << " km)\n";
}

inline int Graph::findNodeByName(const string& name) const {
    if (name.empty()) {
        return -1;
    }

    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].name == name)
            return i;
    }
    return -1;
}

inline void Graph::dijkstra(int src, double dist[], int prev[]) const {
    // Validate source node
    if (!nodeExists(src)) {
        cerr << "Error: Invalid source node for Dijkstra.\n";
        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INF;
            prev[i] = -1;
        }
        return;
    }

    bool visited[MAX_LOC] = { false };
    for (int i = 0; i < nodeCount; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int i = 0; i < nodeCount; i++) {
        int u = -1;
        double best = INF;
        for (int j = 0; j < nodeCount; j++) {
            if (!visited[j] && dist[j] < best) {
                best = dist[j];
                u = j;
            }
        }
        if (u == -1) break;
        visited[u] = true;

        for (Edge* e = nodes[u].adj; e; e = e->next) {
            int v = e->to;
            if (dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                prev[v] = u;
            }
        }
    }
}

inline void Graph::getPath(int dest, const int prev[], int path[], int& pathLen) const {
    if (!nodeExists(dest)) {
        cerr << "Error: Invalid destination node.\n";
        pathLen = 0;
        return;
    }

    pathLen = 0;
    for (int v = dest; v != -1; v = prev[v]) {
        if (pathLen >= MAX_LOC) {
            cerr << "Error: Path length exceeded maximum.\n";
            pathLen = 0;
            return;
        }
        path[pathLen++] = v;
    }

    // Reverse the path
    for (int i = 0; i < pathLen / 2; i++) {
        int temp = path[i];
        path[i] = path[pathLen - 1 - i];
        path[pathLen - 1 - i] = temp;
    }
}

inline int Graph::findNearestNodeOfType(double lat, double lon, LocationType type) const {
    if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
        cerr << "Error: Invalid coordinates for nearest node search.\n";
        return -1;
    }

    if (!isValidLocationType(type)) {
        cerr << "Error: Invalid location type for nearest node search.\n";
        return -1;
    }

    int best = -1;
    double minD = INF;
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].type == type) {
            double d = haversine(lat, lon, nodes[i].coord.lat, nodes[i].coord.lon);
            if (d < minD) {
                minD = d;
                best = i;
            }
        }
    }

    if (best == -1) {
        // No nodes of the requested type found
        cerr << "Warning: No nodes of type '" << getTypeName(type) << "' found.\n";
    }

    return best;
}

inline int Graph::findNearestNodeOfTypeFromNode(int srcNode, LocationType type) const {
    if (!nodeExists(srcNode)) {
        cerr << "Error: Invalid source node.\n";
        return -1;
    }

    if (!isValidLocationType(type)) {
        cerr << "Error: Invalid location type.\n";
        return -1;
    }

    double dist[MAX_LOC];
    int prev[MAX_LOC];
    dijkstra(srcNode, dist, prev);

    int best = -1;
    double minD = INF;
    for (int i = 0; i < nodeCount; i++) {
        if (i != srcNode && nodes[i].type == type && dist[i] < minD) {
            minD = dist[i];
            best = i;
        }
    }

    if (best == -1) {
        cerr << "Warning: No reachable nodes of type '" << getTypeName(type) << "' from node " << srcNode << ".\n";
    }

    return best;
}

inline double Graph::getDistance(int u, int v) const {
    if (!nodeExists(u) || !nodeExists(v)) {
        cerr << "Error: Invalid node IDs for distance calculation.\n";
        return INF;
    }

    if (u == v) return 0.0;

    double dist[MAX_LOC];
    int prev[MAX_LOC];
    dijkstra(u, dist, prev);
    return dist[v];
}

inline int* Graph::getNodesByType(LocationType type, int& count) const {
    if (!isValidLocationType(type)) {
        cerr << "Error: Invalid location type.\n";
        count = 0;
        return new int[0];
    }

    int* res = new int[nodeCount];
    count = 0;
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].type == type) {
            res[count++] = i;
        }
    }
    return res;
}

inline int* Graph::getNodesWithinRadius(double lat, double lon, double radius, int& count) const {
    if (!isValidCoordinate(lat, true) || !isValidCoordinate(lon, false)) {
        cerr << "Error: Invalid coordinates for radius search.\n";
        count = 0;
        return new int[0];
    }

    if (!isValidRadius(radius)) {
        cerr << "Error: Invalid radius. Must be between 0 and 1000 km.\n";
        count = 0;
        return new int[0];
    }

    int* res = new int[nodeCount];
    count = 0;
    for (int i = 0; i < nodeCount; i++) {
        double distance = haversine(lat, lon, nodes[i].coord.lat, nodes[i].coord.lon);
        if (distance <= radius) {
            res[count++] = i;
        }
    }
    return res;
}

inline int Graph::countEdges(int id) const {
    if (!nodeExists(id)) return 0;

    int count = 0;
    Edge* e = nodes[id].adj;
    while (e) {
        count++;
        e = e->next;
    }
    return count;
}

inline void Graph::displayNode(int id) const {
    if (!nodeExists(id)) {
        cerr << "Error: Node " << id << " does not exist.\n";
        return;
    }

    const Node& n = nodes[id];
    cout << "ID: " << id << " | " << n.name << " | " << getTypeName(n.type)
        << " | (" << n.coord.lat << ", " << n.coord.lon << ")"
        << " | Connections: " << countEdges(id) << "\n";
}

inline void Graph::displayNodesByType(LocationType type) const {
    if (!isValidLocationType(type)) {
        cerr << "Error: Invalid location type for display.\n";
        return;
    }

    cout << "\n=== " << getTypeName(type) << "S ===\n";
    bool found = false;
    int count = 0;
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].type == type) {
            displayNode(i);
            found = true;
            count++;
        }
    }
    if (!found) {
        cout << "None found.\n";
    }
    else {
        cout << "Total: " << count << " locations\n";
    }
}

inline void Graph::displayAllNodes() const {
    cout << "\n=== ALL LOCATIONS (" << nodeCount << ") ===\n";
    if (nodeCount == 0) {
        cout << "No locations registered yet.\n";
        return;
    }

    for (int i = 0; i < nodeCount; i++) {
        displayNode(i);
    }

    // Display statistics
    cout << "\n=== GRAPH STATISTICS ===\n";
    cout << "Total Nodes: " << nodeCount << "\n";

    int totalEdges = 0;
    for (int i = 0; i < nodeCount; i++) {
        totalEdges += countEdges(i);
    }
    totalEdges /= 2; // Since edges are bidirectional

    cout << "Total Edges: " << totalEdges << "\n";
    cout << "Average Connections per Node: "
        << (nodeCount > 0 ? (double)totalEdges * 2 / nodeCount : 0) << "\n";

    // Display count by type
    cout << "\nLocations by Type:\n";
    for (int type = STOP; type <= OTHER; type++) {
        int count = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i].type == type) count++;
        }
        if (count > 0) {
            cout << "  " << getTypeName(static_cast<LocationType>(type)) << ": " << count << "\n";
        }
    }
}

inline string Graph::getTypeName(LocationType type) const {
    switch (type) {
    case STOP: return "Bus Stop";
    case SCHOOL: return "School";
    case HOSPITAL: return "Hospital";
    case PHARMACY: return "Pharmacy";
    case HOUSE: return "House";
    case MOSQUE: return "Mosque";
    case PARK: return "Park";
    case WATER_COOLER: return "Water Cooler";
    case MALL: return "Mall";
    default: return "Other";
    }
}