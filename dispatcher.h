#ifndef ALGORITHM_DISPATCHER_H
#define ALGORITHM_DISPATCHER_H

#include <vector>
#include <set>
#include <queue>
#include "point.h"
#include "district.h"
#include "order.h"
#include "restaurant.h"

class dispatcher : public point {

public:

    const unsigned int index;
    enum __status {
          idle      // Wait for schedule
        , load      // On the way to restaurant
        , deliver   // On the way to district
    };

    dispatcher(unsigned int index, point location);

    point *target;
    std::vector<order *> list;
    std::queue<point *> path;

    __status status;

    void moveTo(point target);

    static std::vector<dispatcher *> get(__status s);

	friend std::ostream& operator<<(std::ostream& os, const dispatcher &d) {
		os << "Dispatcher #" << d.index << " " << (point)d << std::endl;
		return os;
	}
};


#endif //ALGORITHM_DISPATCHER_H
