#include "order.h"
#include "dispatcher.h"

std::vector<order> *orders;

order::order(unsigned int index, restaurant *r, district *d, double time) :
        index(index), time(time), timeEstimated(time + point::dist(static_cast<point>(*r), static_cast<point>(*d))), from(r), to(d) {}
