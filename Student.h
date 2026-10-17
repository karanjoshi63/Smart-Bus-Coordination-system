#ifndef STUDENT_H
#define STUDENT_H

#include <cstring>
#include "Person.h"

class Student : public Person
{
    private:
    char stopName[20];

    public:
    Student() : Person()
    {
        stopName[0] = '\0';
    }

    Student(const char n[], const char stop[]) : Person(n)
    {
        strcpy(stopName, stop);
    }

    const char* getStopName()
    {
        return stopName;
    }
};
#endif