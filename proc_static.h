#include <map>
#include "munkres.h"

// Debug utilities
#define VERBOSE 4
#define DEBUG(_) if (VERBOSE > (_))

// Parameter
#define TIME_SLICE_RATIO 3
#define AGGRESSIVE_THRESHOLD 0.8

// Global order pointer
#define GOP(_) (&(orders->at((_).index - 1)))

// Global variable
extern std::vector<restaurant> *restaurants;
extern std::vector<dispatcher> *dispatchers;
extern std::vector<district> *districts;
extern std::vector<order> *orders;

std::ofstream out;

static void orderNew(std::vector<order>::iterator &iter, double maxTime, std::vector<order *> &result) {
    for (; iter != orders->end() && (*iter).time < maxTime; iter++) {
        result.push_back(GOP(*iter));
    }
}

static size_t deliveredOrderCount = 0;

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
            double r = w.evaluateOrder(o);
            if (r < 0) {
                continue;
            }
            if (minWrap == nullptr) {
                minServiceTime = r;
                minWrap = &w;
            } else if (r < minServiceTime) {
                minServiceTime = r;
                minWrap = &w;
            }
            done = true;
        }
        if (!done) {
            result.push_back(order::wrap(o));
            continue;
        }

        if (result.size() > ceil(AGGRESSIVE_THRESHOLD * idc)/*minServiceTime < timeSliceSize * EXPECT_MAX_SERVICE_TIME_RATIO*/) {
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
        if (dispatcherInit < 50) {
            d->moveTo(*result[i].deliverPath.front().p);
            dispatcherInit++;
        }/* else {
            d->target = result[i].deliverPath.front().p;
        }*/
        dispatcherToSchedule.pop();
    }

}


static void matching(std::vector<order::wrap> &w, std::vector<dispatcher *> &d) {
    Matrix<double> matrix(w.size(), d.size());

    for (size_t i = 0; i < w.size(); i++) {
        for (size_t j = 0; j < d.size(); j++) {
            matrix(i, j) = w[i].deliverPath.front().p->distant(*d[j]);
        }
    }

    Munkres<double> m;
    m.solve(matrix);

    for (size_t i = 0; i < w.size(); i++) {
        for (size_t j = 0; j < d.size(); j++) {
            if (matrix(i, j) == 0) {
                d[j]->status = dispatcher::load;
                d[j]->target = w[i].deliverPath.front().p;
                d[j]->list = std::vector<order *>(w[i].orderList);
                d[j]->path = std::queue<order::orderPoint>();
                for (auto p : w[i].deliverPath)
                    d[j]->path.push(p);
            }
        }
    }
}

static bool updateDispatcher(size_t tick, double &cost) {
    bool result = false;
    for (auto &d : *dispatchers) {
		
		if (d.target == nullptr) {
			continue;
		}
        double distance = point::dist(d, *(d.target));
        if (distance >= 1) {
            double xd = d.target->x() - d.x();
            double yd = d.target->y() - d.y();
            xd /= distance;
            yd /= distance;
            d.moveTo(point(d.x() + xd, d.y() + yd));
        } else if (distance < 1) {
            if (d.path.empty()) continue;
            while (true) {
                distance = point::dist(*d.path.front().p, d);
                if (distance >= 1) break;
                if (d.path.front().t == order::orderPoint::r) {
					if (d.path.front().o->time <= tick) {
						d.path.front().o->loaded = true;
						d.status = dispatcher::deliver;
                        out << "[order] o" << d.path.front().o->index
                                  << " taken from " << *(d.path.front().o->from) << std::endl;
					} else {
						break;
					}
                } else if (d.path.front().t == order::orderPoint::d) {
                    d.path.front().o->delivered = true;
                    cost = tick + 1 - d.path.front().o->time;
                    out << "[order] o" << d.path.front().o->index
                              << " done @" << tick + distance
                              << " est @" << d.path.front().o->timeEstimated
                              << " cost " << cost << std::endl;
                    deliveredOrderCount++;
                }
                d.path.pop();
                if (d.path.empty()) {
                    break;
                }
            }
            if (d.path.empty()) {
                d.target = nullptr;
				d.status = dispatcher::idle;
                d.target = d.list.back()->to;
            } else {
                d.target = d.path.front().p;
            }
        }
    }
    return result;
}


