#include <iostream>
#include <cstring>

#include "Request.h"
#include "Queue.h"
#include "Stack.h"
#include "Stop.h"

#include "Bus.h"
#include "OnDemandBus.h"
#include "Hashmap.h"
#include "RouteGraph.h"
#include "Section.h"
#include "TimeUtils.h"
#include "BusMatcher.h"
#include "Timetable.h"

#include "InputUtils.h"
#include "Corridor.h"
#include "Dispatch.h"

using namespace std;

const int MAX_SECTIONS = 100;

const int Stop_count = 80;
const int Threshold = 3;

// prints the names of all the stops (no numbers) in 3 columns
void showStopNames(Stop stops[], int count)
{
    cout << "\nAvailable stops:" << endl;

    for(int i = 0; i < count; i++)
    {
        const char *stopName = stops[i].getName();
        cout << "  " << stopName;

        for(int k = (int)strlen(stopName); k < 22; k++)
        {
            cout << ' ';
        }

        if((i + 1) % 3 == 0)
        {
            cout << endl;
        }
    }

    cout << endl;
}

// asks for a stop NAME (like "miyawala" or "Clock Tower"), upper/lower case does not matter
// typing ? shows all the stop names
// returns the position of the stop in the stops array, or -1 if there is no such stop
int askForStop(Stop stops[], int count, const char prompt[])
{
    char text[40];

    while(true)
    {
        readLine(prompt, text, 40);

        if(inputEnded)
        {
            return -1;
        }

        if(strcmp(text, "?") == 0 || equalsIgnoreCase(text, "list"))
        {
            showStopNames(stops, count);
            continue;
        }

        int position = findStopByName(stops, count, text);

        if(position == -1)
        {
            cout << "\nStop \"" << text << "\" not found. Type ? at the stop prompt to see all stops." << endl;
        }

        return position;
    }
}

