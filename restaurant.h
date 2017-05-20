#ifndef ALGORITHM_RESTAURANT_H
#define ALGORITHM_RESTAURANT_H

#include <vector>
#include "point.h"

class restaurant : public point {
private:
public:
    const unsigned int index;
    restaurant(unsigned int index, point location);
};

#endif //ALGORITHM_RESTAURANT_H
