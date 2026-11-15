#ifndef CORRIDOR_H
#define CORRIDOR_H

// Corridor.h
// A CORRIDOR is one physical bus line written as a sequence of stops, starting from the
// GEU side and going outwards:   GEU -> ISBT -> Rispana -> Jogiwala -> ... -> Doiwala
//
// A bus that runs on a corridor passes ALL the stops of that corridor, so students waiting
// at different stops of the same corridor can share one on-demand bus. That is why the
// threshold is checked on the COMBINED number of students of the whole corridor.
//
// A stop can be on more than one corridor (ISBT, Rispana, Jogiwala are shared by many lines).
// For such a stop we use the corridor which has the most waiting students.
//
// The corridors come from the bus route chart (routes.txt). Stop names are the ones used
// in main.cpp (Harrawala, Miyawala, Mokhampur ... not the spelling variants of the chart).

#include <iostream>
#include <cstring>
#include "InputUtils.h"
#include "Stop.h"
using namespace std;

const int MAX_CORRIDORS = 20;
const int MAX_CORRIDOR_STOPS = 20;
const int CORRIDOR_NAME_SIZE = 30;
const int STOP_NAME_SIZE = 20;          // same size as the name in Stop.h

class Corridor
{
private:
    char name[CORRIDOR_NAME_SIZE];
    char stopNames[MAX_CORRIDOR_STOPS][STOP_NAME_SIZE];
    int stopCount;

public:
    Corridor()
    {
        name[0] = '\0';
        stopCount = 0;
    }

    Corridor(const char n[])
    {
        strncpy(name, n, CORRIDOR_NAME_SIZE - 1);
        name[CORRIDOR_NAME_SIZE - 1] = '\0';
        stopCount = 0;
    }

    const char* getName()
    {
        return name;
    }

    int getStopCount()
    {
        return stopCount;
    }

    const char* getStopName(int i)
    {
        return stopNames[i];
    }

    bool hasStop(const char stop[])
    {
        for(int i = 0; i < stopCount; i++)
        {
            if(equalsIgnoreCase(stopNames[i], stop))
            {
                return true;
            }
        }

        return false;
    }

    // adds the next stop of the corridor, returns false if there is no place,
    // the name is too long, or the stop is already on this corridor
    bool addStop(const char stop[])
    {
        if(stopCount >= MAX_CORRIDOR_STOPS || strlen(stop) >= STOP_NAME_SIZE || hasStop(stop))
        {
            return false;
        }

        strcpy(stopNames[stopCount], stop);
        stopCount++;
        return true;
    }

    // GEU -> first stop -> second stop -> ...
    void showRoute()
    {
        cout << "GEU";

        for(int i = 0; i < stopCount; i++)
        {
            cout << " -> " << stopNames[i];
        }

        cout << endl;
    }
};


class CorridorMap
{
private:
    Corridor corridors[MAX_CORRIDORS];
    int count;

    // makes one corridor from a text like "ISBT|Rispana|Jogiwala" (stops in order, GEU side first)
    void addCorridor(const char name[], const char stopList[])
    {
        bool ok = (count < MAX_CORRIDORS);
        Corridor c(name);

        char piece[STOP_NAME_SIZE];
        int len = 0;

        for(int i = 0; ok; i++)
        {
            char ch = stopList[i];

            if(ch == '|' || ch == '\0')
            {
                piece[len] = '\0';

                if(len > 0 && !c.addStop(piece))
                {
                    ok = false;
                }

                len = 0;

                if(ch == '\0')
                {
                    break;
                }
            }
            else if(len < STOP_NAME_SIZE - 1)
            {
                piece[len] = ch;
                len++;
            }
            else
            {
                ok = false;         // a stop name that is too long
            }
        }

        if(!ok)
        {
            cout << "Warning: corridor \"" << name << "\" could not be loaded completely." << endl;
            return;
        }

        corridors[count] = c;
        count++;
    }

public:
    CorridorMap()
    {
        count = 0;
    }

    int getCount()
    {
        return count;
    }

    Corridor& get(int i)
    {
        return corridors[i];
    }

    // fills found[] with the numbers of ALL the corridors that contain this stop
    // returns how many there are (0 = the stop is on no corridor)
    int findCorridorsOfStop(const char stop[], int found[])
    {
        int n = 0;

        for(int i = 0; i < count; i++)
        {
            if(corridors[i].hasStop(stop))
            {
                found[n] = i;
                n++;
            }
        }

        return n;
    }

