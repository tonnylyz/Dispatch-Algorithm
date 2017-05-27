#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include <vector>
#include <ostream>
#include <iostream>
#include <map>
#include "point.h"
#include "district.h"
#include "restaurant.h"

extern int timeSliceSize;

class order {
private:
public:
    const unsigned int index;
    const double time;
    const double timeEstimated;
    restaurant *from;
    district *to;

    order(unsigned int index, restaurant *r, district *d, double time);

    bool finished;

    bool operator<(order const &rhl) const {
        return index < rhl.index;
    };


    class wrap {
    private:

        std::map<order *, std::vector<point *>> _pathAlt;
        std::map<order *, double> _serviceTime;

        double _evaluate(order *o) {

            auto pathStaged = std::vector<point *>(path);

            std::vector<point *>::iterator iter, from, to;

            from = pathStaged.begin();
            double loadCost = point::dist(pathStaged.front(), o->from);
            for (iter = pathStaged.begin(); iter != pathStaged.end() - 1; iter++) {
                double a = point::dist(o->from, *iter);
                double b = point::dist(o->from, *(iter + 1));
                double c = point::dist(*iter, *(iter + 1));
                double cost = a + b - c;
                if (cost < loadCost) {
                    from = iter + 1;
                    loadCost = cost;
                }
            }
            from = pathStaged.insert(from, (point *) o->from);

            to = pathStaged.end();
            double deliverCost = point::dist(pathStaged.back(), o->to);

            for (iter = from; iter != pathStaged.end() - 1; iter++) {
                double a = point::dist(o->to, *iter);
                double b = point::dist(o->to, *(iter + 1));
                double c = point::dist(*iter, *(iter + 1));
                double cost = a + b - c;
                if (cost < deliverCost) {
                    to = iter + 1;
                    deliverCost = cost;
                }
            }

            pathStaged.insert(to, (point *) o->to);
            list.push_back(o);

            std::map<order *, double> timeMap;

            double time = startTime;
            for (iter = pathStaged.begin(); iter != pathStaged.end(); iter++) {
                for (auto &order : list) {
                    if (*iter == order->from) {
                        if (time < order->time) {
                            time = order->time;
                        }
                    }
                    if (*iter == order->to) {
                        timeMap[order] = time - startTime;
                    }
                    if (iter + 1 != pathStaged.end()) {
                        time += point::dist(*iter, *(iter + 1));
                    }
                }
            }

            double max = -1;

            for (auto i = timeMap.begin(); i != timeMap.end(); i++) {
                if (i->second > max) {
                    max = i->second;
                }
            }

            list.pop_back();

            _pathAlt[o] = pathStaged;
            _serviceTime[o] = max;

            return max;
        };

    public:

        double startTime;
        std::vector<order *> list;
        std::vector<point *> path;

        wrap(order *o) {
            list = std::vector<order *>();
            list.push_back(o);

            startTime = o->time;

            path = std::vector<point *>();
            path.push_back((point *) o->from);
            path.push_back((point *) o->to);

            _pathAlt = std::map<order *, std::vector<point *>>();
            _serviceTime = std::map<order *, double>();
        }

        double evaluateOrder(order *o) {
            if (list.empty()) {
                return 0;
            }
            if (_serviceTime.count(o) == 1)
                return _serviceTime[o];

            return _evaluate(o);
        }

        void addOrder(order *o) {
            if (_pathAlt.count(o) == 0) {
                std::cerr << "A route without evaluation added!" << std::endl;
                return;
            }

            list.push_back(o);
            path = _pathAlt[o];

            if (o->time < startTime) {
                startTime = o->time;
            }

            _pathAlt = std::map<order *, std::vector<point *>>();
            _serviceTime = std::map<order *, double>();
        }

        friend std::ostream &operator<<(std::ostream &os, const wrap &w) {
            os << "Wrap size: " << w.list.size()
               << std::endl;
            os << "Order: ";
            for (const auto &o : w.list) {
                os << "o" << o->index << "(" << *o->from << "," << *o->to << ") ";
            }
            os << std::endl;
            os << "Path: ";
            for (const auto &p : w.path) {
                if (restaurant *r = dynamic_cast<restaurant *>(p)) {
                    os << (*r) << " ";
                } else if (district *d = dynamic_cast<district *>(p)) {
                    os << (*d) << " ";
                }
            }
            os << std::endl;
            os << "Path (plot): [";
            for (const auto &p : w.path) {
                os << *p;
                if (p != w.path.back())
                    os << ",";
            }
            os << "]" << std::endl << std::endl;
            return os;
        }
    };
};

#endif //ALGORITHM_ORDER_H
