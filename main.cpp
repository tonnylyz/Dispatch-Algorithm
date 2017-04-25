#include <fstream>
#include "main.h"

#include "proc_static.h"

#define DATA_SET "sample.txt"

int main() {
    std::cout << "Algorithm Project" << std::endl;

    std::ifstream in(DATA_SET, std::ios::in);
    if (!in.is_open()) {
        std::cerr << "Unable to open file '" << DATA_SET << "'." << std::endl;
        return 0;
    }

    in >> restaurantNum;
    in >> districtNum;
    in >> dispatcherNum;
    in >> containerSize;
    in >> orderNum;

    // Initialize restaurant
    restaurants = std::vector<restaurant>();
    for (unsigned int i = 1; i <= restaurantNum; i++) {
        double x, y;
        in >> x;
        in >> y;
        restaurant tmp = restaurant(i, point(x, y));
        restaurants.push_back(tmp);
    }

    // Initialize district
    districts = std::vector<district>();
    for (unsigned int i = 1; i <= districtNum; i++) {
        double x, y;
        in >> x;
        in >> y;
        district tmp = district(i, point(x, y));
        districts.push_back(tmp);
    }

    // Initialize dispatcher
    dispatcher::initialize(dispatcherNum, dispatchers);

    // Initialize order
    orders = std::vector<order>();
    for (unsigned int i = 1; i <= orderNum; i++) {
        double time;
        unsigned int r;
        unsigned int d;
        in >> time;
        in >> r;
        in >> d;
        orders.push_back(order(restaurants[r], districts[d], time));
    }

    process();

    return 0;
}