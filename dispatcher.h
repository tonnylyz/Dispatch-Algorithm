#ifndef ALGORITHM_DISPATCHER_H
#define ALGORITHM_DISPATCHER_H

#include <vector>
#include "point.h"
class dispatcher {
private:
    const unsigned int _index;
public:
    point location;
    dispatcher(unsigned int index, point location) : _index(index) {
        this->location = location;
    };

    static void initialize(unsigned int n, std::vector<dispatcher> &v) {
        // TODO: Initialize n dispatchers
        for (unsigned int i = 1; i <= n; i++) {
            v.push_back(dispatcher(i, point(0, 0)));
        }
    };
};


#endif //ALGORITHM_DISPATCHER_H
