#ifndef SECTION_H
#define SECTION_H

#include <cstring>

class Section
{
private:
    char name[20];
    char day[10];
    int endTime;

public:

    Section()
    {
        name[0] = '\0';
        day[0] = '\0';
        endTime = 0;
    }

    Section(const char n[], const char d[], int t)
    {
        strcpy(name, n);
        strcpy(day, d);
        endTime = t;
    }

    const char* getName()
    {
        return name;
    }

    const char* getDay()
    {
        return day;
    }

    int getEndTime()
    {
        return endTime;
    }
};

#endif