#ifndef DISPATCH_H
#define DISPATCH_H

// Dispatch.h  (on-demand bus decision for one corridor)
//
// Steps:
//   1. collect the waiting requests of ALL the stops of the corridor
//   2. sort them by class end time (insertion sort)
//   3. cut them into groups: a group starts at the earliest request that is not in a group yet
//      and takes every request that ends within TIME_WINDOW minutes of it
//   4. group size >= threshold  -> ON-DEMAND bus, leaving at the LATEST class end time of the group
//      (students who finish a lab/test a bit late do not miss the bus,
//       and the earlier students still save hours of waiting)
//      group size <  threshold  -> those students use the next fixed bus
//   5. the students who boarded are removed from their stop queues

#include <iostream>
#include "Stop.h"
#include "Bus.h"
#include "OnDemandBus.h"
#include "BusMatcher.h"
#include "TimeUtils.h"
#include "Corridor.h"
using namespace std;

const int TIME_WINDOW = 60;                                         // minutes
const int MAX_CORRIDOR_REQUESTS = MAX_CORRIDOR_STOPS * QUEUE_SIZE;  // every stop queue holds at most QUEUE_SIZE

// one waiting request, found by its stop and its place in that stop's queue
struct WaitingEntry
{
    int stopIndex;      // which stop (position in the stops array)
    int position;       // place in that stop's queue
    int classEnd;       // class end time in minutes
    bool boarded;
};

// copies the info of every waiting request of the corridor into list[], returns how many
int collectWaiting(Corridor &c, Stop stops[], int stopCount, WaitingEntry list[])
{
    int n = 0;

    for(int i = 0; i < c.getStopCount(); i++)
    {
        int s = findStopByName(stops, stopCount, c.getStopName(i));

        if(s == -1)
        {
            continue;
        }

        for(int j = 0; j < stops[s].getWaitingCount(); j++)
        {
            list[n].stopIndex = s;
            list[n].position = j;
            list[n].classEnd = stops[s].getWaiting(j).getClassEndTime();
            list[n].boarded = false;
            n++;
        }
    }

    return n;
}

// insertion sort by class end time (it is stable: equal times keep their order)
void sortByClassEnd(WaitingEntry list[], int n)
{
    for(int i = 1; i < n; i++)
    {
        WaitingEntry key = list[i];
        int j = i - 1;

        while(j >= 0 && list[j].classEnd > key.classEnd)
        {
            list[j + 1] = list[j];
            j--;
        }

        list[j + 1] = key;
    }
}

// minutes a student would wait if there were only fixed buses, -1 if no fixed bus is left
int fixedWaitFor(int classEnd, Bus buses[], int busCount)
{
    int bus = findNextBus(buses, busCount, classEnd);

    if(bus == -1)
    {
        return -1;
    }

    return bus - classEnd;
}

bool isBoarded(WaitingEntry list[], int n, int stopIndex, int position)
{
    for(int i = 0; i < n; i++)
    {
        if(list[i].stopIndex == stopIndex && list[i].position == position)
        {
            return list[i].boarded;
        }
    }

    return false;
}

// takes the boarded students out of their stop queues
// (every queue is emptied and the students who stay are put back in the SAME order)
void removeBoarded(Corridor &c, Stop stops[], int stopCount, WaitingEntry list[], int n)
{
    for(int i = 0; i < c.getStopCount(); i++)
    {
        int s = findStopByName(stops, stopCount, c.getStopName(i));

        if(s == -1)
        {
            continue;
        }

        int total = stops[s].getWaitingCount();

        for(int j = 0; j < total; j++)
        {
            Request r = stops[s].removeFirst();

            if(!isBoarded(list, n, s, j))
            {
                stops[s].addRequest(r);
            }
        }
    }
}

// the whole threshold check for one corridor
void checkCorridorThreshold(Corridor &corridor, Stop stops[], int stopCount,
                            Bus buses[], int busCount, int threshold)
{
    int total = countWaitingInCorridor(corridor, stops, stopCount);

    cout << "Corridor : " << corridor.getName() << endl;
    cout << "Route    : ";
    corridor.showRoute();

    cout << "\nWaiting students on this corridor:" << endl;

    for(int i = 0; i < corridor.getStopCount(); i++)
    {
        int s = findStopByName(stops, stopCount, corridor.getStopName(i));

        if(s != -1 && stops[s].getWaitingCount() > 0)
        {
            cout << "  " << stops[s].getName() << " : " << stops[s].getWaitingCount() << endl;
        }
    }

    cout << "Combined total   : " << total << endl;
    cout << "Required threshold: " << threshold << endl;
    cout << "Time window      : " << TIME_WINDOW << " minutes" << endl;
    cout << "--------------------------------------------" << endl;

    if(total == 0)
    {
        cout << "Nobody is waiting on this corridor" << endl;
        return;
    }

    WaitingEntry list[MAX_CORRIDOR_REQUESTS];
    int n = collectWaiting(corridor, stops, stopCount, list);
    sortByClassEnd(list, n);

    int busesSent = 0;
    int studentsBoarded = 0;
    int groupNumber = 0;
    int start = 0;

    while(start < n)
    {
        // the group = every request within TIME_WINDOW minutes of the earliest one
        int end = start;

        while(end < n && list[end].classEnd - list[start].classEnd <= TIME_WINDOW)
        {
            end++;
        }

        int size = end - start;
        int earliest = list[start].classEnd;
        int latest = list[end - 1].classEnd;      // the list is sorted, so the last one is the latest

        groupNumber++;

        cout << "\nGroup " << groupNumber << ": " << size << " student(s), classes end between ";
        printTime(earliest);
        cout << " and ";
        printTime(latest);
        cout << endl;

        if(size >= threshold)
        {
            // the bus leaves at the LATEST class end time of the group
            OnDemandBus extraBus(latest);
            Bus *busPointer = &extraBus;
            busPointer->showInfo();

            cout << "Students boarding the bus:" << endl;

            for(int k = start; k < end; k++)
            {
                Request r = stops[list[k].stopIndex].getWaiting(list[k].position);
                int actualWait = latest - r.getClassEndTime();
                int fixedWait = fixedWaitFor(r.getClassEndTime(), buses, busCount);

                cout << "  " << r.getStudentName() << " (" << r.getStopName() << ") - waits "
                     << actualWait << " min";

                if(fixedWait == -1)
                {
                    cout << " (no fixed bus left today)";
                }
                else
                {
                    cout << " (fixed bus: " << fixedWait << " min)";
                }

                cout << endl;
                list[k].boarded = true;
            }

            busesSent++;
            studentsBoarded = studentsBoarded + size;
        }
        else
        {
            cout << "Not enough students for a bus, so they take the fixed bus:" << endl;

            for(int k = start; k < end; k++)
            {
                Request r = stops[list[k].stopIndex].getWaiting(list[k].position);
                int bus = findNextBus(buses, busCount, r.getClassEndTime());

                cout << "  " << r.getStudentName() << " (" << r.getStopName() << ") -> ";

                if(bus == -1)
                {
                    cout << "No bus left today" << endl;
                }
                else
                {
                    printTime(bus);
                    cout << endl;
                }
            }
        }

        start = end;
    }

    cout << "\n--------------------------------------------" << endl;
    cout << "On-demand buses sent: " << busesSent << " | Students boarded: " << studentsBoarded << endl;

    removeBoarded(corridor, stops, stopCount, list, n);
}

#endif
