#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

typedef struct {
	int city1;
	int city2;
	int distance;
}PossibleRoad;

int findClosestVertex(int *distance, bool *visitedVertex, int verticesAmount) {
	int minimalDistance = numeric_limits<int>::max() - 10, closestVertex = 0;
	for (int i = 0; i < verticesAmount; ++i) {
		if (visitedVertex[i] == false && distance[i] <= minimalDistance) {
			minimalDistance = distance[i];
			closestVertex = i;
		}
	}
	return closestVertex;
}

int* DijkstraAlg(int *graph, int verticesAmount, int startVertex) {
	int *distance = new int[verticesAmount];
	bool *visitedVertex = new bool[verticesAmount];
	int inf = numeric_limits<int>::max() - 10;
	for (int i = 0; i < verticesAmount; ++i) {
		distance[i] = inf;
		visitedVertex[i] = false;
	}
	distance[startVertex] = 0;
	for (int i = 0; i < verticesAmount - 1; ++i) {
		int currentVertex = findClosestVertex(distance, visitedVertex, verticesAmount);
		visitedVertex[currentVertex] = true;
		for (int j = 0; j < verticesAmount; ++j) {
			if (visitedVertex[j] == false && graph[currentVertex*verticesAmount + j] != 0 && distance[currentVertex] != inf	&& (distance[currentVertex] + graph[currentVertex*verticesAmount + j]) < distance[j])
				distance[j] = distance[currentVertex] + graph[currentVertex*verticesAmount + j];
		}
	}
	delete[] visitedVertex;
	return distance;
}

void checkOptionalRoutes(int *graph, int cityAmount, int optionalRoads, PossibleRoad *possibleRoadsArr) {
	int *oldShortestDistance = DijkstraAlg(graph, cityAmount, 0);
	int shortRoutesCount = 0;
	ofstream out;
	out.open("out.txt");
	if (!out.good())
		return;
	for (int i = 0; i < optionalRoads; ++i) {
		int oldRouteHolder = graph[possibleRoadsArr[i].city1*cityAmount + possibleRoadsArr[i].city2];
		graph[possibleRoadsArr[i].city1*cityAmount + possibleRoadsArr[i].city2] = possibleRoadsArr[i].distance;
		graph[possibleRoadsArr[i].city2*cityAmount + possibleRoadsArr[i].city1] = possibleRoadsArr[i].distance;
		int *newShortestDistance = DijkstraAlg(graph, cityAmount, 0);
		for (int j = 0; j < cityAmount; ++j) {
			if (newShortestDistance[j] < oldShortestDistance[j]) {
				++shortRoutesCount;
				if (shortRoutesCount > 100)
					break;
			}
		}
		
		if (shortRoutesCount > 100)
			out << "100+";
		else
			out << shortRoutesCount << endl;
		shortRoutesCount = 0;
		graph[possibleRoadsArr[i].city1*cityAmount + possibleRoadsArr[i].city2] = oldRouteHolder;
		graph[possibleRoadsArr[i].city2*cityAmount + possibleRoadsArr[i].city1] = oldRouteHolder;
		delete[] newShortestDistance;
	}
	delete[] oldShortestDistance;
}

void answerToMyQuestion() {
	ifstream in;
	in.open("in.txt");
	if (!in.good()) {
		cout << "Pliku niet" << endl;
		return;
	}
	int cityAmount = 0, readyRoads = 0, optionalRoads = 0;
	in >> cityAmount >> readyRoads >> optionalRoads;
	int *graph = new int[cityAmount*cityAmount];
	for (int i = 0; i < cityAmount; ++i) {
		for (int j = 0; j < cityAmount; ++j) {
			graph[i*cityAmount + j] = 0;
		}
	}
	PossibleRoad *possibleRoadsArr = new PossibleRoad[optionalRoads];
	int tmpCity1 = 0, tmpCity2 = 0, tmpDistance = 0;

	for (int i = 0; i < readyRoads; ++i) {
		in >> tmpCity1 >> tmpCity2 >> tmpDistance;
		graph[(tmpCity1 - 1)*cityAmount + (tmpCity2 - 1)] = tmpDistance;
		graph[(tmpCity2 - 1)*cityAmount + (tmpCity1 - 1)] = tmpDistance;
	}
	for (int i = 0; i < cityAmount; ++i) {
		for (int j = 0; j < cityAmount; ++j) {
			cout << graph[i*cityAmount + j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < optionalRoads; ++i) {
		in >> possibleRoadsArr[i].city1 >> possibleRoadsArr[i].city2 >> possibleRoadsArr[i].distance;
		--possibleRoadsArr[i].city1;
		--possibleRoadsArr[i].city2;
	}
	in.close();

	checkOptionalRoutes(graph, cityAmount, optionalRoads, possibleRoadsArr);

	delete[] possibleRoadsArr;
	delete[] graph;
}

int main() {
	answerToMyQuestion();
}
