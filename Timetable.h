#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include "Section.h"
#include "Hashmap.h"
#include "InputUtils.h"

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
        if(equalsIgnoreCase(sections[i].getName(), name) && equalsIgnoreCase(sections[i].getDay(), day))
        {
            return i;
        }
    }

    return -1;
}

// the key is saved in lower case, so "ML2 Monday" and "ml2 monday" give the same key
// (this is what makes the section search case-insensitive)
void makeKey(const char name[],const char day[], char key[])
{
    strcpy(key,name);
    strcat(key," ");
    strcat(key, day);
    toLowerStr(key);
}

void buildSectionMap(Section sections[], int count, HashMap &map)
{
    char key[40];

    for(int i = 0; i<count; i++)
    {
        makeKey(sections[i].getName(), sections[i].getDay(), key);
        map.put(key,i);
    }
}


int findSectionFast(HashMap &map, const char name[], const char day[])
{
    char key[40];
    makeKey(name, day, key);
    return map.get(key);
}


void fixDay(char day[])
{
    for(int i = 0; day[i] != '\0'; i++)
    {
        if(i == 0)
        {
            day[i] = toupper(day[i]);
        }
        else
        {
            day[i] = tolower(day[i]);
        }
    }
}





#endif