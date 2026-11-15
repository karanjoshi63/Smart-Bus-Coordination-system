#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

// RequestManager.h  (Level 3 - On-Demand Request System, "brain")
// Flow:
//   1. Student request bhejta hai (class khatam hone ke baad) -> uske stop ki Queue mein
//   2. Har request student ke apne Undo Stack mein bhi jaati hai
//   3. processRequests(): Max-Heap se sabse zyada demand wala stop pehle
//        - waiting >= threshold  -> ON-DEMAND bus (route = Dijkstra GEU -> stop)
//        - threshold nahi + maxWait minutes nikal gaye -> FALLBACK next fixed bus (Binary Search)
//   4. Har allocated bus ek Bus* array mein (polymorphism) + impact metrics

#include <iostream>
#include <iomanip>
#include <string>
#include "TimeUtils.h"
#include "Timetable.h"
#include "BusMatcher.h"
#include "Student.h"
#include "Request.h"
#include "Stack.h"
#include "Bus.h"
#include "RouteGraph.h"
#include "Stop.h"
using namespace std;

const int MAX_STUDENTS = 200;
const int MAX_ALLOCATIONS = 100;

class RequestManager
{
private:
    Timetable& tt;
    BusMatcher& bm;
    RouteGraph& graph;

    Student students[MAX_STUDENTS];
    UndoStack history[MAX_STUDENTS];     // history[i] = students[i] ka apna stack
    int studentCount;

    RequestQueue stopQueues[MAX_STOPS];  // stopQueues[s] = stop s par wait karne wale

    Bus* allocations[MAX_ALLOCATIONS];   // FixedBus + OnDemandBus dono (polymorphism)
    int allocCount;

    int threshold;     // itne students hon to on-demand bus
    int busCapacity;
    int maxWait;       // itne minute tak threshold na bane to fallback
    int sparesLeft;    // kitni spare (extra) buses bachi hain
    int nextSpareNo;   // naming: S1, S2, ...

    int currentDay;    // 0 = MON ... 4 = FRI
    int currentTime;   // minutes
    int universityId;
    bool demoLoaded;

    // Impact metrics
    int servedCount;
    long totalFixedWait;    // agar sirf fixed buses hoti
    long totalActualWait;   // hamare system se

    int findStudent(int roll)
    {
        for (int i = 0; i < studentCount; i++)
        {
            if (students[i].getRollNo() == roll)
            {
                return i;
            }
        }
        return -1;
    }

    void addAllocation(Bus* b)
    {
        if (allocCount < MAX_ALLOCATIONS)
        {
            allocations[allocCount] = b;
            allocCount++;
        }
        else
        {
            delete b;   // log bhar gaya, memory leak na ho
        }
    }

    void recordMetric(Request& r, int actualWait)
    {
        int fixedBus = bm.findNextBus(r.getClassEndTime());
        int fixedWait = calcWaitTime(r.getClassEndTime(), fixedBus);
        if (fixedWait < 0 || actualWait < 0)
        {
            return;   // compare nahi ho sakta (koi fixed bus hi nahi thi)
        }
        servedCount++;
        totalFixedWait = totalFixedWait + fixedWait;
        totalActualWait = totalActualWait + actualWait;
    }

