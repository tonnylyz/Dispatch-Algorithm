#ifndef ALGORITHM_DISTRICT_H
#define ALGORITHM_DISTRICT_H

#include <vector>
#include "point.h"

class district : public point {
private:
public:
    const unsigned int index;
    district(unsigned int index, point location);
};


#endif //ALGORITHM_DISTRICT_H
