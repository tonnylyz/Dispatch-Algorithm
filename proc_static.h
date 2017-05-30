#include <map>
#include "munkres.h"
#include <thread>
#include <mutex>

// Debug utilities
#define VERBOSE 2
#define DEBUG(_) if (VERBOSE > (_))

// Global order pointer
#define GOP(_) (&(orders->at((_).index - 1)))

// Global variable
extern std::vector<restaurant> *restaurants;
extern std::vector<dispatcher> *dispatchers;
extern std::vector<district> *districts;
extern std::vector<order> *orders;

// Global output stream
std::ofstream out;

// Global counter
static size_t deliveredOrderCount = 0;
static double maxCost = 0;
static std::queue<dispatcher *> initDispatcher = std::queue<dispatcher *>();

std::mutex mutex;
static double __mst = 0;
static order::wrap *__mw = nullptr;


void __eval(order *o, order::wrap *w) {
	double r = w->evaluateOrder(o);
	if (r < 0) return;
	mutex.lock();
	if (__mw == nullptr) {
		__mst = r;
		__mw = w;
	} else if (r < __mst) {
		__mst = r;
		__mw = w;
	}
	mutex.unlock();
}

static void orderWrap(std::vector<order::wrap> &result) {
    int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    for (auto &o : *orders) {
        if (result.size() < dispatchers->size()) {
            result.push_back(order::wrap(&o));
            continue;
        }
        __mst = 0;
        __mw = nullptr;
		auto threads = new std::thread[result.size()];
		for (size_t i = 0; i < result.size(); i++) {
			threads[i] = std::thread(__eval, &o, &result[i]);
		}
		for (size_t i = 0; i < result.size(); i++) {
			threads[i].join();
		}
        if (__mw == nullptr) {
            result.push_back(order::wrap(&o));
            c0++;
        } else {
			__mw->addOrder(&o);
            c1++;
        }
		if (o.index % 100 == 0)
			std::cout << o.index << std::endl;
    }
    std::cout << c0 << "\t" << c1 << "\t" << c2 << "\t" << c3 << std::endl;
    std::cout << result.size() << std::endl;
    assert(result.size() == dispatchers->size());

    for (auto &w : result) {
        if (initDispatcher.empty()) break;
        auto d = initDispatcher.front();
        d->moveTo(*w.deliverPath.front().p);
        d->status = dispatcher::load;
        d->target = w.deliverPath.front().p;
        d->list = std::vector<order *>(w.orderList);
        d->path = std::queue<order::orderPoint>();
        for (auto p : w.deliverPath)
            d->path.push(p);

        initDispatcher.pop();
    }
}

void process() {
    out = std::ofstream("out.txt", std::ios::out);
    if (!out.is_open()) {
        std::cerr << "Unable to open file `out.txt`." << std::endl;
        return;
    }

    auto wrap = std::vector<order::wrap>();
    orderWrap(wrap);
    double max = 0;
    size_t orderCount = 0;
    for (auto &w : wrap) {
        //std::cout << w << std::endl;
        for (auto &p : w.deliverPath) {
            if (p.t == order::orderPoint::d) {
                out << p.o->index << "\t" << p.time - p.o->time << std::endl;
                if (p.time - p.o->time > max)
                    max = p.time - p.o->time;
            }
        }
        orderCount += w.orderList.size();
    }
    std::cout << max << "\t" << orderCount << std::endl;
}
