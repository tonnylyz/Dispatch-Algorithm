#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include <vector>
#include <ostream>
#include <iostream>
#include <map>
#include "point.h"
#include "district.h"
#include "restaurant.h"
#include <cassert>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

extern unsigned int containerSize;

class order {
private:
public:
    const unsigned int index;
    const double time;
    const double timeEstimated;
    restaurant *from;
    district *to;

    explicit order(unsigned int index, restaurant *r, district *d, double time);

    class orderPoint {
    public:
        enum orderPointType {
            r, d, m
        };
        point *p; // point pointer
        order *o; // order pointer
        orderPointType t; // type
        double timeEnter;
        double timeLeave;

        orderPoint(point *p, order *o, orderPointType t) : p(p), o(o), t(t), timeEnter(0), timeLeave(0) {};
    };

    class wrap {
    private:
        std::map<order *, std::vector<orderPoint>> _pathAlt;

        void _calcTime(std::vector<orderPoint> &path, bool dynamic) const;

        static double _maxTime(std::vector<orderPoint> &path, size_t from = 0);

        static double _full(std::vector<orderPoint> &path);

        size_t _currentIndex(double time);

        point *_currentPoint(double time);

    public:
        double startTime;
        std::vector<order *> orderList;
        std::vector<orderPoint> deliverPath;

        explicit wrap(order *o);

        double evaluateInsert(order *o, bool dynamic);

        void pushBack(order *o, bool dynamic);

        void insert(order *o);

        void setStartTime(double offset, bool dynamic);

        friend std::ostream &operator<<(std::ostream &os, const wrap &w) {
            os << "[wrap] size: " << w.orderList.size()
               << std::endl;
            os << "order list: ";
            for (const auto &o : w.orderList) {
                os << "o" << o->index << "(" << *o->from << "," << *o->to << ") ";
            }
            os << std::endl;
            os << "deliver path: ";
            for (const auto &p : w.deliverPath) {
                if (restaurant *r = dynamic_cast<restaurant *>(p.p)) {
                    os << *r << " ";
                } else if (district *d = dynamic_cast<district *>(p.p)) {
                    os << *d << " ";
                } else {
                    os << "m" << *p.p << " ";
                }
            }
            os << std::endl;
            return os;
        }

        static void printPath(std::ostream &os, std::vector<orderPoint> *path, point start);
    };
};

#endif //ALGORITHM_ORDER_H
