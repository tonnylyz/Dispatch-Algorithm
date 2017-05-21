#include "dispatcher.h"

std::vector<dispatcher> * dispatchers;
unsigned int containerSize;

dispatcher::dispatcher(unsigned int index, point location) : index(index) {
    _x = location.x();
    _y = location.y();
    _status = idle;
    toLoad = nullptr;
    toDeliver = nullptr;
    target = nullptr;
}


double dispatcher::moveTo(point target)
{
    _x = target.x();
    _y = target.y();
    //if (restaurant *r = dynamic_cast<restaurant *> (&target)) {
    //    // TODO: Strategy need here
    //} else if (district *r = dynamic_cast<district *> (&target)) {
    //    // TODO: Strategy need here
    //}
    //// Return distance (time)
    return distant(target);
}


std::vector<dispatcher *> dispatcher::get(status s) {
    std::vector<dispatcher *> result = std::vector<dispatcher *>();
    for (auto &d : *dispatchers) {
        if (d._status == s) {
            result.push_back(&d);
        }
    }
    return result;
};
