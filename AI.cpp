#include "AI.h"
#include "Game.h"

AI* AI::uniqueInstance = 0;

AI::AI() {
	
	D = 0;
	successor = 0;
	n = _bigGraph.size();

	updateBigGraph();

}

int AI::nextStationInShortestPath(int x, int y){

	return successor[x][y];
}

void AI::findFinalStations(int startStation, int shape, std::vector<int>& v){

	int distance = 9999;

	for (int i = 0; i < _bigGraph.size(); i++) {
		if(Game::instance()->station(i)->stationShape() == shape)
			if (D[startStation][i] <= distance) {
				if (D[startStation][i] < distance) {
					v.clear();
					distance = D[startStation][i];
				}
				v.push_back(i);
			}
	}
}

void AI::update(){

	// Deallocations
	if (D != 0) {
		for (int x = 0; x < n; x++)
			delete[] D[x];
		delete[] D;
		D = 0;
	}

	if (successor != 0) {
		for (int x = 0; x < n; x++)
			delete[] successor[x];
		delete[] successor;
		successor = 0;
	}
	
	n = _bigGraph.size();

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

	Game::instance()->updatePassengersDestinations();

}

void AI::clearGraph(){

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].clear();

}

void AI::deleteGraphLine(int lineIndex){

	for (auto& list : _graph[lineIndex])
		list.clear();

	updateBigGraph();

}

void AI::addStation(){

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].emplace_back();

	_bigGraph.emplace_back();

}

void AI::addLink(int x, int y, int lineIndex){

	_graph[lineIndex].at(x).push_back(y);
	_graph[lineIndex].at(y).push_back(x);

	updateBigGraph();
}

void AI::printGraph(){

	for (int j = 0; j < MAX_LINES; j++) {
		std::cout << "---------- Graph " << j << " ----------\n";
		for (int i = 0; i < _graph[j].size(); i++) {
			std::cout << "Station " << i << " connected to station ";
			for (auto& i : _graph[j].at(i))
				std::cout << i << ", ";
			std::cout << std::endl;
		}
	}
}

void AI::setOriented(bool oriented, int firstStation, Train* train){

	if (oriented) {

		for (auto& l : _graph[train->lineIndex()])
			l.clear();

		int s1 = firstStation;
		bool flag = true;

		do {
			int s2 = Game::instance()->nextStation(train->lineIndex(), s1, train->index());
			if (_graph[train->lineIndex()].at(s1).empty()) {
				_graph[train->lineIndex()].at(s1).push_back(s2);
				s1 = s2;
			}
			else
				flag = false;

		} while (flag);

	} 
	else { 

		bool flag = true;
		int s1 = firstStation;

		do {

			int s2 = _graph[train->lineIndex()].at(s1).front();
			if (_graph[train->lineIndex()].at(s2).size() != 2) {
				_graph[train->lineIndex()].at(s2).push_back(s1);
				s1 = s2;
			}
			else
				flag = false;

		} while (flag);

	}

	updateBigGraph();
	update();
}

void AI::printBigGraph(){

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

void AI::updateBigGraph(){

	std::set<int> adjacentStations;

	clearBigGraph();

	for (int i = 0; i < _graph[0].size(); i++) {
		for (int j = 0; j < MAX_LINES; j++) {

			for (auto& adjS : _graph[j].at(i))
				adjacentStations.insert(adjS);
		}

		_bigGraph.emplace_back();
		for (auto& adjS : adjacentStations)
			_bigGraph.at(i).push_back(adjS);

		adjacentStations.clear();
	}
}

void AI::read(const QJsonObject& json){

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].clear();

	for (int j = 0; j < MAX_LINES; j++)
		for (int i = 0; i <= Game::instance()->stationsNumber(); i++)
			_graph[j].emplace_back();

	if (json.contains("Graph") && json["Graph"].isArray()) {

		QJsonArray arr = json["Graph"].toArray();

		for (int i = 0; i < MAX_LINES; i++) {

			QJsonArray arr2 = arr.at(i).toArray();

			for (int j = 0; j <= Game::instance()->stationsNumber(); j++) {

				QJsonObject obj = arr2.at(j).toObject();
				int index = -1;
				if (obj.contains("First adjacent station") && obj["First adjacent station"].isDouble()) {
					index = obj["First adjacent station"].toInt();
					_graph[i].at(j).push_back(index);
				}

				if (obj.contains("Second adjacent station") && obj["Second adjacent station"].isDouble()) {
					index = obj["Second adjacent station"].toInt();
					_graph[i].at(j).push_back(index);
				}
			}
		}
	}

	updateBigGraph();

}

void AI::write(QJsonObject& json) const{

	QJsonArray graphArray;
	QJsonArray graphStationsArray[300];

	for (int i = 0; i < MAX_LINES; i++) {
		for (int j = 0; j <= Game::instance()->stationsNumber(); j++) {
			QJsonObject obj;

			if (!_graph[i].at(j).empty())
				obj["First adjacent station"] = _graph[i].at(j).front();
			if (_graph[i].at(j).size() == 2)
				obj["Second adjacent station"] = _graph[i].at(j).back();

			graphStationsArray[i].append(obj);
		}
		graphArray.append(graphStationsArray[i]);
	}
	json["Graph"] = graphArray;

}

AI* AI::instance()
{

	if (uniqueInstance == 0)
		uniqueInstance = new AI;
	return uniqueInstance;
}
