#ifndef ONDEMANDBUS_H
#define ONDEMANDBUS_H

#include "Bus.h"


class OnDemandBus : public Bus {
public:
    OnDemandBus(int t) : Bus(t) {
    }

    virtual void showInfo() {
        cout << "On-demand bus (enough students were waiting) leaving at ";
        printTime(departureTime);
        cout << endl;
    }
};

#endif