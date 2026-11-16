#ifndef ROUTEGRAPH_H
#define ROUTEGRAPH_H

// RouteGraph.h
// The bus stops and the roads between them are saved as a Graph (adjacency list).
// Every road has a travel time in minutes.
// Dijkstra finds the road with the least time from GEU to any stop.
// The answer (the route) comes back as a Linked List of stops.
//
// The travel times are only ESTIMATES, the bus route chart has no timings.

#include <iostream>
#include <cstring>
using namespace std;

const int MAX_STOPS = 40;
const int INF = 1000000000;

// one road going out of a stop (a node of the adjacency list)
struct EdgeNode
{
    int to;             // which stop the road goes to
    int minutes;        // time taken on this road
    EdgeNode *next;
};

// one stop of a route (a node of the route linked list)
struct RouteNode
{
    int stopId;
    RouteNode *next;
};

class RouteGraph
{
private:
    char stopNames[MAX_STOPS][30];
    EdgeNode *adj[MAX_STOPS];       // adj[i] = all the roads going out of stop i
    int stopCount;

    // copying is not allowed (two graphs would free the same memory)
    RouteGraph(const RouteGraph &other);
    RouteGraph &operator=(const RouteGraph &other);

    void addEdgeOneWay(int from, int to, int minutes)
    {
        EdgeNode *e = new EdgeNode;
        e->to = to;
        e->minutes = minutes;
        e->next = adj[from];        // new road goes at the front of the list
        adj[from] = e;
    }

public:
    RouteGraph()
    {
        stopCount = 0;

        for(int i = 0; i < MAX_STOPS; i++)
        {
            adj[i] = NULL;
        }
    }

    // gives back all the memory taken by the roads
    ~RouteGraph()
    {
        for(int i = 0; i < MAX_STOPS; i++)
        {
            EdgeNode *current = adj[i];

            while(current != NULL)
            {
                EdgeNode *temp = current;
                current = current->next;
                delete temp;
            }

            adj[i] = NULL;
        }
    }

    // adds a stop, returns its number (if it is already there, its old number)
    // returns -1 if there is no place or the name is too long
    int addStop(const char name[])
    {
        int old = findStop(name);

        if(old != -1)
        {
            return old;
        }

        if(stopCount >= MAX_STOPS || strlen(name) >= 30)
        {
            return -1;
        }

        strcpy(stopNames[stopCount], name);
        stopCount++;
        return stopCount - 1;
    }

