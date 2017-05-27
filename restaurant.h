#ifndef ALGORITHM_RESTAURANT_H
#define ALGORITHM_RESTAURANT_H

#include <vector>
#include "point.h"

class restaurant : public point {
private:
public:
    const unsigned int index;
    restaurant(unsigned int index, point location);
    friend std::ostream& operator<<(std::ostream& os, const restaurant r) {
        os << "r" << r.index;
        return os;
    }
};

#endif //ALGORITHM_RESTAURANT_H
