#include "dispatcher.h"

std::vector<dispatcher> * dispatchers;
unsigned int containerSize;

dispatcher::dispatcher(unsigned int index, point location) : index(index) {
    _x = location.x();
    _y = location.y();
    status = idle;
    target = nullptr;
    list = std::vector<order *>();
    path = std::queue<order::orderPoint>();
}


void dispatcher::moveTo(point target) {
    _x = target.x();
    _y = target.y();
}


std::vector<dispatcher *> dispatcher::get(statusType s) {
    std::vector<dispatcher *> result = std::vector<dispatcher *>();
    for (auto &d : *dispatchers) {
        if (d.status == s) {
            result.push_back(&d);
        }
    }
    return result;
};
