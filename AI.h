#pragma once
#include <vector>
#include <list>

class AI {

private:

	AI();
	static AI* uniqueInstance;

	std::vector<std::list<int>> _bigGraph;
	int** successor;
	int** D;

public:

	static AI* instance();

	// AI functions
	int nextStationInShortestPath(int x, int y);
	int findFinalStation(int startStation, int shape);
	void update();

	// Graph functions
	void setAdjacentStation(int x, int y);
	void addStation() { _bigGraph.emplace_back(); }
	void clearGraph() { _bigGraph.clear(); }
	void printGraph();

};