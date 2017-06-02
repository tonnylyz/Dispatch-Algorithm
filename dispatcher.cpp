#include "dispatcher.h"

std::vector<dispatcher> * dispatchers;
unsigned int containerSize;

dispatcher::dispatcher(unsigned int index, point location) : index(index) {
    _x = location.x();
    _y = location.y();
	path = nullptr;
}