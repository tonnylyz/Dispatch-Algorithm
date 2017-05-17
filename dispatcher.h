#ifndef ALGORITHM_DISPATCHER_H
#define ALGORITHM_DISPATCHER_H

#include <vector>
#include "point.h"
#include "restaurant.h"
#include "district.h"

class dispatcher : public point {
private:
    const unsigned int _index;
public:
    dispatcher(unsigned int index, point location) : _index(index) {
        _x = location.x();
        _y = location.y();
    }

    double moveTo(point target)
    {
        _x = target.x();
        _y = target.y();
        if (restaurant *r = dynamic_cast<restaurant *> (&target)) {
            // TODO: Strategy need here
        } else if (district *r = dynamic_cast<district *> (&target)) {
            // TODO: Strategy need here
        }
        // Return distance (time)
        return distant(target);
    }

    static void initialize(unsigned int n, std::vector<dispatcher> &v) {
        // TODO: Initialize n dispatchers
        for (unsigned int i = 1; i <= n; i++) {
            v.push_back(dispatcher(i, point(0, 0)));
        }
    };
};


#endif //ALGORITHM_DISPATCHER_H
