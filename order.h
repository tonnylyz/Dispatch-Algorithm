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
        orderPoint(point *p, order *o, orderPointType t) : p(p), o(o), t(t) {};
    };

    class wrap {
    private:

        std::map<order *, std::vector<orderPoint>> _pathAlt;
        std::map<order *, double> _serviceTime;

        double _evaluate(order *o) {
			auto path = std::vector<point *>();
			for (auto &i : deliverPath) {
				path.push_back(i.p);
			}

            std::vector<point *>::iterator iter, from, to;
			int from_index, to_index;
            //TODO: We never insert a restaurant to the end of a path
            from = path.begin();
            double loadCost = point::dist(path.front(), o->from);
            for (iter = path.begin(); iter != path.end() - 1; iter++) {
                double a = point::dist(o->from, *iter);
                double b = point::dist(o->from, *(iter + 1));
                double c = point::dist(*iter, *(iter + 1));
                double cost = a + b - c;
                if (cost < loadCost) {
                    from = iter + 1;
                    loadCost = cost;
                }
            }
            from = path.insert(from, (point *) o->from);
			from_index = from - path.begin();
            to = path.end();
            double deliverCost = point::dist(path.back(), o->to);

            for (iter = from; iter != path.end() - 1; iter++) {
                double a = point::dist(o->to, *iter);
                double b = point::dist(o->to, *(iter + 1));
                double c = point::dist(*iter, *(iter + 1));
                double cost = a + b - c;
                if (cost < deliverCost) {
                    to = iter + 1;
                    deliverCost = cost;
                }
            }

            to = path.insert(to, (point *) o->to);
			to_index = to - path.begin();
            orderList.push_back(o);

            std::map<order *, double> timeMap;

            double time = startTime;
            for (iter = path.begin(); iter != path.end(); iter++) {
                for (auto &order : orderList) {
                    if (*iter == order->from) {
                        if (time < order->time) {
                            time = order->time;
                        }
                    }
                    if (*iter == order->to) {
                        timeMap[order] = time - startTime;
                    }
                    if (iter + 1 != path.end()) {
                        time += point::dist(*iter, *(iter + 1));
                    }
                }
            }

            double max = 0;
            for (auto i = timeMap.begin(); i != timeMap.end(); i++) {
                if (i->second > max) {
                    max = i->second;
                }
            }

            orderList.pop_back();

            _pathAlt[o] = std::vector<orderPoint>(deliverPath);
            _pathAlt[o].insert(
                    _pathAlt[o].begin() + from_index,
                    orderPoint(o->from, o, orderPoint::r)
            );
            _pathAlt[o].insert(
                    _pathAlt[o].begin() + to_index,
                    orderPoint(o->to, o, orderPoint::d)
            );
            _serviceTime[o] = max;
            return max;
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
            deliverPath.push_back(orderPoint((point *) o->from, o, orderPoint::r));
            deliverPath.push_back(orderPoint((point *) o->to,   o, orderPoint::d));

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

            orderList.push_back(o);
            deliverPath = _pathAlt[o];

            if (o->time < startTime) {
                startTime = o->time;
            }

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
