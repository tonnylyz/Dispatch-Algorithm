#include "order.h"
#include "dispatcher.h"

std::vector<order> *orders;

order::order(unsigned int index, restaurant *r, district *d, double time) :
        index(index), time(time), timeEstimated(time + point::dist(static_cast<point>(*r), static_cast<point>(*d))),
        from(r), to(d) {}

void order::wrap::_calcTime(std::vector<order::orderPoint> &path, bool dynamic) const {
    auto time = startTime;
    for (auto iter = path.begin(); iter != path.end(); ++iter) {
        switch (iter->t) {
            case orderPoint::r:
                if (dynamic) {
                    iter->timeEnter = time;
                    if (iter != path.end() - 1) {
                        switch ((iter + 1)->t) {
                            case orderPoint::r:
                                iter->timeLeave = MAX(MAX(time, iter->o->time), (iter + 1)->o->time);
                                break;
                            case orderPoint::d:
                            case orderPoint::m:
                                iter->timeLeave = MAX(time, iter->o->time);
                                break;
                        }
                    }
                } else {
                    iter->timeEnter = time;
                    iter->timeLeave = MAX(time, iter->o->time);
                }
                break;
            case orderPoint::d:
                if (dynamic) {
                    iter->timeEnter = time;
                    if (iter != path.end() - 1) {
                        switch ((iter + 1)->t) {
                            case orderPoint::r:
                            case orderPoint::m:
                                iter->timeLeave = MAX(time, (iter + 1)->o->time);
                                break;
                            case orderPoint::d:
                                iter->timeLeave = time;
                                break;
                        }
                    } else {
                        iter->timeLeave = time;
                    }
                } else {
                    iter->timeEnter = time;
                    iter->timeLeave = time;
                }
                break;
            case orderPoint::m:
                if (dynamic) {
                    iter->timeEnter = time;
                    iter->timeLeave = time;
                }
                break;
        }

        if (iter == path.end() - 1)
            break;
        time = MAX(iter->timeEnter, iter->timeLeave);
        time += point::dist(iter->p, (iter + 1)->p);
    }
}

double order::wrap::_maxTime(std::vector<order::orderPoint> &path, size_t from) {
    double max = 0;
    for (auto iter = path.begin() + from; iter != path.end(); ++iter) {
        auto op = *iter;
        if (op.t == orderPoint::d) {
            max += op.timeLeave - op.o->time;
        }
    }
    return max;
}

double order::wrap::_full(std::vector<order::orderPoint> &path) {
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

size_t order::wrap::_currentIndex(double time) {
    for (size_t i = 0; i < deliverPath.size(); i++) {
        if (deliverPath[i].timeLeave > time) {
            return i;
        }
    }
    return deliverPath.size();
}

point *order::wrap::_currentPoint(double time) {
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

order::wrap::wrap(order *o) {
    orderList = std::vector<order *>();
    orderList.push_back(o);

    startTime = o->time;

    deliverPath = std::vector<orderPoint>();

    deliverPath.emplace_back(o->from, o, orderPoint::r);
    deliverPath.emplace_back(o->to, o, orderPoint::d);
    _calcTime(deliverPath, false);

    _pathAlt = std::map<order *, std::vector<orderPoint>>();
}

double order::wrap::evaluateInsert(order *o, bool dynamic) {
    double minTime = 0;
    std::vector<orderPoint> minPath;
    size_t from = _currentIndex(o->time);
    if (from == deliverPath.size())
        return -1;
    while (true) {
        for (auto to = from + 1; to <= deliverPath.size() + 1; to++) {
            auto path = std::vector<orderPoint>(deliverPath);
            path.emplace(path.begin() + from, o->from, o, orderPoint::r);
            path.emplace(path.begin() + to, o->to, o, orderPoint::d);
            if (dynamic && from == _currentIndex(o->time)) {
                path.emplace(path.begin() + from,
                             _currentPoint(o->time),
                             (deliverPath.begin() + from)->o,
                             orderPoint::m);
            }
            if (_full(path) > containerSize) {
                continue;
            }
            _calcTime(path, dynamic);
            auto time = _maxTime(path, from) - _maxTime(deliverPath, from);
            if (minTime == 0 || time < minTime) {
                minTime = time;
                minPath = std::vector<orderPoint>(path);
            }
        }
        from++;
        if (from == deliverPath.size()) {
            break;
        }
    }
    _pathAlt[o] = std::vector<orderPoint>(minPath);
    return minTime;
}

void order::wrap::pushBack(order *o, bool dynamic) {
    orderList.push_back(o);
    deliverPath.emplace_back(o->from, o, orderPoint::r);
    deliverPath.emplace_back(o->to, o, orderPoint::d);
    _calcTime(deliverPath, dynamic);

    _pathAlt = std::map<order *, std::vector<orderPoint>>();
}

void order::wrap::insert(order *o) {
    orderList.push_back(o);
    deliverPath = _pathAlt[o];
    _pathAlt = std::map<order *, std::vector<orderPoint>>();
}

void order::wrap::setStartTime(double offset, bool dynamic) {
    startTime = offset;
    _calcTime(deliverPath, dynamic);
}

void order::wrap::printPath(std::ostream &os, std::vector<order::orderPoint> *path, point start) {
    if (*(path->front().p) != start) {
        os << path->size() + 1 << std::endl;
        os << start.x() << " ";
        os << start.y() << " ";
        os << "0 ";
        os << "0 ";
        os << "0" << std::endl;
    } else {
        os << path->size() << std::endl;
    }

    size_t load = 0;
    for (auto p = path->begin(); p != path->end(); ++p) {
        if (p->t == orderPoint::r) {
            load++;
        }
        if (p->t == orderPoint::d) {
            load--;
        }
        os << p->p->x() << " ";
        os << p->p->y() << " ";

        os << p->timeEnter << " ";
        os << p->timeLeave << " ";

        os << load << std::endl;
    }
}
