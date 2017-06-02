#ifndef ALGORITHM_ORDER_H
#define ALGORITHM_ORDER_H

#include <vector>
#include <ostream>
#include <iostream>
#include <map>
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

	explicit order(unsigned int index, restaurant *r, district *d, double time);

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

        void _calcTime(std::vector<orderPoint> &path) const
        {
	        auto time = startTime;
            for (auto iter = path.begin(); iter != path.end(); ++iter) {
                if (iter->t == orderPoint::r) {
                    if (time < iter->o->time) {
                        time = iter->o->time;
                    }
					iter->time = time;
                } else if (iter->t == orderPoint::d) {
                    iter->time = time;
                }
                if (iter + 1 != path.end()) {
                    time += point::dist(iter->p, (iter + 1)->p);
                }
            }
        }

	    static double _maxTime(std::vector<orderPoint> &path, size_t from = 0) {
            double max = 0;
            for (auto iter = path.begin() + from; iter != path.end(); ++iter) {
	            auto op = *iter;
                if (op.t == orderPoint::d && op.time - op.o->time > max) {
                    max = op.time - op.o->time;
                }
            }
            return max;
        }

        bool _full() const {
            unsigned int maxLoaded = 0, loaded = 0;
            for (auto iter = deliverPath.begin(); iter != deliverPath.end(); ++iter) {
                if (iter->t == orderPoint::r) {
                    loaded++;
                } else if (iter->t == orderPoint::d) {
                    loaded--;
                }
                if (loaded > maxLoaded) {
                    maxLoaded = loaded;
                }
            }
            return maxLoaded >= containerSize;
        }

		std::vector<orderPoint>::iterator _currentPosition(double time)
        {
			auto result = deliverPath.end();
	        for (auto iter = deliverPath.begin(); iter != deliverPath.end(); ++iter)
	        {
		        if (iter->time > time)
		        {
					result = iter + 1;
		        }
	        }
			return result;
        }

    public:
        double startTime;
        std::vector<order *> orderList;
        std::vector<orderPoint> deliverPath;

	    explicit wrap(order *o) {
            orderList = std::vector<order *>();
            orderList.push_back(o);

            startTime = o->time;

            deliverPath = std::vector<orderPoint>();

            deliverPath.emplace_back(o->from, o, orderPoint::r);
            deliverPath.emplace_back(o->to,   o, orderPoint::d);
            _calcTime(deliverPath);

            _pathAlt = std::map<order *, std::vector<orderPoint>>();
        }

		double evaluateInsert(order *o, bool dynamic = false)
        {
			double minTime = 0;
			std::vector<orderPoint> minPath;
			// Here will not insert to end

			for (auto from = dynamic ? _currentPosition(o->time) - deliverPath.begin() : (deliverPath.size() - 1);; from--) {
				if (from >= deliverPath.size())
					return -1;
				if (deliverPath[from].t == orderPoint::r
					&& o->time < deliverPath[from].time) {
					break;
				}
				for (auto to = from + 1; to <= deliverPath.size() + 1; to++) {
					auto path = std::vector<orderPoint>(deliverPath);
					path.emplace(path.begin() + from, o->from, o, orderPoint::r);
					path.emplace(path.begin() + to, o->to, o, orderPoint::d);
					_calcTime(path);
					auto time = _maxTime(path, from) - _maxTime(deliverPath, from);
					if (minTime == 0) {
						minTime = time;
						minPath = std::vector<orderPoint>(path);
					}
					else if (time < minTime) {
						minTime = time;
						minPath = std::vector<orderPoint>(path);
					}
				}
				if (from == 0) {
					break;
				}
			}
			_pathAlt[o] = std::vector<orderPoint>(minPath);
			return minTime;
        }

		void pushBack(order *o)
	    {
			orderList.push_back(o);
			deliverPath.emplace_back(o->from, o, orderPoint::r);
			deliverPath.emplace_back(o->to, o, orderPoint::d);
			_calcTime(deliverPath);

			_pathAlt = std::map<order *, std::vector<orderPoint>>();
	    }

        void insert(order *o) {
            if (_pathAlt.count(o) == 0) {
                std::cerr << "A route without evaluation added!" << std::endl;
                return;
            }
            orderList.push_back(o);
            deliverPath = _pathAlt[o];
            _pathAlt = std::map<order *, std::vector<orderPoint>>();
        }

		void addOffset(double offset)
	    {
			startTime += offset;
			_calcTime(deliverPath);
	    }

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
                }
            }
			os << std::endl;
            return os;
        }

		static void printPath(std::ostream &os, std::vector<orderPoint> *path,  point start)
		{
			if (path->front().p->x() != start.x() || path->front().p->y() != start.y())
			{
				os << path->size() + 1 << std::endl;
				os << start.x() << " ";
				os << start.y() << " ";
				os << "0 ";
				os << "0 ";
				os << "0" << std::endl;
			}
			else
			{
				os << path->size() << std::endl;
			}
			
			size_t load = 0;
			for (auto p = path->begin(); p != path->end(); ++p)
			{
				if (p->t == orderPoint::r)
				{
					load++;
				}
				if (p->t == orderPoint::d)
				{
					load--;
				}
				os << p->p->x() << " ";
				os << p->p->y() << " ";

				if (p != path->begin())
					os << (p - 1)->time + point::dist(p->p, (p - 1)->p) << " ";
				else
					os << point::dist(p->p, &start) << " ";
				os << p->time << " ";

				os << load << std::endl;
			}
	    }
    };
};

#endif //ALGORITHM_ORDER_H
