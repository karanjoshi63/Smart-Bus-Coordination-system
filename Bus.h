#define BUS_H

#include <iostream>
#include "TimeUtils.h"
using namespace std;
class Bus
{
    private:
    int departureTime;

    public:
    Bus(int t)
    {
        departureTime = t;
    }

    int getTime()
    {
        return departureTime;
    }

    virtual void showInfo() 
    {
        cout << "Fixed bus leaving at ";
        printTime(departureTime);
        cout << endl;
    }
};
#endif