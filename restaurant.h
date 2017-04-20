#ifndef ALGORITHM_RESTAURANT_H
#define ALGORITHM_RESTAURANT_H


#include <vector>
#include "point.h"

class restaurant {
private:
public:

    const unsigned int index;
    const point location;
    restaurant(unsigned int index, point location) : index(index), location(location) {

    }
};


#endif //ALGORITHM_RESTAURANT_H
