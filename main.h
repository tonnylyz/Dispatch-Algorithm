#ifndef ALGORITHM_MAIN_H
#define ALGORITHM_MAIN_H

#include <iostream>
#include <vector>
#include <map>
#include "restaurant.h"
#include "district.h"
#include "dispatcher.h"
#include "order.h"

unsigned int restaurantNum;
unsigned int districtNum;
unsigned int dispatcherNum;
unsigned int containerSize;
unsigned int orderNum;
std::vector<restaurant> restaurants;
std::vector<district> districts;
std::vector<dispatcher> dispatchers;
std::vector<order> orders;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


#endif //ALGORITHM_MAIN_H
