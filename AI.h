#pragma once
#include <vector>
#include <list>
#include <set>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "Train.h"

class AI {

private:

	AI();
	static AI* uniqueInstance;

	std::vector<std::list<int>> _graph[7];
	std::vector<std::list<int>> _bigGraph;

	int** successor;
	int** D;
	int n;

public:

	static AI* instance();

	// AI functions
	int nextStationInShortestPath(int x, int y);
	void findFinalStations(int startStation, int shape, std::vector<int> &v);
	void update();

	// Small Graphs functions
	void clearGraph();
	void deleteGraphLine(int lineIndex);
	void addStation();
	void addLink(int x, int y, int lineIndex);
	void printGraph();
	void setOrientation(bool oriented, int firstStation, Train* train);

	// Big Graph functions	
	void clearBigGraph() { _bigGraph.clear(); }
	void printBigGraph();
	void updateBigGraph();

	// Savegame functions
	void read(const QJsonObject& json);
	void write(QJsonObject& json) const;

};