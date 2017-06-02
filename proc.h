#pragma once
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include "munkres.h"
#include "order.h"
#include "dispatcher.h"

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

enum __mode { Static, Dynamic };
__mode mode;

// Global output stream
std::ofstream out;

// Global counter
static size_t deliveredOrderCount = 0;
static double maxCost = 0;

std::mutex mutex;

static void orderWrap(std::vector<order::wrap> &result) {
	auto t1 = 0, t2 = 0, t3 = 0;
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
				auto insertCost = w.evaluateInsert(&o, mode == Dynamic);
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
		if (micw != nullptr)
		{
			micw->insert(&o);
			t2++;
			continue;
		}

		// Second chance
		for (auto &w : result)
		{
			auto responseTime = o.time - w.deliverPath.back().time - o.from->distant(*w.deliverPath.back().p);
			if (mrtw == nullptr || responseTime < mrt)
			{
				mrt = responseTime;
				mrtw = &w;
			}
		}
		mrtw->pushBack(&o);
		t3++;
    }
	std::cout << "=====================================" << std::endl;
	std::cout << "QR\tIT\tSC" << std::endl;
	std::cout << t1 << "\t" << t2 << "\t" << t3 << std::endl;
	std::cout << "=====================================" << std::endl;

	if (mode == Dynamic)
	{
		auto targets = std::vector<restaurant *>();
		for (auto &r : *restaurants)
			targets.push_back(&r);
		random_shuffle(targets.begin(), targets.end());
		targets.resize(dispatchers->size());
		for (size_t i = 0; i < targets.size(); i++)
		{
			dispatchers->at(i).moveTo(static_cast<point>(*targets[i]));
		}
		Matrix<double> matrix(result.size(), dispatchers->size());
		for (size_t i = 0; i < result.size(); i++) {
			for (size_t j = 0; j < dispatchers->size(); j++) {
				matrix(i, j) = result[i].deliverPath.front().p->distant(static_cast<point>(dispatchers->at(j)));
			}
		}
		Munkres<double> m;
		m.solve(matrix);
		for (size_t i = 0; i < result.size(); i++) {
			for (size_t j = 0; j < dispatchers->size(); j++) {
				if (matrix(i, j) == 0) {
					result[i].addOffset(result[i].deliverPath.front().p->distant(static_cast<point>(dispatchers->at(j))));
					dispatchers->at(j).path = &result[i].deliverPath;
				}
			}
		}
	}
	else if (mode == Static)
	{
		for (size_t i = 0; i < result.size(); i++)
		{
			dispatchers->at(i).moveTo(*result[i].deliverPath.front().p);
			dispatchers->at(i).path = &result[i].deliverPath;
		}
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

	for (auto &d : *dispatchers)
	{
		out << d.index << " ";
		order::wrap::printPath(out, d.path, static_cast<point>(d));
		for (auto &p : *d.path) {
			if (p.t == order::orderPoint::d) {
				if (p.time - p.o->time > max)
					max = p.time - p.o->time;
			}
		}
	}
	for (auto &w : wrap)
	{
		std::cout << w;
	}
	assert(dispatchers->size() == wrap.size());
	std::cout << "=====================================" << std::endl;
    std::cout << "Final result: " << max << std::endl;
	out << max << std::endl;
}
