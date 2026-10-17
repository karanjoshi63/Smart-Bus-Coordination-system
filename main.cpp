#include <iostream>

#include "Bus.h"
#include "Section.h"
#include "TimeUtils.h"
#include "BusMatcher.h"
#include "Timetable.h"

using namespace std;

const int MAX_SECTIONS = 100;

int main()
{

    int busCount = 3;

    Bus buses[3] ={Bus(1080), Bus(780), Bus(960) };


    sortBuses(buses, busCount);

    Section sections[MAX_SECTIONS];

    int sectionCount = loadTimetable(sections, MAX_SECTIONS);


    int choice = -1;

    while(choice != 0)
    {
        cout << "---- MENU -----" << endl;

        cout << "1. Show timetable with next bus for all sections" << endl;
        cout << "2. Find next bus for a section" << endl;
        cout << "3. Find next bus for any time" << endl;
        cout << "0. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;



        if(choice == 1)
        {
            for(int i = 0; i < sectionCount; i++)
            {
                int bus = findNextBus( buses,busCount,sections[i].getEndTime());

                cout << sections[i].getName()<< " - "<< sections[i].getDay()<< " ends at ";

                printTime(sections[i].getEndTime());


                if(bus == -1)
                {
                    cout << " no bus left today" << endl;
                }
                else
                {
                    cout << " next bus at ";
                    printTime(bus);
                    cout << endl;
                }
            }
        }


        else if(choice == 2)
        {
            char name[20];
            char day[10];

            cout << "Enter your section (like A or ML1 ): ";
            cin >> name;

            cout << "Enter day (like Monday): ";
            cin >> day;


            int position = findSection( sections,sectionCount,name,day);


            if(position == -1)
            {
                cout << "Section not found in the timetable"<< endl;
            }
            else
            {
                int endTime = sections[position].getEndTime();

                int bus = findNextBus(buses, busCount, endTime);


                cout << "\nYour section: " << sections[position].getName()<< endl;
                cout << "Day: "<< sections[position].getDay() << endl;
                cout << "Your class ends at ";
                printTime(endTime);
                cout << endl;


                if(bus == -1)
                {
                    cout << "No bus left today" << endl;
                }
                else
                {
                    cout << "Your next bus is at ";

                    printTime(bus);

                    cout << endl;

                    cout << "You will wait "<< bus - endTime << " minutes" << endl;
                }

                cout<<endl;
            }
        }


      

        else if(choice == 3)
        {
            int hour;
            int minute;

            cout << "Enter hour (24 hour format, like 14 for 2 PM): ";
            cin >> hour;

            cout << "Enter minutes: ";
            cin >> minute;


            int givenTime = hour * 60 + minute;


            int bus = findNextBus(buses,busCount,givenTime );


            if(bus == -1)
            {
                cout << "No bus left today" << endl;
            }
            else
            {
                cout << "Next bus at ";

                printTime(bus);

                cout << endl;
            }
        }


      
        else if(choice != 0)
        {
            cout << "Invalid choice!" << endl;
        }
    }



    return 0;
}