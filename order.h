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

#define MAX(a,b) ((a) > (b) ? (a) : (b))

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
             r
        	,d
        	,m
        };
        point * p; // point pointer
        order * o; // order pointer
        orderPointType t; // type
		double timeEnter;
        double timeLeave;
        orderPoint(point *p, order *o, orderPointType t) : p(p), o(o), t(t), timeEnter(0), timeLeave(0) {};
    };

    class wrap {
    private:
        std::map<order *, std::vector<orderPoint>> _pathAlt;

        void _calcTime(std::vector<orderPoint> &path, bool dynamic) const
        {
	        auto time = startTime;
            for (auto iter = path.begin(); iter != path.end(); ++iter) {
	            switch (iter->t)
	            {
				case orderPoint::r:
					if (dynamic)
					{
						iter->timeEnter = time;
						if (iter != path.end() - 1)
						{
							switch ((iter + 1)->t)
							{
							case orderPoint::r: 
								assert(MAX(time, iter->o->time) >= (iter + 1)->o->time);
								iter->timeLeave = MAX(MAX(time, iter->o->time), (iter + 1)->o->time);
								break;
							case orderPoint::d: 
								iter->timeLeave = MAX(time, iter->o->time);
								break;
							case orderPoint::m:
								iter->timeLeave = MAX(time, iter->o->time);
								break;
							}
						}
						else
						{
							iter->timeLeave = time;
						}
					}
					else
					{
						iter->timeEnter = time;
						iter->timeLeave = MAX(time, iter->o->time);
					}
					break;
				case orderPoint::d:
					if (dynamic)
					{
						iter->timeEnter = time;
						if (iter != path.end() - 1)
						{
							switch ((iter + 1)->t)
							{
							case orderPoint::r:
								iter->timeLeave = MAX(time, (iter + 1)->o->time);
								break;
							case orderPoint::d:
								iter->timeLeave = time;
								break;
							case orderPoint::m:
								iter->timeLeave = MAX(time, (iter + 1)->o->time);
								break;
							}
						}
						else
						{
							iter->timeLeave = time;
						}
					}
					else
					{
						iter->timeEnter = time;
						iter->timeLeave = time;
					}
					break;
				case orderPoint::m:
					if (dynamic)
					{
						iter->timeEnter = time;
						iter->timeLeave = time;
					}
					break;
	            }

				if (iter == path.end() - 1)
					break;
				if (dynamic)
				{
					time = MAX(iter->timeEnter, iter->timeLeave);
					time += point::dist(iter->p, (iter + 1)->p);
				}
				else
				{
					time = MAX(iter->timeEnter, iter->timeLeave);
					time += point::dist(iter->p, (iter + 1)->p);
				}
            }
        }

	    static double _maxTime(std::vector<orderPoint> &path, size_t from = 0) {
            double max = 0;
            for (auto iter = path.begin() + from; iter != path.end(); ++iter) {
	            auto op = *iter;
                if (op.t == orderPoint::d) {
                    max += op.timeLeave - op.o->time;
                }
            }
            return max;
        }

        static double _full(std::vector<orderPoint> &path) {
            unsigned int maxLoaded = 0, loaded = 0;
            for (auto iter = path.begin(); iter != path.end(); ++iter) {
                if (iter->t == orderPoint::r) {
                    loaded++;
                } else if (iter->t == orderPoint::d) {
                    loaded--;
                }
                if (loaded > maxLoaded) {
                    maxLoaded = loaded;
                }
            }
            return maxLoaded;
        }

		size_t _currentIndex(double time)
        {
			for (size_t i = 0; i < deliverPath.size(); i++)
			{
				if (deliverPath[i].timeLeave > time)
				{
					return i;
				}
			}
			return -1;
        }

		point * _currentPoint(double time)
        {
			auto a = deliverPath.begin() + _currentIndex(time) - 1;
			auto b = deliverPath.begin() + _currentIndex(time);
			double offset = time - a->timeLeave;
			double distance = point::dist(a->p, b->p);
			double ratio = offset / distance;
			double xd = b->p->x() - a->p->x();
			double yd = b->p->y() - a->p->y();
			xd *= ratio;
			yd *= ratio;
			//TODO: Memory leak here!
			point *p = new point(a->p->x() + xd, a->p->y() + yd);
			return p;
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
            _calcTime(deliverPath, false);

            _pathAlt = std::map<order *, std::vector<orderPoint>>();
        }

		double evaluateInsert(order *o, bool dynamic)
        {
			double minTime = 0;
			std::vector<orderPoint> minPath;
			// Here will not insert to end

			for (auto from = dynamic ? _currentIndex(o->time) : deliverPath.size() - 1;; dynamic ? from++ : from--) {
				if (from == -1)
					return -1;
				if (dynamic && from >= deliverPath.size())
					break;
				for (auto to = from + 1; to <= deliverPath.size() + 1; to++) {
					auto path = std::vector<orderPoint>(deliverPath);


					path.emplace(path.begin() + from, o->from, o, orderPoint::r);
					path.emplace(path.begin() + to, o->to, o, orderPoint::d);
					if (dynamic && from == _currentIndex(o->time))
					{
						path.emplace(path.begin() + from, _currentPoint(o->time), (path.begin() + from + 1)->o, orderPoint::m);
					}

					if (_full(path) > containerSize)
						continue;
					_calcTime(path, dynamic);
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
				if (!dynamic && deliverPath[from].t == orderPoint::r
					&& o->time < deliverPath[from].timeEnter) {
					break;
				}
				if (from == 0) {
					break;
				}
			}
			_pathAlt[o] = std::vector<orderPoint>(minPath);
			return minTime;
        }

		void pushBack(order *o, bool dynamic)
	    {
			orderList.push_back(o);
			deliverPath.emplace_back(o->from, o, orderPoint::r);
			deliverPath.emplace_back(o->to, o, orderPoint::d);
			_calcTime(deliverPath, dynamic);

			_pathAlt = std::map<order *, std::vector<orderPoint>>();
	    }

        void insert(order *o) {
            if (_pathAlt.count(o) == 0) {
                std::cerr << "A route without evaluation added!" << std::endl;
                return;
            }
			assert(o->time >= startTime);
			            orderList.push_back(o);
            deliverPath = _pathAlt[o];
            _pathAlt = std::map<order *, std::vector<orderPoint>>();
        }

		void setStartTime(double offset, bool dynamic)
	    {
			startTime = offset;
			_calcTime(deliverPath, dynamic);
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
                } else
                {
					os << "m" << *p.p << " ";
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

				os << p->timeEnter << " ";
				os << p->timeLeave << " ";
				assert(load <= containerSize);

				os << load << std::endl;
			}
	    }
    };
};

#endif //ALGORITHM_ORDER_H
