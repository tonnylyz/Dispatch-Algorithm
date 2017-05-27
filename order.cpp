#include "order.h"
#include "dispatcher.h"

std::vector<order> *orders;

order::order(unsigned int index, restaurant *r, district *d, double time) :
        index(index), from(r), to(d), time(time), timeEstimated(time + point::dist(*r, *d)) {
    finished = false;
}
