#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <iostream>
#include <QApplication>
#include <QFont>
#include "utils.h"
#include "Station.h"
#include "Line.h"
#include "Button.h"
#include "Train.h"
#include "Passenger.h"


class Game : public QGraphicsView {

	Q_OBJECT

enum game_state{READY, RUNNING, PAUSED, GAME_OVER};

private:

	static Game* uniqueInstance;		// Singleton
	Game(QGraphicsView* parent = 0);	// Singleton

	void init();

	QGraphicsScene* _scene;
	QTimer _engine;
	QTimer _passengerTimer;
	QTimer _stationsTimer;
	QTimer _passengersInOutTimer;
	game_state _state;
	bool _debug;
	float _fpsMultiplier;

	bool _mousePressed = false;
	int _stationsNumber = -1;
	int _activeLine = -1;
	int _activeStation = -1;
	unsigned int _score = 0;
	QGraphicsTextItem *_scoreText = 0;

	QPoint startPos[3] = { QPoint(ST_1[0], ST_1[1]),
						   QPoint(ST_2[0], ST_2[1]), 
						   QPoint(ST_3[0], ST_3[1]) };

	std::vector<Station*> _stationsVec;
	std::vector<Line*> _linesVec;
	std::vector<Button*> _deleteButtons;
	std::vector<Train*> _trainsVec;
	std::vector<Passenger*> _passengersVec;

	std::vector<std::list<int>> _graph[7];

public:

	static Game* instance();
	
	int randomShape();
	void deleteLine(int lineIndex);
	bool lineExists(int index) { return _linesVec.at(index); }
	QPoint passPosStation(int stationIndex);
	void reorgPassengers(int stationIndex);
	bool passengersArrived(int TrainIndex, int StationIndex);
	int nearestStation(QPoint trainPos);
	int nextStation(int lineIndex, int stationIndex, int trainIndex);
	bool trainArrived(int trainIndex);
	void addTrain(QRect rect);
	bool availableTrains();

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
	// Passengers move from and to the stations
	void passengersInOut();


};