    // every line is one physical road, listed from the GEU side outwards
    void loadCorridors()
    {
        // ISBT side
        addCorridor("Clock Tower Corridor",
            "ISBT|Matawala Bagh|Saharanpur Chowk|Darshanlal Chowk|Clock Tower|Astley Hall");

        // Kargi / Daudwala side
        addCorridor("Daudwala Corridor",
            "ISBT|Kargi|Bangali Kothi|Vishnupuram|Mathurawala|Daudwala");

        // Rispana / Doiwala side (the stops after Doiwala are on different roads)
        addCorridor("Doiwala Corridor",
            "ISBT|Rispana|Jogiwala|Mokhampur|Miyawala|Harrawala|Kuanwala|Lachiwala|Doiwala|Laal Tappad|Nepali Farm");
        addCorridor("Ranipokhri Corridor",
            "ISBT|Rispana|Jogiwala|Mokhampur|Miyawala|Harrawala|Kuanwala|Lachiwala|Doiwala|Ranipokhri");
        addCorridor("Nakrounda Corridor",
            "ISBT|Rispana|Jogiwala|Mokhampur|Miyawala|Harrawala|Nakrounda More");

        // Raipur side
        addCorridor("Raipur Corridor",
            "ISBT|Rispana|Jogiwala|Nehrugram|Ring Road|6 No. Puliya|Dobhal Chowk|Raipur|Gujrowali");

        // IT Park side
        addCorridor("IT Park Corridor",
            "ISBT|Kargi|Fountain Chowk|6 No. Puliya|Laadpur|Sahastradhara|Nala Paani Chowk|IT Park|Khirsali Chowk");

        // Dharampur side (two different roads)
        addCorridor("Fountain Chowk Corridor",
            "ISBT|Rispana|Nehru Colony|Fountain Chowk|Balbir Road|Dharampur");
        addCorridor("Race Course Corridor",
            "ISBT|Kargi|Mata Mandir|Dharampur|Race Course");

        // Rajpur side
        addCorridor("Rajpur Corridor",
            "ISBT|Kargi|Rispana|Araghar|Dwarka Store|Survey Chowk|Nanni Bakery|Dilaram Bazar|Great Value|Rajpur");

        // Shimla Bypass side (the chart gives only Kulhal-ISBT, so Kulhal is kept at the end)
        addCorridor("Shimla Bypass Corridor",
            "ISBT|Mehuwala|Telpur|Badowala|Naya Gaon Palio|Kulhal");

        // GMS Road side
        addCorridor("Garhi Cantt Corridor",
            "GMS Road|Ballupur|ONGC Chowk|Garhi Cantt|CM House|Chir Bagh|Vijay Colony|Hathibarkala");
        addCorridor("Rajender Nagar Corridor",
            "GMS Road|Ballupur|Blood Bank|Kishan Nagar|Bindal Pul|Yamuna Colony|Rajender Nagar");

        // Premnagar / Vikasnagar side
        addCorridor("Vikasnagar Corridor",
            "GMS Road|Balliwala Chowk|Vasant Vihar|Panditwari|Premnagar|Nanda Ki Chowki|Sudhowala|Selaqui|Sahaspur|Langa Road|Herbertpur|Tehsil|Vikasnagar");
        addCorridor("Rajawala Corridor",
            "GMS Road|Panditwari|Premnagar|Nanda Ki Chowki|Sudhowala|Manduwala|Bahuwala|Rajawala");
    }
};


// total students waiting on ALL the stops of one corridor (this is the aggregation)
int countWaitingInCorridor(Corridor &c, Stop stops[], int stopCount)
{
    int total = 0;

    for(int i = 0; i < c.getStopCount(); i++)
    {
        int s = findStopByName(stops, stopCount, c.getStopName(i));

        if(s != -1)
        {
            total = total + stops[s].getWaitingCount();
        }
    }

    return total;
}

// A stop on many corridors: choose the corridor with the most waiting students
// (if two are equal, the first one wins).
// candidates gets the number of corridors that contain the stop.
// returns -1 if the stop is on no corridor.
int pickBestCorridor(CorridorMap &map, const char stop[], Stop stops[], int stopCount, int &candidates)
{
    int found[MAX_CORRIDORS];
    candidates = map.findCorridorsOfStop(stop, found);

    int best = -1;
    int bestCount = -1;

    for(int i = 0; i < candidates; i++)
    {
        int waiting = countWaitingInCorridor(map.get(found[i]), stops, stopCount);

        if(waiting > bestCount)
        {
            bestCount = waiting;
            best = found[i];
        }
    }

    return best;
}

// safety check at the start: every stop written in a corridor must exist in the stops array
// (a spelling mistake would silently break the aggregation). Prints a warning for each problem.
int checkCorridorStops(CorridorMap &map, Stop stops[], int stopCount)
{
    int problems = 0;

    for(int i = 0; i < map.getCount(); i++)
    {
        Corridor &c = map.get(i);

        for(int j = 0; j < c.getStopCount(); j++)
        {
            if(findStopByName(stops, stopCount, c.getStopName(j)) == -1)
            {
                cout << "Warning: stop \"" << c.getStopName(j) << "\" of " << c.getName()
                     << " is not in the stop list." << endl;
                problems++;
            }
        }
    }

    return problems;
}

#endif
