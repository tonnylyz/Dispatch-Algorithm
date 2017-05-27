#ifndef ALGORITHM_DISTRICT_H
#define ALGORITHM_DISTRICT_H

#include <vector>
#include "point.h"

class district : public point {
private:
public:
    const unsigned int index;
    district(unsigned int index, point location);
    friend std::ostream& operator<<(std::ostream& os, const district d) {
        os << "d" << d.index;
        return os;
    }
};


#endif //ALGORITHM_DISTRICT_H
