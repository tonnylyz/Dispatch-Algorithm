#include "district.h"

std::vector<district> *districts;

district::district(unsigned int index, point location) : index(index) {
    _x = location.x();
    _y = location.y();
}