    // Threshold poora -> ek spare bus bhejo (max busCapacity students)
    void dispatchOnDemand(int stopId)
    {
        RequestQueue& q = stopQueues[stopId];

        int travel = 0;
        RouteNode* route = graph.buildRoute(universityId, stopId, travel);   // Dijkstra
        string routeText = "(route not found)";
        if (route != nullptr)
        {
            routeText = graph.routeToString(route);
            graph.deleteRoute(route);
        }

        int boarding = q.size();
        if (boarding > busCapacity)
        {
            boarding = busCapacity;   // baaki agli bus ke liye ruk jayenge
        }

        string busId = "S" + to_string(nextSpareNo);
        nextSpareNo++;
        sparesLeft--;

        cout << " -> ON-DEMAND bus " << busId << " leaves GEU at " << minutesTo12Hour(currentTime)
             << " with " << boarding << " students" << endl;
        cout << "    Route: " << routeText << " (~" << travel << " min)" << endl;

        for (int k = 0; k < boarding; k++)
        {
            Request r = q.dequeue();   // FIFO: pehle aaya, pehle gaya
            int actualWait = currentTime - r.getClassEndTime();
            int fixedWait = calcWaitTime(r.getClassEndTime(), bm.findNextBus(r.getClassEndTime()));

            cout << "    Roll " << r.getRollNo() << ": waited " << actualWait << " min";
            if (fixedWait >= 0)
            {
                cout << " (fixed bus pe " << fixedWait << " min lagte)";
            }
            cout << endl;

            recordMetric(r, actualWait);
        }

        addAllocation(new OnDemandBus(busId, busCapacity, currentTime, boarding, routeText, travel));
    }

    // Threshold nahi bana -> sabko next fixed bus (Binary Search)
    void fallbackToFixed(int stopId)
    {
        RequestQueue& q = stopQueues[stopId];
        int nextBus = bm.findNextBus(currentTime);
        int n = q.size();

        cout << " -> FALLBACK: " << n << " student(s) go by fixed bus at "
             << minutesTo12Hour(nextBus) << endl;
        if (nextBus == -1)
        {
            cout << "    No fixed bus left today - contact transport office." << endl;
        }

        while (!q.isEmpty())
        {
            Request r = q.dequeue();
            if (nextBus != -1)
            {
                recordMetric(r, nextBus - r.getClassEndTime());
            }
        }

        addAllocation(new FixedBus(graph.getStopName(stopId), busCapacity, nextBus, n));
    }

    void processStop(int stopId)
    {
        RequestQueue& q = stopQueues[stopId];
        cout << endl << "[" << graph.getStopName(stopId) << "] waiting: " << q.size()
             << " (threshold " << threshold << ")" << endl;

        // Case 1: threshold poora -> on-demand (capacity se zyada ho to multiple buses)
        while (q.size() >= threshold && sparesLeft > 0)
        {
            dispatchOnDemand(stopId);
        }

        if (q.isEmpty())
        {
            return;
        }

        if (q.size() >= threshold && sparesLeft == 0)
        {
            cout << " -> No spare bus left!" << endl;
            fallbackToFixed(stopId);
            return;
        }

        // Case 2: threshold nahi bana
        int waited = currentTime - q.peek().getRequestTime();
        if (waited >= maxWait)
        {
            cout << " -> Threshold not reached in " << maxWait << " min." << endl;
            fallbackToFixed(stopId);
        }
        else
        {
            cout << " -> Waiting for more students (oldest waited " << waited
                 << " min, fallback after " << maxWait << " min)" << endl;
        }
    }

public:
    RequestManager(Timetable& t, BusMatcher& b, RouteGraph& g,
                   int thr = 5, int cap = 50, int wait = 20, int spares = 3)
        : tt(t), bm(b), graph(g)
    {
        studentCount = 0;
        allocCount = 0;
        threshold = thr;
        busCapacity = cap;
        maxWait = wait;
        sparesLeft = spares;
        nextSpareNo = 1;
        currentDay = 0;
        currentTime = timeToMinutes("13:00");
        universityId = graph.findStop("GEU");
        demoLoaded = false;
        servedCount = 0;
        totalFixedWait = 0;
        totalActualWait = 0;
        for (int i = 0; i < MAX_ALLOCATIONS; i++)
        {
            allocations[i] = nullptr;
        }
    }

    RequestManager(const RequestManager& other) = delete;
    RequestManager& operator=(const RequestManager& other) = delete;

    ~RequestManager()
    {
        for (int i = 0; i < allocCount; i++)
        {
            delete allocations[i];   // virtual destructor ki wajah se sahi class ka delete
        }
    }

