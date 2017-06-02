#ifndef ALGORITHM_DISPATCHER_H
#define ALGORITHM_DISPATCHER_H

#include "point.h"
#include "order.h"

class dispatcher : public point {

public:
    const unsigned int index;
	explicit dispatcher(unsigned int index, point location);
	std::vector<order::orderPoint> *path;
};


#endif //ALGORITHM_DISPATCHER_H
