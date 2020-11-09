#pragma once

#include <QGraphicsView>
#include <QTimer>
#include "config.h"

class Game : public QGraphicsView {

	Q_OBJECT

enum game_state{READY, RUNNING, PAUSE, GAME_OVER};

private:

	static Game* uniqueInstance;		// Singleton
	Game(QGraphicsView* parent = 0);	// Singleton

	QGraphicsScene* _scene;
	QTimer _engine;
	game_state _state;

public:

	static Game* instance();

};