    // ---------- Getters / Settings ----------
    int getThreshold()   { return threshold; }
    int getSparesLeft()  { return sparesLeft; }
    int getCurrentDay()  { return currentDay; }
    int getCurrentTime() { return currentTime; }

    bool setClock(int day, int time)
    {
        if (day < 0 || day > 4 || time < 0)
        {
            return false;
        }
        currentDay = day;
        currentTime = time;
        return true;
    }

    bool setSettings(int thr, int cap, int wait, int spares)
    {
        if (thr <= 0 || cap <= 0 || wait <= 0 || spares < 0 || thr > cap)
        {
            return false;
        }
        threshold = thr;
        busCapacity = cap;
        maxWait = wait;
        sparesLeft = spares;
        return true;
    }

    // ---------- Students ----------
    bool registerStudent(int roll, string name, string section, int stopId)
    {
        if (studentCount >= MAX_STUDENTS)
        {
            cout << "Student list full." << endl;
            return false;
        }
        if (findStudent(roll) != -1)
        {
            cout << "Roll " << roll << " already registered." << endl;
            return false;
        }
        if (tt.findSection(section) == -1)   // Hash table lookup
        {
            cout << "Section " << section << " not found in timetable." << endl;
            return false;
        }
        if (!graph.isValidStop(stopId) || stopId == universityId)
        {
            cout << "Invalid home stop." << endl;
            return false;
        }

        students[studentCount] = Student(roll, name, section, stopId, graph.getStopName(stopId));
        studentCount++;
        return true;
    }

    void showStudents()
    {
        cout << endl << "===== REGISTERED STUDENTS (" << studentCount << ") =====" << endl;
        for (int i = 0; i < studentCount; i++)
        {
            students[i].showInfo();
        }
    }

    // ---------- Requests ----------
    bool submitRequest(int roll)
    {
        int idx = findStudent(roll);
        if (idx == -1)
        {
            cout << "Roll " << roll << " not registered." << endl;
            return false;
        }

        Student& s = students[idx];
        int endTime = tt.getEndTime(s.getSection(), currentDay);   // Hash table lookup
        if (endTime == -1)
        {
            cout << "Timetable not found for " << s.getSection() << endl;
            return false;
        }
        if (currentTime < endTime)
        {
            cout << "Roll " << roll << ": class ends at " << minutesTo12Hour(endTime)
                 << ", request after that." << endl;
            return false;
        }

        RequestQueue& q = stopQueues[s.getStopId()];
        if (q.contains(roll))
        {
            cout << "Roll " << roll << " already has a pending request." << endl;
            return false;
        }

        Request r(roll, s.getStopId(), endTime, currentTime);
        q.enqueue(r);
        history[idx].push(r);

        cout << "Request added: Roll " << roll << " -> " << s.getStopName()
             << " (" << q.size() << "/" << threshold << " waiting)" << endl;
        return true;
    }

    // Sirf usi student ka last request undo hota hai
    bool undoLastRequest(int roll)
    {
        int idx = findStudent(roll);
        if (idx == -1)
        {
            cout << "Roll " << roll << " not registered." << endl;
            return false;
        }
        if (history[idx].isEmpty())
        {
            cout << "Nothing to undo." << endl;
            return false;
        }

        Request r = history[idx].pop();
        if (stopQueues[r.getStopId()].removeByRoll(roll))
        {
            cout << "Undo done: request at " << graph.getStopName(r.getStopId())
                 << " (" << minutesTo12Hour(r.getRequestTime()) << ") cancelled." << endl;
            return true;
        }

        cout << "Last request already processed (bus assigned) - undo not possible." << endl;
        return false;
    }

    void showQueues()
    {
        cout << endl << "===== STOP QUEUES (" << dayName(currentDay) << " "
             << minutesTo12Hour(currentTime) << ") =====" << endl;
        bool any = false;
        for (int s = 0; s < graph.getStopCount(); s++)
        {
            if (!stopQueues[s].isEmpty())
            {
                cout << padRight(graph.getStopName(s), 24) << "[" << stopQueues[s].size() << "] ";
                stopQueues[s].print();
                cout << endl;
                any = true;
            }
        }
        if (!any)
        {
            cout << "No pending requests." << endl;
        }
    }

