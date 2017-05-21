#ifndef ALGORITHM_DISPATCHER_H
#define ALGORITHM_DISPATCHER_H

#include <vector>
#include <set>
#include "point.h"
#include "district.h"
#include "order.h"
#include "restaurant.h"

class dispatcher : public point {

public:

    const unsigned int index;
    enum status {
          idle      // Wait for schedule
        , load      // On the way to restaurant
        , deliver   // On the way to district
    };

    dispatcher(unsigned int index, point location);

    order* toLoad;
    order* toDeliver;

    point *target;
    double timeStart;
    double timeFinish;

    status getStatus() const {
        return _status;
    }

    void setStatus(status s) {
        _status = s;
    }

    double moveTo(point target);

    static std::vector<dispatcher *> get(status s);

private:
    status _status;
};


#endif //ALGORITHM_DISPATCHER_H
