#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <iostream>
using namespace std;

void printTime(int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;

    const char* ampm = "AM";
    if (h >= 12)
    {
        ampm = "PM";
    }
    if (h > 12)
    {
        h = h - 12;
    }
    if (h == 0) 
    {
        h = 12;
    }

    cout << h << ":";
    if (m < 10) 
    {
        cout << "0";
    }
    cout << m << " " << ampm;
}

#endif