    // ---------- Main processing ----------
    void processRequests()
    {
        StopHeap heap;
        for (int s = 0; s < graph.getStopCount(); s++)
        {
            if (!stopQueues[s].isEmpty())
            {
                heap.push(s, stopQueues[s].size());
            }
        }

        cout << endl << "===== LEVEL 3: PROCESSING REQUESTS (" << dayName(currentDay) << " "
             << minutesTo12Hour(currentTime) << ") =====" << endl;
        cout << "Threshold: " << threshold << " | Max wait: " << maxWait
             << " min | Spare buses left: " << sparesLeft << endl;

        if (heap.isEmpty())
        {
            cout << "No pending requests." << endl;
            return;
        }

        // Max-Heap: sabse zyada demand wala stop pehle
        while (!heap.isEmpty())
        {
            HeapItem top = heap.pop();
            processStop(top.stopId);
        }
    }

    void showAllocations()
    {
        cout << endl << "===== ALLOCATION LOG (" << allocCount << ") =====" << endl;
        if (allocCount == 0)
        {
            cout << "No buses allocated yet." << endl;
        }
        for (int i = 0; i < allocCount; i++)
        {
            cout << i + 1 << ". ";
            allocations[i]->showInfo();   // runtime polymorphism
        }
    }

    void showSummary()
    {
        cout << endl << "===== IMPACT SUMMARY =====" << endl;
        if (servedCount == 0)
        {
            cout << "No students served yet." << endl;
            return;
        }

        double fixedAvg = (double)totalFixedWait / servedCount;
        double ourAvg = (double)totalActualWait / servedCount;

        cout << fixed << setprecision(1);
        cout << "Students served             : " << servedCount << endl;
        cout << "Avg wait (only fixed buses) : " << fixedAvg << " min" << endl;
        cout << "Avg wait (our system)       : " << ourAvg << " min" << endl;
        if (fixedAvg > 0)
        {
            cout << "Improvement                 : " << (fixedAvg - ourAvg) * 100.0 / fixedAvg << " %" << endl;
        }
        cout.unsetf(ios::fixed);
        cout << setprecision(6);
    }

    // ---------- Demo data (viva ke liye) ----------
    // TUE ko zyadatar sections 4:55 PM par free -> next fixed bus 6:00 PM (65 min wait)
    void loadDemoData()
    {
        if (demoLoaded)
        {
            cout << "Demo data already loaded." << endl;
            return;
        }
        demoLoaded = true;

        int rispana = graph.findStop("Rispana");
        int raipur = graph.findStop("Raipur");
        int premNagar = graph.findStop("Prem Nagar");

        // Sample students (dummy data)
        registerStudent(101, "Aman", "CSE-A", rispana);
        registerStudent(102, "Riya", "CSE-B", rispana);
        registerStudent(103, "Kunal", "CSE-C", rispana);
        registerStudent(104, "Sneha", "CSE-D", rispana);
        registerStudent(105, "Rohit", "CSE-E", rispana);
        registerStudent(106, "Neha", "CSE-F", rispana);
        registerStudent(107, "Arjun", "CSE-G", raipur);
        registerStudent(108, "Pooja", "CSE-H", raipur);
        registerStudent(109, "Vikas", "CSE-I", premNagar);

        setClock(dayToIndex("TUE"), timeToMinutes("17:00"));
        cout << "Demo clock set to TUE 5:00 PM" << endl;
        for (int roll = 101; roll <= 109; roll++)
        {
            submitRequest(roll);
        }

        cout << endl << "Next: 'Process requests' -> Rispana gets on-demand bus." << endl;
        cout << "Then set clock to TUE 17:25 and process again -> Raipur/Prem Nagar fallback." << endl;
    }
};

#endif
