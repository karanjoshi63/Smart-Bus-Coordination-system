#ifndef REQUEST_H
#define REQUEST_H

#include "Student.h"

class Request
{
    private:
    Student student;
    int classEndTime;

    public:
    Request()
    {
        classEndTime = 0;
    }

    Request(Student s, int t)
    {
        student = s;
        classEndTime = t;
    }

    const char* getStudentName()
    {
        return student.getName();
    }

    int getClassEndTime()
    {
        return classEndTime;
    }
};

#endif