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
    double maxWaitTime = -1;
	for (auto &o : *orders) {
		averageWaitTime += o.timeEstimated - o.time;
	}
	averageWaitTime /= orders->size();


	std::cout << "Average estimated wait time: " << averageWaitTime << std::endl;
    std::cout << "=====================================" << std::endl;

    int timeSliceSize = (int)averageWaitTime * 3; // Take average estimated wait time as our time slice size

    bool finished = false;

    std::vector<order>::iterator orderIterator = orders->begin();
	// Strategy of FCFS

	for (unsigned int i = 0; i < dispatchers->size(); i++) {
        dispatchers->at(i).moveTo(*(orders->at(i).from));
	}

    for (int ts = 0; !finished; ts++) {

        std::cout << "Start to schedule for slice " << ts << std::endl;

        auto newOrder = std::vector<order>();

        for (; (*orderIterator).time < (ts + 1) * timeSliceSize;
                orderIterator++) {
            newOrder.push_back(*orderIterator);
			if (orderIterator->index == orders->size()) {
				break;
			}
        }


		auto idleDispatcher = dispatcher::get(dispatcher::idle);
		auto loadDispatcher = dispatcher::get(dispatcher::load);
		auto deliverDispatcher = dispatcher::get(dispatcher::deliver);

        for (auto &o : newOrder) {
            std::cout << "no#" << o.index << std::endl;
        }

        std::cout << "new order : " << newOrder.size()
                  << " idle : " << idleDispatcher.size()
                  << " load : " << loadDispatcher.size()
                  << " deli : " << deliverDispatcher.size() << std::endl;
        if (newOrder.size() == 0) {

        } else if (idleDispatcher.size() >= newOrder.size()) {

			Matrix<double> matrix(newOrder.size(), idleDispatcher.size());

            for (size_t i = 0; i < newOrder.size(); i++) {
                for (size_t j = 0; j < idleDispatcher.size(); j++) {
                    matrix(i, j) = newOrder[i].from->distant(*idleDispatcher[j]);
                }
            }

            //std::cout << matrix << std::endl;

            Munkres<double> m;
            m.solve(matrix);

            //std::cout << matrix << std::endl;

            for (size_t i = 0; i < newOrder.size(); i++) {
                for (size_t j = 0; j < idleDispatcher.size(); j++) {
                    if (matrix(i, j) == 0) {
                        std::cout << "Dispatcher #" << idleDispatcher[j]->index
                                  << " take order #" << newOrder[i].index
                                  << " estimate to move " << point::dist(*idleDispatcher[j], *(newOrder[i].from))
                                  << std::endl;
                        idleDispatcher[j]->setStatus(dispatcher::load);
                        idleDispatcher[j]->target = newOrder[i].from;
                        idleDispatcher[j]->timeStart = ts * timeSliceSize;
                        idleDispatcher[j]->toLoad = &(*orders)[newOrder[i].index - 1];
                    }
                }
            }

		} else {
            std::cout << "System overload!" << std::endl;
        }

		for (int tick = ts * timeSliceSize; tick < (ts + 1) * timeSliceSize; tick++) {
			// Update dispatchers
            for (auto &d : *dispatchers) {

                if (d.target != nullptr) {
                    double dist = point::dist(d, *(d.target));

                    order *o = d.toDeliver != nullptr ? d.toDeliver :
                               d.toLoad    != nullptr ? d.toLoad : nullptr;

                    if (dist < 1 && o != nullptr) {
                        if (restaurant *r = dynamic_cast<restaurant *>(d.target)) {
                            if (o->time <= tick) {
                                d.toDeliver = d.toLoad;
                                d.toLoad = nullptr;
                                d.setStatus(dispatcher::deliver);
                                d.target = o->to;
                            }
                        } else if (district *r = dynamic_cast<district *>(d.target)) {
                            double cost = (tick + dist) - o->time;
                            std::cout << "Order #" << o->index
                                      << " done @" << tick + dist
                                      << " est @" << o->timeEstimated
                                      << " cost " << cost << std::endl;
                            d.toDeliver = nullptr;
                            d.setStatus(dispatcher::idle);
                            if (cost > maxWaitTime) {
                                maxWaitTime = cost;
                            }
                            if (o->index == orders->size()) {
                                finished = true;
                            }

                            d.target = o->from;
                        }
                    } else {
                        double xd = d.target->x() - d.x();
                        double yd = d.target->y() - d.y();
                        xd /= dist;
                        yd /= dist;
/*                        printf("Dispatcher #%d (%.3lf) move from (%.3lf, %.3lf) to (%.3lf, %.3lf)\n", d.index, dist,
                               d.x(), d.y(), d.x() + xd, d.y() + yd
                        );*/

                        d.moveTo(point(d.x() + xd, d.y() + yd));
                    }
                }
            }
		}


        std::cout << "=====================================" << std::endl;
    }
    std::cout << "Max waiting time: " << maxWaitTime << std::endl;
}
