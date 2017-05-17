#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include "restaurant.h"
#include "district.h"

class order {
private:
public:
    const double time;
    const restaurant from;
    const district to;
    order(restaurant r, district d, double time) : from(r), to(d), time(time) {

    }
};


#endif //ALGORITHM_ORDER_H
