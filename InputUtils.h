#ifndef INPUTUTILS_H
#define INPUTUTILS_H

// InputUtils.h
// Small helper functions for SAFE and CASE-INSENSITIVE text input.
// Everything is written by hand (no STL, no std::string, no <cctype>).
//
//   toLowerChar / toLowerStr  -> make text lower case
//   equalsIgnoreCase          -> compare two texts, "ml2" == "ML2"
//   cleanSpaces               -> "  Clock   Tower " becomes "Clock Tower"
//   readLine                  -> reads a whole line with cin.getline (never overflows the buffer)
//   readInt                   -> reads a line and turns it into a number
//   parseTime12               -> turns "2:30 PM" into minutes after midnight

#include <iostream>
#include <cstring>
using namespace std;

// becomes true when there is no more input (Ctrl+Z / Ctrl+D or a closed input file)
bool inputEnded = false;

char toLowerChar(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return c + ('a' - 'A');
    }

    return c;
}

void toLowerStr(char s[])
{
    for(int i = 0; s[i] != '\0'; i++)
    {
        s[i] = toLowerChar(s[i]);
    }
}

// true if both texts are the same when upper/lower case is ignored
bool equalsIgnoreCase(const char a[], const char b[])
{
    int i = 0;

    while(a[i] != '\0' && b[i] != '\0')
    {
        if(toLowerChar(a[i]) != toLowerChar(b[i]))
        {
            return false;
        }

        i++;
    }

    return a[i] == b[i];        // both texts must end at the same place
}

bool isSpaceChar(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

// removes the spaces at the start and the end, and keeps only ONE space between words
void cleanSpaces(char s[])
{
    int write = 0;
    bool spacePending = false;

    for(int read = 0; s[read] != '\0'; read++)
    {
        if(isSpaceChar(s[read]))
        {
            if(write > 0)
            {
                spacePending = true;
            }
        }
        else
        {
            if(spacePending)
            {
                s[write] = ' ';
                write++;
                spacePending = false;
            }

            s[write] = s[read];
            write++;
        }
    }

    s[write] = '\0';
}

// shows the prompt and reads one whole line into buf (size = size of the buffer)
//  - cin.getline never writes outside the buffer
//  - extra spaces are removed
//  - if the line is empty or too long, it asks again
// when the input has ended, inputEnded becomes true and buf is empty
void readLine(const char prompt[], char buf[], int size)
{
    while(true)
    {
        cout << prompt;
        cin.getline(buf, size);

        if(cin.fail())
        {
            if(cin.eof())
            {
                inputEnded = true;
                buf[0] = '\0';
                return;
            }

            // the line was longer than the buffer: clear the error and throw the rest of the line away
            cin.clear();
            cin.ignore(1000000, '\n');
            cout << "Input is too long (maximum " << size - 1 << " characters). Try again." << endl;
            continue;
        }

        cleanSpaces(buf);

        if(buf[0] == '\0')
        {
            cout << "Input cannot be empty. Try again." << endl;
            continue;
        }

        return;
    }
}

// turns a text like "25" into the number 25
// returns -1 if the text is not made only of digits (or is too big)
int parseNumber(const char s[])
{
    if(s[0] == '\0')
    {
        return -1;
    }

    int value = 0;

    for(int i = 0; s[i] != '\0'; i++)
    {
        if(s[i] < '0' || s[i] > '9')
        {
            return -1;
        }

        value = value * 10 + (s[i] - '0');

        if(value > 100000)
        {
            return -1;
        }
    }

    return value;
}

// turns a 12-hour time like "2:30 PM", "2:30pm" or "2 pm" (any case) into minutes after midnight
// (2:30 PM -> 870, 12:00 AM -> 0, 12:00 PM -> 720)
// returns -1 if the text is not a valid 12-hour time
int parseTime12(const char s[])
{
    int i = 0;
    int hour = 0;
    int digits = 0;

    while(s[i] >= '0' && s[i] <= '9')
    {
        hour = hour * 10 + (s[i] - '0');
        digits++;
        i++;

        if(digits > 2)
        {
            return -1;
        }
    }

    if(digits == 0 || hour < 1 || hour > 12)
    {
        return -1;
    }

    int minute = 0;

    if(s[i] == ':')
    {
        i++;
        int minuteDigits = 0;

        while(s[i] >= '0' && s[i] <= '9')
        {
            minute = minute * 10 + (s[i] - '0');
            minuteDigits++;
            i++;

            if(minuteDigits > 2)
            {
                return -1;
            }
        }

        if(minuteDigits != 2 || minute > 59)
        {
            return -1;
        }
    }

    if(s[i] == ' ')
    {
        i++;
    }

    char half = toLowerChar(s[i]);

    if(half != 'a' && half != 'p')
    {
        return -1;
    }

    if(toLowerChar(s[i + 1]) != 'm' || s[i + 2] != '\0')
    {
        return -1;
    }

    if(hour == 12)
    {
        hour = 0;               // 12 AM is 0:00, and 12 PM becomes 12:00 below
    }

    if(half == 'p')
    {
        hour = hour + 12;
    }

    return hour * 60 + minute;
}

// reads a line and returns it as a number, or -1 if it is not a valid number
int readInt(const char prompt[])
{
    char line[20];
    readLine(prompt, line, 20);

    if(inputEnded)
    {
        return -1;
    }

    return parseNumber(line);
}

#endif
