#include <iostream>
#include <fstream>
#include <limits>
#include <vector>

using namespace std;


struct Flow {
	int entryVillage;
	int finishVillage;
	int time;
	int price;
};

struct PathStruct {
	int entryVillage;
	int price;
	int crossingTime;
};

struct BestPath {
	int finishVillage;
	int wholePathTime;
	int totalPrice;
};

struct Ans {
	int totalTime;
	vector<int> villages;
};

vector<Flow> pathsToVillage(Flow *flow, int village, int boatmen) {
	vector<Flow> paths;
	for (int i = 0; i < boatmen; ++i) {
		if (flow[i].finishVillage == village)
			paths.push_back(flow[i]);
	}

	return paths;
}


Ans shortestRouteFlow(Flow *flow, int villages, int boatmen, int coins) {
	int inf = numeric_limits<int>::max();
	
	PathStruct** pathDP = new PathStruct*[villages];
	for (int i = 0; i < villages; ++i)
		pathDP[i] = new PathStruct[coins + 1];

	int** timeDP = new int*[villages];
	for (int i = 0; i < villages; ++i)
		timeDP[i] = new int[coins + 1];

	for (int i = 0; i < villages; ++i) {
		for (int j = 0; j < coins + 1; ++j) {
			if (i==0) {				
				pathDP[i][j].entryVillage = 0;
				timeDP[i][j] = 0;
			}
			else {				
				pathDP[i][j].entryVillage = -1;
				timeDP[i][j] = inf;
			}
		}
	}


	for (int i = 0; i < villages; ++i) {
		vector<Flow> paths = pathsToVillage(flow, i, boatmen);
		for (int j = 0; j < coins + 1; ++j) {
			if (paths.empty())
				break;
			for (auto k : paths) {
				if (j - k.price >= 0) {
					if ((timeDP[k.entryVillage][j - k.price] != inf) && ((timeDP[k.entryVillage][j - k.price] + k.time) < timeDP[k.finishVillage][j])) {
						timeDP[k.finishVillage][j] = timeDP[k.entryVillage][j - k.price] + k.time;
						pathDP[k.finishVillage][j].entryVillage = k.entryVillage;
						pathDP[k.finishVillage][j].crossingTime = k.time;
						pathDP[k.finishVillage][j].price = k.price;
					}
				}				
			}
		}
		paths.clear();
	}


	vector<BestPath> bestPath;
	BestPath lastVil;
	lastVil.finishVillage = villages - 1;
	lastVil.wholePathTime = pathDP[villages - 1][coins].crossingTime;
	lastVil.totalPrice = pathDP[villages - 1][coins].price;

	bestPath.push_back(lastVil);


	PathStruct tmp = pathDP[villages - 1][coins];
	BestPath previousLastVil;
	previousLastVil.finishVillage = tmp.entryVillage;
	previousLastVil.wholePathTime = pathDP[tmp.entryVillage][coins - tmp.price].crossingTime;
	previousLastVil.totalPrice = pathDP[tmp.entryVillage][coins - tmp.price].price;

	bestPath.push_back(previousLastVil);


	int tmp2 = coins;
	while (tmp.entryVillage) {
		tmp2 -= tmp.price;
		int village = pathDP[tmp.entryVillage][tmp2].entryVillage;
		int crossingTime = pathDP[tmp.entryVillage][tmp2].crossingTime;
		int price = pathDP[tmp.entryVillage][tmp2].price;
		BestPath hlpr;
		hlpr.finishVillage = village;
		hlpr.wholePathTime = crossingTime;
		hlpr.totalPrice = price;
		bestPath.push_back(hlpr);
		tmp = pathDP[tmp.entryVillage][coins];
	}
	Ans returnVal;
	returnVal.totalTime = timeDP[villages - 1][coins];
	for (vector<BestPath>::size_type i = 0; i != bestPath.size(); ++i) {
		for (int j = 0; j < boatmen; ++j) {
			if ((flow[j].entryVillage == bestPath[i].finishVillage) && (flow[j].finishVillage == bestPath[i - 1].finishVillage)
				&& ((bestPath[i].wholePathTime == flow[j].time) || (bestPath[i - 1].wholePathTime == flow[j].time))
				&& ((bestPath[i].totalPrice == flow[j].price) || (bestPath[i - 1].totalPrice == flow[j].price))) {
				returnVal.villages.push_back(j + 1);
				break;
			}
		}
	}
	 
	delete[] timeDP;
	delete[] pathDP;

	bestPath.clear();

	return returnVal;
}

void flowAlg() {
	ifstream in;
	in.open("in.txt");
	if (!in.good())
		return;
	int villages = 0, boatmen = 0, coins = 0;
	in >> villages >> boatmen >> coins;

	Flow* flow = new Flow[boatmen];
	for (int i = 0; i < boatmen; ++i) {
		in >> flow[i].entryVillage >> flow[i].finishVillage >> flow[i].time >> flow[i].price;
		--flow[i].entryVillage;
		--flow[i].finishVillage;
	}
	in.close();

	Ans outAns = shortestRouteFlow(flow, villages, boatmen, coins);

	ofstream out;
	out.open("out.txt");
	if (!out.good())
		return;
	if (outAns.villages.empty())
		out << "NIE";
	else {
		out << outAns.totalTime << endl;
		for (int i = outAns.villages.size() - 1; i != -1; --i)
			out << outAns.villages[i] << " ";
	}
	out.close();

	outAns.villages.clear();
	delete[] flow;
	return;
}


int main() {
	flowAlg();
	return 0;
}