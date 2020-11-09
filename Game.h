#pragma once

#include <QGraphicsRectItem>

#include <QGraphicsView>
#include <QTimer>
#include <QPixMap>
#include "config.h"


class Game : public QGraphicsView {

	Q_OBJECT

enum game_state{READY, RUNNING, PAUSE, GAME_OVER};

private:

	static Game* uniqueInstance;		// Singleton
	Game(QGraphicsView* parent = 0);	// Singleton

	void init();

	QGraphicsScene* _scene;
	QTimer _engine;
	game_state _state = READY;

public:

	static Game* instance();


};