#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QTimer>
#include <QPixMap>
#include "Map.h"
#include "Line.h"
#include "utils.h"


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

	std::list<Station*> _stationsList;

	std::list<Line*> _linesList;

public:

	static Game* instance();

	Station* spawnStation();

public slots:

	// advance to the next frame
	void advance();

	// resets the game
	void reset();

	void start();

	// Event handling
	void keyPressEvent(QKeyEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
};