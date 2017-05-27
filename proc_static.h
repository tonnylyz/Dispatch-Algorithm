#include <map>
#include <functional>
#include "munkres.h"

// Debug utilities
#define VERBOSE 5
#define DEBUG(_) if (VERBOSE > (_))

// Parameter
#define EXPECT_MAX_SERVICE_TIME_RATIO 2
#define DISPATCHER_USE_PER_SLICE_RATIO 0.8

// Global order pointer
#define GOP(_) (&(orders->at((_).index - 1)))

// Global variable
extern std::vector<restaurant> *restaurants;
extern std::vector<dispatcher> *dispatchers;
extern std::vector<district> *districts;
extern std::vector<order> *orders;
extern unsigned int containerSize;
int timeSliceSize;

static void orderNew(std::vector<order>::iterator &iter, double maxTime, std::vector<order *> &result) {
    for (; iter != orders->end() && (*iter).time < maxTime; iter++) {
        result.push_back(GOP(*iter));
    }
}

static size_t dispatcherInit = 0;
static std::queue<dispatcher *> dispatcherToSchedule = std::queue<dispatcher *>();

static void orderWrap(const std::vector<order *> &newOrder, size_t idc, std::vector<order::wrap> &result) {

    for (auto &o : newOrder) {
        if (result.empty()) {
            result.push_back(order::wrap(o));
            continue;
        }

        double minServiceTime = 0;
        order::wrap *minWrap = nullptr;

        bool done = false;
        for (auto &w : result) {

            if (w.list.size() < containerSize) {
                double r = w.evaluateOrder(o);
                if (minWrap == nullptr) {
                    minServiceTime = r;
                    minWrap = &w;
                } else if (r < minServiceTime) {
                    minServiceTime = r;
                    minWrap = &w;
                }
                done = true;
            }
        }
        if (!done) {
            result.push_back(order::wrap(o));
            continue;
        }

        if (minServiceTime < timeSliceSize * EXPECT_MAX_SERVICE_TIME_RATIO) {
            minWrap->addOrder(o);
        } else {
            if (result.size() < idc) {
                result.push_back(order::wrap(o));
                continue;
            } else {
                minWrap->addOrder(o);
            }
        }
    }

    int tss = dispatcherToSchedule.size() < result.size() ? dispatcherToSchedule.size() : result.size();
    for (int i = 0; i < tss; i++) {
        auto d = dispatcherToSchedule.front();
        dispatcherToSchedule.pop();
        if (dispatcherInit < 50) {
            d->moveTo(*result[i].path.front());
            dispatcherInit++;
        } else {
            d->target = result[i].path.front();
        }
    }

}


static void matching(std::vector<order::wrap> &w, std::vector<dispatcher *> &d) {
    Matrix<double> matrix(w.size(), d.size());

    for (size_t i = 0; i < w.size(); i++) {
        for (size_t j = 0; j < d.size(); j++) {
            matrix(i, j) = w[i].path.front()->distant(*d[j]);
        }
    }

    //std::cout << matrix << std::endl;

    Munkres<double> m;
    m.solve(matrix);

    //std::cout << matrix << std::endl;

    for (size_t i = 0; i < w.size(); i++) {
        for (size_t j = 0; j < d.size(); j++) {
            if (matrix(i, j) == 0) {
                d[j]->status = dispatcher::load;
                d[j]->target = w[i].path.front();
                d[j]->list = std::vector<order *>(w[i].list);
                d[j]->path = std::queue<point *>();
                for (auto p : w[i].path)
                    d[j]->path.push(p);
            }
        }
    }
}

static bool updateDispatcher(size_t tick) {

    for (auto &d : *dispatchers) {

        std::cout << d;

        if (d.target == nullptr) continue;

        double xd = d.target->x() - d.x();
        double yd = d.target->y() - d.y();
        xd /= point::dist(d, *(d.target));
        yd /= point::dist(d, *(d.target));
        d.moveTo(point(d.x() + xd, d.y() + yd));

        if (d.path.empty()) continue;
        while (1) {
            if (d.path.empty()) break;
            double dist = point::dist(*d.path.front(), d);
            if (dist > 1) {
                break;
            } else if (dist <= 1) {
                for (auto &o : d.list) {
                    if (o->finished) continue;
                    if (d.path.front() == o->from) {
                        if (d.status == dispatcher::idle) {
                            d.status = dispatcher::deliver;
                        }
                    } else if (d.path.front() == o->to) {
                        double cost = (tick + dist) - o->time;
                        std::cout << "Order #" << o->index
                                  << " done @" << tick + dist
                                  << " est @" << o->timeEstimated
                                  << " cost " << cost << std::endl;
                        o->finished = true;
                        if (o->index == orders->size()) {
                            return true;
                        }
                    }
                }
                d.path.pop();
            } else {
                assert(0);
            }

        }
        if (d.path.empty()) {
            d.target = nullptr;
            dispatcherToSchedule.push(&d);
            continue;
        }
        d.target = d.path.front();
        continue;

    }
    return false;
}


void process() {
    // TODO: Deal with the static problem

    double averageWaitTime = 0;
    double maxWaitTime = -1;
    for (auto &o : *orders) {
        averageWaitTime += o.timeEstimated - o.time;
    }
    averageWaitTime /= orders->size();

    DEBUG(1) {
        std::cout << "Average estimated wait time: " << averageWaitTime << std::endl;
        std::cout << "=====================================" << std::endl;
    }

    // Take average estimated wait time as our time slice size
    timeSliceSize = (int) averageWaitTime;

    bool finished = false;

    std::vector<order>::iterator orderIterator = orders->begin();


    for (size_t ts = 0; !finished; ts++) {

        DEBUG(2)std::cout << "Start to schedule for slice " << ts << std::endl;

        auto idleDispatcher = dispatcher::get(dispatcher::idle);
        auto loadDispatcher = dispatcher::get(dispatcher::load);
        auto deliverDispatcher = dispatcher::get(dispatcher::deliver);

        auto newOrder = std::vector<order *>();
        orderNew(orderIterator, (ts + 1) * timeSliceSize, newOrder);

        auto newOrderWrap = std::vector<order::wrap>();
        orderWrap(newOrder, (size_t) (idleDispatcher.size() * DISPATCHER_USE_PER_SLICE_RATIO), newOrderWrap);

        DEBUG(2)for (const auto &w : newOrderWrap) {
                std::cout << w;
            }

        DEBUG(2)std::cout << "new order wrap : " << newOrderWrap.size()
                          << " order : " << newOrder.size()
                          << " idle : " << idleDispatcher.size()
                          << " load : " << loadDispatcher.size()
                          << " deli : " << deliverDispatcher.size() << std::endl;

        if (idleDispatcher.size() != 0 && newOrderWrap.size() != 0) {
            matching(newOrderWrap, idleDispatcher);
        }

        for (size_t tick = ts * timeSliceSize;
             tick < (ts + 1) * timeSliceSize;
             tick++) {
            finished = updateDispatcher(tick);
        }

        DEBUG(2)std::cout << "=====================================" << std::endl;
    }
    std::cout << "Max waiting time: " << maxWaitTime << std::endl;
}
