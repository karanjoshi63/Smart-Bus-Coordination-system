#ifndef PERSON_H
#define PERSON_H

#include <cstring>

class Person
{
    protected:
    char name[20];

    public:
    Person()
    {
        name[0] = '\0';
    }

    Person(const char n[])
    {
        strcpy(name , n);
    }

    const char* getName()
    {
        return name;
    }
};

#endif