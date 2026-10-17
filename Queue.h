#ifndef QUEUE_H
#define QUEUE_H

#include "Request.h"

const int QUEUE_SIZE = 50;

class Queue
{
    private:
    Request items[QUEUE_SIZE];
    int front;
    int rear;
    int count;

    public:
    Queue()
    {
        front = 0;
        rear = -1;
        count =0;
    }

    bool isEmpty()
    {
        return count ==0;
    }

    bool isFull()
    {
        return count == QUEUE_SIZE;
    }

    int getSize()
    {
        return count;
    }

    bool enqueue(Request r)
    {
        if(isFull())
        {
            return false;

        }

        rear = (rear + 1)% QUEUE_SIZE;
        items[rear] = r;
        count++;
        return true;
    }

    Request dequeue()
    {
        if(isEmpty())
        {
            return Request();
        }

        Request r = items[front];
        front = (front +1)% QUEUE_SIZE;
        count--;
        return r;
    }

    Request getAt(int i)
    {
        return items[(front +i) %QUEUE_SIZE];
    }
};

#endif