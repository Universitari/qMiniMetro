#include "Game.h"

Game* Game::uniqueInstance = 0;

Game* Game::instance() {

	if (uniqueInstance == 0)
		uniqueInstance = new Game;
	return uniqueInstance;
}

Game::Game(QGraphicsView* parent) : QGraphicsView(parent) {

	_scene = new QGraphicsScene;
	setScene(_scene);
	_scene->setSceneRect(0, 0, 1920, 1080);
	this->init();

	QPixmap map(":/Graphics/LondonMap.png");
	
	bool x = map.isNull();
	if (x) printf("map e' vuoto");
	else printf("map non e' vuoto");

	_scene->addPixmap(map);

	scale(GAME_SCALE, GAME_SCALE);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// setInteractive(false);		// disables events DA VEDERE

	QGraphicsRectItem* rettangolo;
	rettangolo = new QGraphicsRectItem(310, 175, 20, 10);
	_scene->addItem(rettangolo);

}

void Game::init(){

	if (_state == READY)
	{
		_scene->clear();

		// set window dimensions
		setFixedWidth(GAME_SCALE * _scene->width());
		setFixedHeight(GAME_SCALE * _scene->height());
	}
}