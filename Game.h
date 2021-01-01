#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QTimer>
#include <QPixMap>
#include <iostream>
#include "Map.h"
#include "utils.h"
#include "Station.h"
#include "Line.h"
#include "Button.h"
#include "Train.h"
#include "Passenger.h"


class Game : public QGraphicsView {

	Q_OBJECT

enum game_state{READY, RUNNING, PAUSE, GAME_OVER};

private:

	static Game* uniqueInstance;		// Singleton
	Game(QGraphicsView* parent = 0);	// Singleton

	void init();

	QGraphicsScene* _scene;
	QTimer _engine;
	game_state _state;
	bool _mousePressed = false;
	int _stationsNumber = -1;
	int _activeLine = -1;
	int _activeStation = -1;

	std::vector<Station*> _stationsList;
	std::vector<Line*> _linesList;
	std::vector<Button*> _deleteButtons;
	std::vector<Train*> _trainsList;

	Passenger* passeggero;

	std::vector<std::list<int>> _graph;

	// typename std::vector<Line*>::iterator _lineIterator = _linesList.begin();

public:

	static Game* instance();

	Station* spawnStation(int x = 400, int y = 400);

	void deleteLine(int lineIndex);

	bool lineExists(int index) { return _linesList.at(index); }

	// Event handling
	void keyPressEvent(QKeyEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);

public slots:

	// advance to the next frame
	void advance();
	// resets the game
	void reset();
	// starts the game
	void start();


};