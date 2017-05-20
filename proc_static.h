#include <map>
#include "munkres.h"

extern std::vector<restaurant> *restaurants;
extern std::vector<dispatcher> *dispatchers;
extern std::vector<district> *districts;
extern std::vector<order> *orders;
extern unsigned int containerSize;

void process() {
    // TODO: Deal with the static problem

	double averageWaitTime = 0;
	for (auto &o : *orders) {
		averageWaitTime += o.timeEstimated - o.time;
	}
	averageWaitTime /= orders->size();


	std::cout << "Average estimated wait time: " << averageWaitTime << std::endl;
    std::cout << "=====================================" << std::endl;

    int timeSliceSize = (int)averageWaitTime; // Take average estimated wait time as our time slice size

    bool finished = false;

    std::vector<order>::iterator orderIterator = orders->begin();
	// Strategy of FCFS

	for (unsigned int i = 0; i < dispatchers->size(); i++) {

        dispatchers->at(i).moveTo(orders->at(i).from);
	}

    auto leftOrder = std::vector<order>();

    for (int ts = 0; !finished; ts++) {

        std::cout << "Start to schedule for slice " << ts << std::endl;

        auto newOrder = std::vector<order>();

        for (int tick = ts * timeSliceSize; tick < (ts + 1) * timeSliceSize; tick++) {
            while ((*orderIterator).time < tick) {
                newOrder.push_back(*orderIterator);
                orderIterator++;
            }
        }

		auto idleDispatcher = dispatcher::get(dispatcher::idle);
		auto loadDispatcher = dispatcher::get(dispatcher::load);
		auto deliverDispatcher = dispatcher::get(dispatcher::deliver);

        std::cout << "new order : " << newOrder.size()
                  << " idle : " << idleDispatcher.size()
                  << " load : " << loadDispatcher.size()
                  << " deli : " << deliverDispatcher.size() << std::endl;
        if (newOrder.size() == 0) {

        } else if (idleDispatcher.size() >= newOrder.size()) {

			Matrix<double> matrix(newOrder.size(), idleDispatcher.size());

            for (size_t i = 0; i < newOrder.size(); i++) {
                for (size_t j = 0; j < idleDispatcher.size(); j++) {
                    matrix(i, j) = newOrder[i].from.distant(idleDispatcher[j]);
                }
            }

            //std::cout << matrix << std::endl;

            Munkres<double> m;
            m.solve(matrix);

            //std::cout << matrix << std::endl;

            for (size_t i = 0; i < newOrder.size(); i++) {
                for (size_t j = 0; j < idleDispatcher.size(); j++) {
                    if (matrix(i, j) == 0) {
                        std::cout << "Dispatcher #" << idleDispatcher[j].index
                                  << " take order #" << newOrder[i].index
                                  << " estimate to move " << point::dist(idleDispatcher[j], newOrder[i].from)
                                  << std::endl;
                        dispatchers->at(idleDispatcher[j].index - 1).setStatus(dispatcher::load);
                    }
                }
            }
		} else {
            std::cout << "System overload!" << std::endl;
        }

		for (int tick = ts * timeSliceSize; tick < (ts + 1) * timeSliceSize; tick++) {
			// Update orders
			for (auto &o : newOrder) {

			}
		}


        std::cout << "=====================================" << std::endl;
        if (ts == 100)
            finished = true;
    }
}
