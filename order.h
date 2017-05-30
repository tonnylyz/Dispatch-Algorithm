#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include <vector>
#include <ostream>
#include <iostream>
#include <map>
#include <queue>
#include <assert.h>
#include "point.h"
#include "district.h"
#include "restaurant.h"

extern unsigned int containerSize;

class order {
private:
public:
    const unsigned int index;
    const double time;
    const double timeEstimated;
    restaurant *from;
    district *to;

    order(unsigned int index, restaurant *r, district *d, double time);

    bool loaded;
    bool delivered;

    class orderPoint {
    public:
        enum orderPointType {
            r,
            d
        };
        point * p;
        order * o;
        orderPointType t;
        double time;
        orderPoint(point *p, order *o, orderPointType t) : p(p), o(o), t(t), time(0) {};
    };

    class wrap {
    private:
        std::map<order *, std::vector<orderPoint>> _pathAlt;
        std::map<order *, double> _serviceTime;
		//std::mutex wmutex;

        void _calcTime(std::vector<orderPoint> &path) {
            double time = startTime;
            for (auto iter = path.begin(); iter != path.end(); iter++) {
                if (iter->t == orderPoint::r) {
                    if (time < iter->o->time) {
                        time = iter->o->time;
                        iter->time = iter->o->time;
                    } else {
                        iter->time = time;
                    }
                } else if (iter->t == orderPoint::d) {
                    iter->time = time;
                }
                if (iter + 1 != path.end()) {
                    time += point::dist(iter->p, (iter + 1)->p);
                }
            }
        }

        double _maxTime(const std::vector<orderPoint> &path) {
            double max = 0;
            for (auto &op : path) {
                if (op.t == orderPoint::d && op.time - op.o->time > max) {
                    max = op.time - op.o->time;
                }
            }
            return max;
        }

        double _evaluate(order *o) {
            double minTime = 0;
            std::vector<orderPoint> minPath;
            for (size_t from = deliverPath.size();; from--) {
                if (from != deliverPath.size()
                    && deliverPath[from].t == orderPoint::r
                    && o->timeEstimated < deliverPath[from].time) {
                    break;
                }
                for (size_t to = from + 1; to <= deliverPath.size() + 1; to++) {
                    auto path = std::vector<orderPoint>(deliverPath);
                    path.emplace(path.begin() + from, o->from, o, orderPoint::r);
                    path.emplace(path.begin() + to,   o->to,   o, orderPoint::d);
                    _calcTime(path);
                    double time = _maxTime(path);
                    if (minTime == 0) {
                        minTime = time;
                        minPath = std::vector<orderPoint>(path);
                    } else if (time < minTime) {
                        minTime = time;
                        minPath = std::vector<orderPoint>(path);
                    }
                }
                if (from == 0) {
                    break;
                }
            }
            _pathAlt[o] = std::vector<orderPoint>(minPath);
            _serviceTime[o] = minTime;
            return minTime;
        };

        bool _full() {
            unsigned int maxLoaded = 0, loaded = 0;
            for (auto i = deliverPath.begin(); i != deliverPath.end(); i++) {
                if (i->t == orderPoint::r) {
                    loaded++;
                } else if (i->t == orderPoint::d) {
                    loaded--;
                }
                if (loaded > maxLoaded) {
                    maxLoaded = loaded;
                }
            }
            return maxLoaded >= containerSize;
        }

    public:
        double startTime;
        std::vector<order *> orderList;
        std::vector<orderPoint> deliverPath;

        wrap(order *o) {
            orderList = std::vector<order *>();
            orderList.push_back(o);

            startTime = o->time;

            deliverPath = std::vector<orderPoint>();

            deliverPath.emplace_back((point *) o->from, o, orderPoint::r);
            deliverPath.emplace_back((point *) o->to,   o, orderPoint::d);
            _calcTime(deliverPath);

            _pathAlt = std::map<order *, std::vector<orderPoint>>();
            _serviceTime = std::map<order *, double>();
        }

        double evaluateOrder(order *o) {
            if (_full()) {
                return -1;
            }
            if (_serviceTime.count(o) == 1) {
                return _serviceTime[o];
            }
            return _evaluate(o);
        }

        void addOrder(order *o) {
            if (_pathAlt.count(o) == 0) {
                std::cerr << "A route without evaluation added!" << std::endl;
                return;
            }
            if (o->time < startTime) {
                std::cerr << "A route ahead added!" << std::endl;
                return;
            }
            orderList.push_back(o);
            deliverPath = _pathAlt[o];
            _pathAlt = std::map<order *, std::vector<orderPoint>>();
            _serviceTime = std::map<order *, double>();
        }

        friend std::ostream &operator<<(std::ostream &os, const wrap &w) {
            os << "[wrap] size: " << w.orderList.size()
               << std::endl;
            os << "\t\tOrder: ";
            for (const auto &o : w.orderList) {
                os << "o" << o->index << "(" << *o->from << "," << *o->to << ") ";
            }
            os << std::endl;
            os << "\t\tPath: ";
            for (const auto &p : w.deliverPath) {
                if (restaurant *r = dynamic_cast<restaurant *>(p.p)) {
                    os << (*r) << " ";
                } else if (district *d = dynamic_cast<district *>(p.p)) {
                    os << (*d) << " ";
                }
            }
            os << std::endl;
//            os << "Path (plot): [";
//            for (const auto &p : w.deliverPath) {
//                os << (*p.p) << ",";
//            }
//            os << "]";
//            os << std::endl;
            return os;
        }
    };
};

#endif //ALGORITHM_ORDER_H
