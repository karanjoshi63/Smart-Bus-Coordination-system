#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <iostream>
using namespace std;

// Helper for case-insensitive string comparison (works for section, day, stop, AM/PM)
inline bool equalsIgnoreCase(const char s1[], const char s2[]) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        char c1 = s1[i];
        char c2 = s2[i];

        if (c1 >= 'A' && c1 <= 'Z') {
            c1 = c1 + 32;
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 = c2 + 32;
        }

        if (c1 != c2) {
            return false;
        }
        i++;
    }
    return s1[i] == '\0' && s2[i] == '\0';
}

// Converts 12-hour format (e.g., 2, 15, "PM") to total minutes from midnight
inline int toMinutes12Hour(int h, int m, const char ampm[]) {
    if (equalsIgnoreCase(ampm, "PM") && h != 12) {
        h = h + 12;
    } else if (equalsIgnoreCase(ampm, "AM") && h == 12) {
        h = 0;
    }
    return h * 60 + m;
}

inline void printTime(int minutes) {
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