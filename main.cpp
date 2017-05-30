#include <fstream>
#include <iostream>

#include "dispatcher.h"
#include "proc_static.h"

#define DATA_SET "data/data00.txt"

extern std::vector<restaurant> *restaurants;
extern std::vector<dispatcher> *dispatchers;
extern std::vector<district> *districts;
extern std::vector<order> *orders;
extern unsigned int containerSize;

int main() {
    std::cout << "Algorithm Project" << std::endl;

    unsigned int restaurantNum = 0;
    unsigned int dispatcherNum = 0;
    unsigned int districtNum = 0;
    unsigned int orderNum = 0;

    restaurants = new std::vector<restaurant>();
    dispatchers = new std::vector<dispatcher>();
    districts = new std::vector<district>();
    orders = new std::vector<order>();

    containerSize = 0;


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
    for (unsigned int i = 1; i <= restaurantNum; i++) {
        double x, y;
        in >> x;
        in >> y;
        restaurant tmp = restaurant(i, point(x, y));
        restaurants->push_back(tmp);
    }

    // Initialize district
    for (unsigned int i = 1; i <= districtNum; i++) {
        double x, y;
        in >> x;
        in >> y;
        district tmp = district(i, point(x, y));
        districts->push_back(tmp);
    }

    // Initialize order
    for (unsigned int i = 1; i <= orderNum; i++) {
        double time;
        unsigned int r;
        unsigned int d;
        in >> time;
        in >> r;
        in >> d;
        orders->push_back(order(i, &(*restaurants)[r - 1], &(*districts)[d - 1], time));
    }

    // Initialize dispatcher
    for (unsigned int i = 1; i <= dispatcherNum; i++) {
        dispatchers->push_back(dispatcher(i, point(0, 0)));
    }

    std::cout << "Print overall information:" << std::endl;
    std::cout << "Restaurant number: " << restaurantNum << std::endl;
    std::cout << "District number: " << districtNum << std::endl;
    std::cout << "Dispatcher number: " << dispatcherNum << std::endl;
    std::cout << "Order number: " << orderNum << std::endl;
    std::cout << "=====================================" << std::endl;

    process();

    delete restaurants;
    delete districts;
    delete orders;
    return 0;
}