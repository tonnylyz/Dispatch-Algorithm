#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include <vector>
#include "point.h"
#include "district.h"
#include "restaurant.h"

class order {
private:
public:
    const unsigned int index;
    const double time;
    const double timeEstimated;
    restaurant *from;
    district *to;
    order(unsigned int index, restaurant *r, district *d, double time);

};

#endif //ALGORITHM_ORDER_H
