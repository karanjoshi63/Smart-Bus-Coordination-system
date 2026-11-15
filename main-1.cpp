#include <iostream>
#include <cstring>

#include "Request.h"
#include "Queue.h"
#include "Stack.h"
#include "Stop.h"

#include "Bus.h"
#include "OnDemandBus.h"
#include "HashMap.h"
#include "RouteGraph.h"
#include "Section.h"
#include "TimeUtils.h"
#include "BusMatcher.h"
#include "Timetable.h"

using namespace std;

const int MAX_SECTIONS = 100;

const int Stop_count = 80;
const int Threshold = 3;

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


    int choice = -1;

    while(choice != 0)
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
        cout << "Enter choice: ";
        cin >> choice;



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

            cout << "Enter your section (like A or ML1 ): ";
            cin >> name;
             fixSectionName(name);

          

            cout << "Enter day (like Monday): ";
            cin >> day;
            fixDay(day);


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
            int hour;
            int minute;

            cout << "\n============== FIND BUS BY TIME ==============\n" << endl;

            cout << "Enter hour (24 hour format, like 14 for 2 PM): ";
            cin >> hour;

            cout << "Enter minutes: ";
            cin >> minute;


            int givenTime = hour * 60 + minute;


            int bus = findNextBus(buses,busCount,givenTime );


            if(bus == -1)
            {
                cout << "\nNo bus left today" << endl;
            }
            else
            {
                cout << "\nNext bus at: ";

                printTime(bus);

                cout << endl;
            }
        }

        else if(choice == 4)
        {
            char studentName[20];
            char sectionName[20];

            char day[10];
            int stopNumber;

            cout << "\n============== STUDENT BUS REQUEST ==============\n" << endl;

            cout << "Enter your name: ";
            cin >> studentName;
            
            cout << "Enter your section: ";
            cin >> sectionName;
            fixSectionName(sectionName);
            cout << "Enter day: ";
            cin >> day;
            fixDay(day);

            int position = findSectionFast(sectionMap, sectionName, day);

            if(position == -1)
            {
                cout << "\nSection not found in the timetable" << endl;
            }

            else
            {
                cout << "\nWhere do you want to go? " << endl;
                cout << "--------------------------------------------" << endl;

                for(int i = 0; i<Stop_count; i++)
                {
                    cout << i+1 << ". " << stops[i].getName() << endl;
                }

                cout << "--------------------------------------------" << endl;
                cout << "Enter stop number: ";
                cin >> stopNumber;

                if(stopNumber<1 || stopNumber>Stop_count)
                {
                    cout << "Wrong stop number" << endl;
                }

                else
                {
                    Student student(studentName, stops[stopNumber-1].getName());
                    Request request(student, sections[position].getEndTime());

                    if(stops[stopNumber -1].addRequest(request))
                    {
                        history.push(request);
                        cout << "\nRequest added successfully for " << stops[stopNumber-1].getName() << endl;
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
            int stopNumber;

            cout << "\n============== STOP THRESHOLD CHECK ==============\n" << endl;

            cout << "Enter stop number (1-" << Stop_count << "): ";
            cin >> stopNumber;

            if(stopNumber<1 || stopNumber >Stop_count)
            {
                cout << "Wrong stop number" << endl;
            }

            else
            {
                Stop &stop = stops[stopNumber-1];
                int total = stop.getWaitingCount();

                cout << "\nStudents waiting: " << total << endl;
                cout << "Stop name: " << stop.getName() << endl;
                cout << "Required threshold: " << Threshold << endl;
                cout << "--------------------------------------------" << endl;

                if(total == 0)
                {
                    cout << "Nobody wants to go here" << endl;
                }

                else if(total>=Threshold)
                {
                    int latest = 0;
                    for(int i = 0; i<total; i++)
                    {
                        if(stop.getWaiting(i).getClassEndTime() > latest)
                        {
                            latest = stop.getWaiting(i).getClassEndTime();
                        }
                    }

                    OnDemandBus extraBus(latest);
                    Bus *busPointer = &extraBus;
                    busPointer->showInfo();

                    cout << "\nStudents boarding the bus:" << endl;

                    while(stop.getWaitingCount()>0)
                    {
                        Request r = stop.removeFirst();
                        cout << "  " << r.getStudentName() << " boards the bus" << endl;
                    }

                }

                else
                {
                    cout << "Not enough students, so everyone takes the fixed bus:" << endl;
                    cout << "--------------------------------------------" << endl;

                    for(int i = 0; i<total; i++)
                    {
                        Request r = stop.getWaiting(i);
                        int bus = findNextBus(buses,busCount,r.getClassEndTime());

                        cout << r.getStudentName() << " -> ";

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
            int stopNumber;

            cout << "Choose the stop:" << endl;
            for(int i = 0; i < Stop_count; i++)
            {
                cout << i + 1 << ". " << stops[i].getName() << endl;
            }
            cin >> stopNumber;

            if(stopNumber < 1 || stopNumber > Stop_count)
            {
                cout << "Wrong stop number" << endl;
            }
            else
            {
                int stopId = routeGraph.findStop(stops[stopNumber - 1].getName());

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