#ifndef ALGORITHM_DISTRICT_H
#define ALGORITHM_DISTRICT_H

#include "point.h"

class district : public point {
private:
public:
    const unsigned int index;
    district(unsigned int index, point location) : index(index) {
        _x = location.x();
        _y = location.y();
    }

};


#endif //ALGORITHM_DISTRICT_H
