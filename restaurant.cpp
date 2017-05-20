#include "restaurant.h"

std::vector<restaurant> *restaurants;

restaurant::restaurant(unsigned int index, point location) : index(index) {
    _x = location.x();
    _y = location.y();
}