int main()
{

    int busCount = 3;

    Bus buses[3] ={Bus(1090), Bus(790), Bus(970) };


    sortBuses(buses, busCount);

    Section sections[MAX_SECTIONS];

    int sectionCount = loadTimetable(sections, MAX_SECTIONS);
    HashMap sectionMap;
    buildSectionMap(sections, sectionCount, sectionMap);

    // Spelling differences from the chart (Kargi Chowk, Miawala, 6 No. Pulia...) are handled in Stop.h
    Stop stops[Stop_count] = {
        // ISBT / Clock Tower side
        Stop("ISBT"), Stop("Matawala Bagh"), Stop("Saharanpur Chowk"), Stop("Darshanlal Chowk"),
        Stop("Clock Tower"), Stop("Astley Hall"),
        // Kargi / Daudwala side
        Stop("Kargi"), Stop("Bangali Kothi"), Stop("Vishnupuram"), Stop("Mathurawala"), Stop("Daudwala"),
        // Rispana / Doiwala side
        Stop("Rispana"), Stop("Jogiwala"), Stop("Mokhampur"), Stop("Miyawala"), Stop("Harrawala"),
        Stop("Kuanwala"), Stop("Lachiwala"), Stop("Doiwala"), Stop("Laal Tappad"), Stop("Nepali Farm"),
        Stop("Ranipokhri"), Stop("Nakrounda More"),
        // Raipur side
        Stop("Nehrugram"), Stop("Ring Road"), Stop("6 No. Puliya"), Stop("Dobhal Chowk"),
        Stop("Raipur"), Stop("Gujrowali"),
        // GMS Road / Garhi Cantt side
        Stop("GMS Road"), Stop("Ballupur"), Stop("ONGC Chowk"), Stop("Garhi Cantt"), Stop("CM House"),
        Stop("Chir Bagh"), Stop("Vijay Colony"), Stop("Hathibarkala"),
        // Rajender Nagar side
        Stop("Blood Bank"), Stop("Kishan Nagar"), Stop("Bindal Pul"), Stop("Yamuna Colony"), Stop("Rajender Nagar"),
        // IT Park side
        Stop("Nehru Colony"), Stop("Fountain Chowk"), Stop("Laadpur"), Stop("Sahastradhara"),
        Stop("Nala Paani Chowk"), Stop("IT Park"), Stop("Khirsali Chowk"),
        // Dharampur side
        Stop("Mata Mandir"), Stop("Dharampur"), Stop("Balbir Road"), Stop("Race Course"),
        // Shimla Bypass side
        Stop("Mehuwala"), Stop("Telpur"), Stop("Badowala"), Stop("Naya Gaon Palio"), Stop("Kulhal"),
        // Rajpur side
        Stop("Araghar"), Stop("Dwarka Store"), Stop("Survey Chowk"), Stop("Nanni Bakery"),
        Stop("Dilaram Bazar"), Stop("Great Value"), Stop("Rajpur"),
        // Premnagar / Vikasnagar side
        Stop("Balliwala Chowk"), Stop("Vasant Vihar"), Stop("Panditwari"), Stop("Premnagar"),
        Stop("Nanda Ki Chowki"), Stop("Sudhowala"), Stop("Selaqui"), Stop("Sahaspur"), Stop("Langa Road"),
        Stop("Herbertpur"), Stop("Tehsil"), Stop("Vikasnagar"),
        // Rajawala side
        Stop("Manduwala"), Stop("Bahuwala"), Stop("Rajawala")
    };



    Stack history;

    RouteGraph routeGraph;
    routeGraph.loadRoutes();

    int universityId = routeGraph.findStop("GEU");

    // route corridors: stops grouped in sequence along the same physical road
    CorridorMap corridorMap;
    corridorMap.loadCorridors();
    checkCorridorStops(corridorMap, stops, Stop_count);

    int choice = -1;

    while(choice != 0 && !inputEnded)
    {
        cout << "\n============================================" << endl;
        cout << "          SMART BUS COORDINATION SYSTEM     " << endl;
        cout << "============================================" << endl;

        cout << "\n------------------- MENU -------------------" << endl;

        cout <<"  1. Show timetable with next bus for all sections" << endl;
        cout <<"  2. Find next bus for a section" << endl;
        cout <<"  3. Find next bus for any time" << endl;
        cout <<"  4. Student bus request" << endl;
        cout <<"  5. Show students going to each stop" << endl;
        cout <<"  6. Check a stop (threshold)" << endl;
        cout <<"  7.undo last request"<<endl;
        cout <<"  8. show hash map information"<<endl;
        cout <<"  9. show shortest route to a stop"<<endl;
        cout <<"  10. show route network"<<endl;
        cout <<"  0. Exit" << endl;

        cout << "--------------------------------------------" << endl;
        choice = readInt("Enter choice: ");

        if(inputEnded)
        {
            break;
        }

        if(choice == 1)
        {
            cout << "\n============== TIMETABLE DETAILS ==============\n" << endl;

            for(int i = 0; i < sectionCount; i++)
            {
                int bus = findNextBus( buses,busCount,sections[i].getEndTime());

                cout << "\n--------------------------------------------" << endl;
                cout << "Section: " << sections[i].getName()<< " | Day: "<< sections[i].getDay()<< endl;
                cout << "Class ends at: ";

                printTime(sections[i].getEndTime());


                if(bus == -1)
                {
                    cout << "No bus left today" << endl;
                }
                else
                {
                    cout << " Next bus at: ";
                    printTime(bus);
                    cout << endl;
                }
            }

            cout << "\n============================================" << endl;
        }


        else if(choice == 2)
        {
            char name[20];
            char day[10];

            cout << "\n============== FIND NEXT BUS ==============\n" << endl;

            readLine("Enter your section (like A or ML1, any case): ", name, 20);
            readLine("Enter day (like Monday, any case): ", day, 10);


            int position = findSectionFast(sectionMap, name, day);


            if(position == -1)
            {
                cout << "\nSection not found in the timetable"<< endl;
            }
            else
            {
                int endTime = sections[position].getEndTime();

                int bus = findNextBus(buses, busCount, endTime);


                cout << "\n--------------- SECTION DETAILS ---------------" << endl;
                cout << "Your section: " << sections[position].getName()<< endl;
                cout << "Day: "<< sections[position].getDay() << endl;
                cout << "Your class ends at: ";
                printTime(endTime);
                cout << endl;


                if(bus == -1)
                {
                    cout << "No bus left today" << endl;
                }
                else
                {
                    cout << "Your next bus is at: ";

                    printTime(bus);

                    cout << endl;

                    cout << "Waiting time: " << bus - endTime << " minutes" << endl;
                }

                cout << "----------------------------------------------" << endl;
                cout << endl;
            }
        }


        else if(choice == 3)
        {
            char timeText[20];

            cout << "\n============== FIND BUS BY TIME ==============\n" << endl;

            readLine("Enter time (like 2:30 PM or 11:05 AM): ", timeText, 20);

            int givenTime = parseTime12(timeText);

            if(givenTime == -1)
            {
                if(!inputEnded)
                {
                    cout << "\nInvalid time. Write it like 2:30 PM or 11:05 AM" << endl;
                }
            }
            else
            {
                cout << "\nTime entered: ";
                printTime(givenTime);
                cout << endl;

                int bus = findNextBus(buses,busCount,givenTime );

                if(bus == -1)
                {
                    cout << "No bus left today" << endl;
                }
                else
                {
                    cout << "Next bus at: ";

                    printTime(bus);

                    cout << endl;
                }
            }
        }

        else if(choice == 4)
        {
            char studentName[20];
            char sectionName[20];

            char day[10];

            cout << "\n============== STUDENT BUS REQUEST ==============\n" << endl;

            readLine("Enter your name: ", studentName, 20);
            readLine("Enter your section (any case): ", sectionName, 20);
            readLine("Enter day (any case): ", day, 10);

            int position = findSectionFast(sectionMap, sectionName, day);

            if(position == -1)
            {
                if(!inputEnded)
                {
                    cout << "\nSection not found in the timetable" << endl;
                }
            }

            else
            {
                cout << "\nWhere do you want to go? (type the stop name, or ? to see all stops)" << endl;

                int stopIndex = askForStop(stops, Stop_count, "Enter stop name: ");

                if(stopIndex != -1)
                {
                    Student student(studentName, stops[stopIndex].getName());
                    Request request(student, sections[position].getEndTime());

                    if(stops[stopIndex].addRequest(request))
                    {
                        history.push(request);
                        cout << "\nRequest added successfully for " << stops[stopIndex].getName() << endl;
                    }

                    else
                    {
                        cout << "Queue is full" << endl;
                    }
                }
            }
        }

        else if(choice == 5)
        {
            cout << "\n============== STUDENTS AT EACH STOP ==============\n" << endl;

            for(int i = 0; i<Stop_count; i++)
            {
                cout << "\nStop: " << stops[i].getName() << endl;
                cout << "Students waiting: " << stops[i].getWaitingCount() << endl;
                cout << "--------------------------------------------" << endl;

                for(int j = 0; j<stops[i].getWaitingCount(); j++)
                {
                    cout << stops[i].getWaiting(j).getStudentName() << endl;

                }
            }

            cout << "\n============================================" << endl;
        }

        else if(choice == 6)
        {
            cout << "\n============== STOP THRESHOLD CHECK ==============\n" << endl;

            int stopIndex = askForStop(stops, Stop_count, "Enter stop name (or ? to see all stops): ");

            if(stopIndex != -1)
            {
                const char *stopName = stops[stopIndex].getName();

                cout << "\nStop checked: " << stopName << endl;

                // all the stops of the same corridor are counted together
                int candidates = 0;
                int best = pickBestCorridor(corridorMap, stopName, stops, Stop_count, candidates);

                if(candidates > 1)
                {
                    cout << stopName << " is on " << candidates
                         << " corridors, using the one with the most waiting students." << endl;
                }

                cout << "--------------------------------------------" << endl;

                if(best == -1)
                {
                    // a stop that is on no corridor is checked alone
                    Corridor single(stopName);
                    single.addStop(stopName);
                    checkCorridorThreshold(single, stops, Stop_count, buses, busCount, Threshold);
                }
                else
                {
                    checkCorridorThreshold(corridorMap.get(best), stops, Stop_count, buses, busCount, Threshold);
                }
            }
        }

     else if(choice == 7)
        {
            bool removed = false;

            // Skip old history entries whose requests have already boarded.
            while(!history.isEmpty() && !removed)
            {
                Request last = history.pop();

                for(int i = 0; i < Stop_count; i++)
                {
                    if(strcmp(stops[i].getName(), last.getStopName()) == 0)
                    {
                        int total = stops[i].getWaitingCount();

                        if(total > 0)
                        {
                            Request lastWaiting = stops[i].getLast();

                            // Match stop, student and class-end time.
                            // This avoids removing another request by mistake.
                            if(strcmp(lastWaiting.getStudentName(),
                                      last.getStudentName()) == 0 &&
                               lastWaiting.getClassEndTime() ==
                                      last.getClassEndTime())
                            {
                                stops[i].removeLast();
                                removed = true;
                                cout << "Undo done, request of "
                                     << last.getStudentName() << " is removed."
                                     << endl;
                            }
                        }
                        break;
                    }
                }
            }

            if(!removed)
            {
                cout << "Nothing to undo. All remaining requests have "
                     << "already boarded or been removed." << endl;
            }
        }

        else if(choice == 8)
        {

            cout << "Sections saved: " << sectionMap.getCount() << " in " << HASH_SIZE << " slots" << endl;
            cout << "Collisions while saving: " << sectionMap.getCollisions() << endl;

            if(sectionMap.getLastProbes() == 0)
            {
                cout << "Search for a section first (option 2 or 4), then check this again" << endl;
            }
            else
            {
                cout << "Slots checked in the last search: " << sectionMap.getLastProbes() << endl;
            }
        }

        else if(choice == 9)
        {
            cout << "\n============== SHORTEST ROUTE TO A STOP ==============\n" << endl;

            int stopIndex = askForStop(stops, Stop_count, "Enter stop name (or ? to see all stops): ");

            if(stopIndex != -1)
            {
                int stopId = routeGraph.findStop(stops[stopIndex].getName());

                if(stopId == -1)
                {
                    cout << "No route data for this stop" << endl;
                }
                else
                {
                    routeGraph.printShortestRoute(universityId, stopId);
                }
            }
        }


        // ===== NEW: option 10, show the whole network =====
        else if(choice == 10)
        {
            routeGraph.showNetwork();
        }


        else if(choice != 0)
        {
            cout << "\nInvalid choice! Please enter a valid option." << endl;
        }
    }

    cout << "\n============================================" << endl;
    cout << "       Thank you for using our system!      " << endl;
    cout << "============================================" << endl;

    return 0;
}
