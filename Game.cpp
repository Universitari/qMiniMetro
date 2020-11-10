#include "Game.h"

Game* Game::uniqueInstance = 0;

Game* Game::instance() {

	if (uniqueInstance == 0)
		uniqueInstance = new Game;
	return uniqueInstance;
}

Station* Game::spawnStation() {
	QPoint spawnPoint(400, 400);
	bool found = true;

	if (!_stationsList.empty())
		while (found) {
			for (auto& s : _stationsList) {
				if (s->position() == spawnPoint) {
					found = true;
					break;
				}
				else found = false;
			}

			if (found) {
				spawnPoint.setX(rand() % WINDOW_WIDTH);
				spawnPoint.setY(rand() % WINDOW_HEIGHT);
				printf("cambiate coordinate\n");
			}
		}

	Station* newStation = new Station(spawnPoint);
	return newStation;
}

Game::Game(QGraphicsView* parent) : QGraphicsView(parent) {

	srand(time(NULL));

	_scene = new QGraphicsScene;
	setScene(_scene);
	_scene->setSceneRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	this->init();

	Map::instance()->load(":/Graphics/LondonMap.png", _scene);
	for (int i = 0; i < 10; i++) {
		printf("carico stazione %d\n", i);
		_stationsList.push_back(spawnStation());
		_scene->addItem(_stationsList.back());
	}


	scale(GAME_SCALE, GAME_SCALE);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// setInteractive(false);		// disables events DA VEDERE

	// QGraphicsRectItem* rettangolo;
	// rettangolo = new QGraphicsRectItem(310, 175, 20, 10);
	// _scene->addItem(rettangolo);

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