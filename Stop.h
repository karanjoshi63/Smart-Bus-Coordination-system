#ifndef STOP_H
#define STOP_H

#include <cstring>
#include "Queue.h"

 

class Stop {
private:
    char name[20];
    Queue waiting;
 
public:
    Stop() 
    {
        name[0] = '\0';
    }
 
    Stop(const char n[])
     {
        strcpy(name, n);
    }
 
    const char* getName()
    {
        return name;
    }
 
    bool addRequest(Request r)
    {
        return waiting.enqueue(r);
    }
 
    int getWaitingCount() 
    {
        return waiting.getSize();
    }
 
    Request getWaiting(int i)
    {
        return waiting.getAt(i);
    }
 
    Request removeFirst() 
    {
        return waiting.dequeue();
    }
};
 
#endif
 


  