    // returns the number of a stop, or -1 if the stop is not in the graph
    int findStop(const char name[])
    {
        for(int i = 0; i < stopCount; i++)
        {
            if(strcmp(stopNames[i], name) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    // a road that can be used in both directions
    bool addRoad(const char a[], const char b[], int minutes)
    {
        int x = findStop(a);
        int y = findStop(b);

        if(x == -1 || y == -1 || x == y || minutes <= 0)
        {
            return false;
        }

        addEdgeOneWay(x, y, minutes);
        addEdgeOneWay(y, x, minutes);
        return true;
    }

    int getStopCount()
    {
        return stopCount;
    }

    bool isValidStop(int id)
    {
        return id >= 0 && id < stopCount;
    }

    const char* getStopName(int id)
    {
        if(!isValidStop(id))
        {
            return "???";
        }

        return stopNames[id];
    }

    // Dijkstra's algorithm (simple array version, no heap needed for so few stops)
    // dist[i]   = least time from the source to stop i
    // parent[i] = the stop just before i on that fastest route
    void dijkstra(int source, int dist[], int parent[])
    {
        bool done[MAX_STOPS];

        for(int i = 0; i < stopCount; i++)
        {
            dist[i] = INF;
            parent[i] = -1;
            done[i] = false;
        }

        dist[source] = 0;

        for(int step = 0; step < stopCount; step++)
        {
            // pick the stop with the least time which is not finished yet
            int u = -1;

            for(int i = 0; i < stopCount; i++)
            {
                if(!done[i] && dist[i] != INF && (u == -1 || dist[i] < dist[u]))
                {
                    u = i;
                }
            }

            if(u == -1)
            {
                break;      // the remaining stops cannot be reached
            }

            done[u] = true;

            // check every road going out of u, maybe it gives a faster way
            EdgeNode *e = adj[u];

            while(e != NULL)
            {
                if(!done[e->to] && dist[u] + e->minutes < dist[e->to])
                {
                    dist[e->to] = dist[u] + e->minutes;
                    parent[e->to] = u;
                }

                e = e->next;
            }
        }
    }

    // makes the fastest route from -> to as a linked list
    // travelTime gets the total minutes, or -1 if there is no route (then it returns NULL)
    // the list is made with new, so call deleteRoute() after using it
    RouteNode* buildRoute(int from, int to, int &travelTime)
    {
        travelTime = -1;

        if(!isValidStop(from) || !isValidStop(to))
        {
            return NULL;
        }

        int dist[MAX_STOPS];
        int parent[MAX_STOPS];
        dijkstra(from, dist, parent);

        if(dist[to] == INF)
        {
            return NULL;
        }

        travelTime = dist[to];

        // go back from 'to' using parent, and add every stop at the FRONT of the list
        // so the list comes out in the order from -> to
        RouteNode *head = NULL;
        int current = to;

        while(current != -1)
        {
            RouteNode *node = new RouteNode;
            node->stopId = current;
            node->next = head;
            head = node;
            current = parent[current];
        }

        return head;
    }

    void printRoute(RouteNode *head)
    {
        RouteNode *current = head;

        while(current != NULL)
        {
            cout << stopNames[current->stopId];

            if(current->next != NULL)
            {
                cout << " -> ";
            }

            current = current->next;
        }

        cout << endl;
    }

    void deleteRoute(RouteNode *head)
    {
        while(head != NULL)
        {
            RouteNode *temp = head;
            head = head->next;
            delete temp;
        }
    }

    // prints the fastest route and its time
    void printShortestRoute(int from, int to)
    {
        int travel = 0;
        RouteNode *route = buildRoute(from, to, travel);

        if(route == NULL)
        {
            cout << "No route found" << endl;
            return;
        }

        cout << "Shortest route: ";
        printRoute(route);
        cout << "Estimated travel time: " << travel << " minutes" << endl;

        deleteRoute(route);
    }

    // prints every stop with the roads going out of it
    void showNetwork()
    {
        cout << "===== ROUTE NETWORK =====" << endl;

        for(int i = 0; i < stopCount; i++)
        {
            cout << i << ". " << stopNames[i] << " : ";

            EdgeNode *e = adj[i];

            while(e != NULL)
            {
                cout << stopNames[e->to] << "(" << e->minutes << "m)";

                if(e->next != NULL)
                {
                    cout << ", ";
                }

                e = e->next;
            }

            cout << endl;
        }
    }

    // the stops and roads taken from our 5 return routes and the bus route chart
    // GEU is always stop number 0
    void loadRoutes()
    {
        const char *stops[] = {
            "GEU", "ISBT", "Kargi Chowk", "Rispana",
            "Jogiwala", "Mokampur", "Miyawala", "Harrawala", "Kuanwala", "Lachiwala",
            "Doiwala", "Ranipokhri",
            "Fountain Chowk", "Ambiwala", "Nehru Gram", "6 No. Puliya", "Dobhal Chowk",
            "Raipur", "Gujrowali", "Donali", "Bhagwan Das Chowk",
            "Ladpur", "Sahastradhara Crossing", "Nala Paani Chowk", "IT Park",
            "Bangali Kothi", "Matawala Bagh", "Saharanpur Chowk", "Darshanlal Chowk", "Clock Tower",
            "GMS Road", "Ballupur Chowk", "ONGC Chowk", "Panditwari", "Premnagar"
        };

        int n = sizeof(stops) / sizeof(stops[0]);

        for(int i = 0; i < n; i++)
        {
            addStop(stops[i]);
        }

        // roads: two stops which come one after the other in a route get a road
        // the minutes are ESTIMATES

        // common part of all 5 routes
        addRoad("GEU", "ISBT", 8);
        addRoad("ISBT", "Kargi Chowk", 5);
        addRoad("Kargi Chowk", "Rispana", 7);

        // routes 1 and 2: Rispana - Jogiwala - Mokampur - Miyawala - Harrawala - ... Doiwala / Ranipokhri
        addRoad("Rispana", "Jogiwala", 6);
        addRoad("Jogiwala", "Mokampur", 5);
        addRoad("Mokampur", "Miyawala", 4);
        addRoad("Miyawala", "Harrawala", 4);
        addRoad("Harrawala", "Ranipokhri", 25);
        addRoad("Harrawala", "Kuanwala", 4);
        addRoad("Kuanwala", "Lachiwala", 4);
        addRoad("Lachiwala", "Doiwala", 4);
        addRoad("Doiwala", "Ranipokhri", 15);

        // route 3: Rispana - Fountain Chowk - Ambiwala - 6 No. Puliya - Dobhal Chowk - Raipur - Gujrowali - Donali - Bhagwan Das Chowk
        addRoad("Rispana", "Fountain Chowk", 6);
        addRoad("Fountain Chowk", "Ambiwala", 4);
        addRoad("Ambiwala", "6 No. Puliya", 4);
        addRoad("6 No. Puliya", "Dobhal Chowk", 5);
        addRoad("Dobhal Chowk", "Raipur", 5);
        addRoad("Raipur", "Gujrowali", 4);
        addRoad("Gujrowali", "Donali", 4);
        addRoad("Donali", "Bhagwan Das Chowk", 4);

        // route 4: Rispana - Jogiwala - Nehru Gram - 6 No. Puliya
        addRoad("Jogiwala", "Nehru Gram", 5);
        addRoad("Nehru Gram", "6 No. Puliya", 6);

        // route 5: 6 No. Puliya - Ladpur - Sahastradhara - Nala Paani - IT Park
        addRoad("6 No. Puliya", "Ladpur", 5);
        addRoad("Ladpur", "Sahastradhara Crossing", 5);
        addRoad("Sahastradhara Crossing", "Nala Paani Chowk", 4);
        addRoad("Nala Paani Chowk", "IT Park", 4);

        // bus route chart: ISBT side
        addRoad("Kargi Chowk", "Bangali Kothi", 6);
        addRoad("ISBT", "Matawala Bagh", 10);
        addRoad("Matawala Bagh", "Saharanpur Chowk", 5);
        addRoad("ISBT", "Saharanpur Chowk", 12);
        addRoad("Saharanpur Chowk", "Darshanlal Chowk", 8);
        addRoad("Darshanlal Chowk", "Clock Tower", 4);

        // bus route chart: GMS Road side
        addRoad("GEU", "GMS Road", 6);
        addRoad("GMS Road", "Ballupur Chowk", 7);
        addRoad("Ballupur Chowk", "ONGC Chowk", 4);
        addRoad("GMS Road", "Panditwari", 10);
        addRoad("Panditwari", "Premnagar", 5);
    }
};

#endif