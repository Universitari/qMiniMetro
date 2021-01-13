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
	QTimer _passengerTimer;
	QTimer _stationsTimer;
	game_state _state;

	bool _mousePressed = false;
	int _stationsNumber = -1;
	int _activeLine = -1;
	int _activeStation = -1;

	QPoint startPos[3] = { QPoint(ST_1[0], ST_1[1]),
						   QPoint(ST_2[0], ST_2[1]), 
						   QPoint(ST_3[0], ST_3[1]) };

	std::vector<Station*> _stationsList;
	std::vector<Line*> _linesList;
	std::vector<Button*> _deleteButtons;
	std::vector<Train*> _trainsList;
	std::vector<Passenger*> _passengersList;


	std::vector<std::list<int>> _graph[7];

	// typename std::vector<Line*>::iterator _lineIterator = _linesList.begin();

public:

	static Game* instance();
	
	int randomShape();
	void deleteLine(int lineIndex);
	bool lineExists(int index) { return _linesList.at(index); }
	QPoint passPosStation(int stationIndex);
	void reorgPassengers(int stationIndex);

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
	// Spawns a passenger
	void spawnPassenger();
	// Spawns a station
	void spawnStation();


};