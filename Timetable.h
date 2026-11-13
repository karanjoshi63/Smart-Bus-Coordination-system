#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include "Section.h"

using namespace std;

int loadTimetable(Section sections[], int maxcount)
{
    ifstream file("timetable.txt");
   

    if(!file)
    {
        cout << "timetable.txt not found" << endl;
        return 0;
    }

    int count = 0;

    char name[20];
    char day[10];

    int hour;
    int minute;

    while(count < maxcount && file >> name >> day >> hour >> minute)
    {
        sections[count] = Section(name, day, hour * 60 + minute);
        count++;
    }

    file.close();

    return count;
}


int findSection(Section sections[], int count,const char name[], const char day[])
{
    for(int i = 0; i < count; i++)
    {
        if(strcmp(sections[i].getName(), name) == 0 && strcmp(sections[i].getDay(), day) == 0)
        {
            return i;
        }
    }

    return -1;
}

#endif