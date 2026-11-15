#ifndef STACK_H
#define STACK_H

#include "Request.h"

const int STACK_SIZE = 50;


class Stack
{
private:
    Request items[STACK_SIZE];
    int top;   

public:
    Stack()
    {
        top = -1;
    }

    bool isEmpty()
    {
        return top == -1;
    }

    bool isFull()
    {
        return top == STACK_SIZE - 1;
    }

    int getSize()
    {
        return top + 1;
    }

   
    bool push(Request r)
    {
        if(isFull())
        {
            return false;
        }

        top++;
        items[top] = r;
        return true;
    }

   
    Request pop()
    {
        if(isEmpty())
        {
            return Request();
        }

        Request r = items[top];
        top--;
        return r;
    }
};

#endif