#include "AI.h"
#include "Game.h"

AI* AI::uniqueInstance = 0;

AI::AI() {
	
	D = nullptr;
	successor = nullptr;

	update();

}

int AI::nextStationInShortestPath(int x, int y)
{
	return successor[x][y];
}

int AI::findFinalStation(int startStation, int shape){

	int distance = 9999;
	int finalStation = -1;

	for (int i = 0; i < _bigGraph.size(); i++) {
		printf("for, %d\n", i);
		if(Game::instance()->station(i)->stationShape() == shape)
			if (D[startStation][i] < distance) {
				distance = D[startStation][i];
				finalStation = i;
			}
	}
	printf("final station: %d\n", finalStation);
	return finalStation;
}

void AI::update(){

	int n = _bigGraph.size();

	printf("deallocating\n");

	// Deallocations
	if (D != nullptr) {
		for (int x = 0; x < n; x++)
			delete[] D[x];
		delete[] D;
		D = nullptr;
	}

	if (successor != nullptr) {
		for (int x = 0; x < n; x++)
			delete[] successor[x];
		delete[] successor;
		successor = nullptr;
	}

	printf("initializing\n");

	// Initialization
	D = new int* [n];
	successor = new int* [n];
	for (int x = 0; x < n; x++) {

		D[x] = new int[n];
		successor[x] = new int[n];
		for (int y = 0; y < n; y++) {

			if (x == y) {
				D[x][y] = 0;
				successor[x][y] = y;
			}
			else {
				D[x][y] = 99999;
				successor[x][y] = -1;

				for (auto neighbor : _bigGraph.at(x))
					if (neighbor == y) {
						D[x][y] = 1;
						successor[x][y] = y;
					}
			}
		}
	}

	// Floyd-Warshall
	for (int k = 0; k < n; k++)
		for (int x = 0; x < n; x++)
			for (int y = 0; y < n; y++)
				if (D[x][k] + D[k][y] < D[x][y]) {

					D[x][y] = D[x][k] + D[k][y];
					successor[x][y] = successor[x][k];
				}
}

void AI::setAdjacentStation(int x, int y){

	_bigGraph.at(x).push_back(y);
	_bigGraph.at(y).push_back(x);
}

void AI::printGraph(){

	std::cout << "---------- bigGraph ----------\n";
	for (auto& s : _bigGraph) {
		auto it = s.begin();
		while (it != s.end()) {
			printf("%d, ", *it);
			it++;
		}
		printf("\n");
	}
}

AI* AI::instance()
{

	if (uniqueInstance == 0)
		uniqueInstance = new AI;
	return uniqueInstance;
}
