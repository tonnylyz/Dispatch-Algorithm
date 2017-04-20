#include "main.h"

#include "proc_static.h"

int main() {
    std::cout << "Algorithm Project" << std::endl;

    std::cin >> restaurantNum;
    std::cin >> districtNum;
    std::cin >> dispatcherNum;
    std::cin >> containerSize;
    std::cin >> orderNum;

    // Initialize restaurant
    restaurants = std::vector<restaurant>();
    for (unsigned int i = 1; i <= restaurantNum; i++) {
        double x, y;
        std::cin >> x;
        std::cin >> y;
        restaurant tmp = restaurant(i, point(x, y));
        restaurants.push_back(tmp);
    }

    // Initialize district
    districts = std::vector<district>();
    for (unsigned int i = 1; i <= districtNum; i++) {
        double x, y;
        std::cin >> x;
        std::cin >> y;
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
        std::cin >> time;
        std::cin >> r;
        std::cin >> d;
        orders.push_back(order(restaurants[r], districts[d], time));
    }

    process();

    return 0;
}