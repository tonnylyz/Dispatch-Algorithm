#include "main.h"

void process() {
    // TODO: Deal with the static problem

	double max_time = orders[orderNum - 1].time;

	std::cout << "Print overall infomation:" << std::endl;
	std::cout << "Restaurant number: " << restaurantNum << std::endl;
	std::cout << "District number: " << districtNum << std::endl;
	std::cout << "Dispatcher number: " << dispatcherNum << std::endl;
	std::cout << "Order number: " << orderNum << std::endl;
	std::cout << "Simulation time ticks: " << max_time << std::endl;
	std::cout << "=====================================" << std::endl;

	int sliceSize = 1000;
	std::map<int, int> slices = std::map<int, int>();

	int timeMin = 0;

	for (auto &o : orders) {
		int time = (int)o.time;
		time /= sliceSize;
		slices[time]++;
		timeMin += point::dist(o.from, o.to);
	}

	timeMin /= orderNum;
	std::cout << "Average estimated minimal time: " << timeMin << std::endl;
    std::cout << "=====================================" << std::endl;
	for (auto &s : slices) {
		std::cout << "Slice #" << s.first << " : " << s.second << std::endl;
	}

}
