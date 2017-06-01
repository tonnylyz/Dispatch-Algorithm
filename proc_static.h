#pragma once
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

static void orderWrap(std::vector<order::wrap> &result) {
	auto t1 = 0, t2 = 0;
    for (auto &o : *orders) {
        if (result.size() < dispatchers->size()) {
            result.push_back(order::wrap(&o));
            continue;
        }

		if (o.index % 300 == 0)
			std::cout << "[" << o.index << "/" << orders->size() << "]" << std::endl;

		// Quick Response: try add to the end (for low demand)
		double mrt = 0;
		order::wrap *mrtw = nullptr;
		for (auto &w : result)
		{
			auto responseTime = o.time - w.deliverPath.back().time - o.from->distant(*w.deliverPath.back().p);
			if (abs(responseTime) < 10)
			{
				responseTime = abs(responseTime);
			}
			if (responseTime > 0 && (mrtw == nullptr || responseTime < mrt))
			{
				mrt = responseTime;
				mrtw = &w;
			}
		}
		if (mrtw != nullptr)
		{
			mrtw->pushBack(&o);
			t1++;
			continue;
		}

		// Insert Try: for add into the path (for high demand)
		std::vector<std::thread> workers;

		double mic;
		order::wrap *micw = nullptr;
		for (auto &w : result)
		{
			workers.push_back(std::thread([&]()
			{
				auto insertCost = w.evaluateInsert(&o);
				if (insertCost == -1)
				{
					return;
				}
				if (micw == nullptr || insertCost < mic)
				{
					mutex.lock();
					mic = insertCost;
					micw = &w;
					mutex.unlock();
				}
			}));

		}

		std::for_each(workers.begin(), workers.end(), [](std::thread &t)
		{
			t.join();
		});
		micw->insert(&o);
		t2++;
    }
	std::cout << "=====================================" << std::endl;
	std::cout << "QR\tIT" << std::endl;
	std::cout << t1 << "\t" << t2 << std::endl;
	std::cout << "=====================================" << std::endl;
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

inline void process() {
    out = std::ofstream("out.txt", std::ios::out);
    if (!out.is_open()) {
        std::cerr << "Unable to open file `out.txt`." << std::endl;
        return;
    }
	std::cout << "Start processing" << std::endl;
    auto wrap = std::vector<order::wrap>();
    orderWrap(wrap);
    double max = 0;
    size_t orderCount = 0;
    for (auto &w : wrap) {
        std::cout << w << std::endl;
        for (auto &p : w.deliverPath) {
            if (p.t == order::orderPoint::d) {
                out << p.o->index << "\t" << p.time - p.o->time << std::endl;
                if (p.time - p.o->time > max)
                    max = p.time - p.o->time;
            }
        }
        orderCount += w.orderList.size();
    }

	std::cout << "=====================================" << std::endl;
    std::cout << "Final result: " << max << std::endl;
}
