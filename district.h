#ifndef ALGORITHM_DISTRICT_H
#define ALGORITHM_DISTRICT_H


#include <vector>
#include "point.h"

class district {
private:
public:

    const unsigned int index;
    const point location;
    district(unsigned int index, point location) : index(index), location(location) {

    }
};


#endif //ALGORITHM_DISTRICT_H
