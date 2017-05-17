#ifndef ALGORITHM_RESTAURANT_H
#define ALGORITHM_RESTAURANT_H

#include "point.h"

class restaurant : public point {
private:
public:
    const unsigned int index;
    restaurant(unsigned int index, point location) : index(index) {
        _x = location.x();
        _y = location.y();
    }
};


#endif //ALGORITHM_RESTAURANT_H
