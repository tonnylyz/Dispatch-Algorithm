#ifndef ALGORITHM_RESTAURANT_H
#define ALGORITHM_RESTAURANT_H


#include <vector>
#include <algorithm>
#include "point.h"
#include "order.h"

class restaurant : point {
private:
public:
    const unsigned int index;
    restaurant(unsigned int index, point location) : index(index) {
        _x = location.x();
        _y = location.y();
        _meal = std::vector<order> ();
    }
};


#endif //ALGORITHM_RESTAURANT_H
