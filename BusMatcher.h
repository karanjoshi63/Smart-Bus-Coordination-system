#ifndef BUSMATCHER_H
#define BUSMATCHER_H

#include "Bus.h"


void sortBuses(Bus buses[], int n) 
{
    for (int i = 0; i < n - 1; i++) 
    {
        for (int j = 0; j < n - i - 1; j++)
         {
            if (buses[j].getTime() > buses[j + 1].getTime())
             {
                Bus temp = buses[j];
                buses[j] = buses[j + 1];
                buses[j + 1] = temp;
            }
        }
    }
}


int findNextBus(Bus buses[], int n, int endTime)
 {
    int low = 0;
    int high = n - 1;
    int answer = -1;

    while (low <= high)
     {
        int mid = (low + high) / 2;

        if (buses[mid].getTime() >= endTime)
         {
            answer = buses[mid].getTime();   
            high = mid - 1;
        }
         else 
         {
            low = mid + 1;
        }
    }

    return answer;
}

#endif