void process() {
    double averageWaitTime = 0;
    double maxWaitTime = -1;
    for (auto &o : *orders) {
        averageWaitTime += o.timeEstimated - o.time;
    }
    averageWaitTime /= orders->size();


    out = std::ofstream("out.txt", std::ios::out);
    if (!out.is_open()) {
        std::cerr << "Unable to open file `out.txt`." << std::endl;
        return;
    }
    
    DEBUG(1) {
        out << "Average estimated wait time: " << averageWaitTime << std::endl;
        out << "=====================================" << std::endl;
    }

    // Take average estimated wait time as our time slice size
    int timeSliceSize = (int)averageWaitTime * TIME_SLICE_RATIO;

    auto orderIterator = orders->begin();
    auto leftWrap = std::queue<order::wrap>();
    size_t lastLeft = orders->size();
    for (size_t ts = 0; orders->size() > deliveredOrderCount; ts++) {

        DEBUG(2)out << "Start to schedule for slice " << ts
                          << " | " << orders->size() - deliveredOrderCount << " left" << std::endl;
        auto idleDispatcher = dispatcher::get(dispatcher::idle);
        auto loadDispatcher = dispatcher::get(dispatcher::load);
        auto deliverDispatcher = dispatcher::get(dispatcher::deliver);
        if (ts != 0 && orders->size() - deliveredOrderCount == lastLeft && deliverDispatcher.size() == 0) {
			out << "Unexpected invalid order state:" << std::endl;
            for (auto &o : *orders) {
                if (!o.delivered) {
                    out << "[Error] o" << o.index << "\r";
                }
            }
            return;
        }
        lastLeft = orders->size() - deliveredOrderCount;

        auto newOrder = std::vector<order *>();
        orderNew(orderIterator, (ts + 1) * timeSliceSize, newOrder);

        auto newOrderWrap = std::vector<order::wrap>();
        size_t leftWrapSize = leftWrap.size();
        while (!leftWrap.empty()) {
            newOrderWrap.push_back((order::wrap)leftWrap.front());
            leftWrap.pop();
        }
        orderWrap(newOrder, idleDispatcher.size(), newOrderWrap);

        DEBUG(3)for (const auto &w : newOrderWrap) {
                out << w;
            }
        DEBUG(2)out << "new wrap : " << newOrderWrap.size() - leftWrapSize
                          << " | left wrap : " << leftWrapSize
                          << " | order : " << newOrder.size()
                          << " | idle : " << idleDispatcher.size()
                          << " | load : " << loadDispatcher.size()
                          << " | deli : " << deliverDispatcher.size() << std::endl;

        if (newOrderWrap.size() > idleDispatcher.size()) {
            for (int i = idleDispatcher.size(); i < newOrderWrap.size(); i++) {
                leftWrap.push(newOrderWrap[i]);
            }
            for (int i = idleDispatcher.size(); i < newOrderWrap.size(); i++) {
                newOrderWrap.erase(newOrderWrap.begin() + i);
            }
        }

        if (idleDispatcher.size() != 0 && newOrderWrap.size() != 0) {
            matching(newOrderWrap, idleDispatcher);
        }

        for (size_t tick = ts * timeSliceSize;
             tick < (ts + 1) * timeSliceSize;
             tick++) {
            double cost = -1;
            updateDispatcher(tick, cost);
            if (cost > maxWaitTime) {
                maxWaitTime = cost;
            }
        }

        DEBUG(2)out << "=====================================" << std::endl;
    }
    out << "Max waiting time: " << maxWaitTime << std::endl;
}
