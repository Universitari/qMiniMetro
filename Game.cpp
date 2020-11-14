#include "Game.h"

Game* Game::uniqueInstance = 0;

Game* Game::instance() {

	if (uniqueInstance == 0)
		uniqueInstance = new Game;
	return uniqueInstance;
}

Game::Game(QGraphicsView* parent) : QGraphicsView(parent) {

	srand(time(NULL));

	_scene = new QGraphicsScene;
	setScene(_scene);
	_scene->setSceneRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	setInteractive(true);

	this->reset();
	
	scale(GAME_SCALE, GAME_SCALE);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QObject::connect(&_engine, SIGNAL(timeout()), this, SLOT(advance()));

}

void Game::init() {

	if (_state == READY)
	{
		_scene->clear();

		// set window dimensions
		setFixedWidth(GAME_SCALE * _scene->width());
		setFixedHeight(GAME_SCALE * _scene->height());
		Map::instance()->load(":/Graphics/LondonMap.png", _scene);
	}
}

Station* Game::spawnStation() {
	QPoint spawnPoint(400, 400);
	bool found = true;

	if (!_stationsList.empty())
		while (found) {

			for (auto& s : _stationsList) {

				if (s->position() == spawnPoint) {

					spawnPoint.setX(2*STATION_SIZE + rand() % (WINDOW_WIDTH - 4*STATION_SIZE));
					spawnPoint.setY(2*STATION_SIZE + rand() % (WINDOW_HEIGHT - 4*STATION_SIZE));
					break;
				}
				else found = false;
			}
		}

	Station* newStation = new Station(spawnPoint);
	return newStation;
}

void Game::reset(){

	_state = READY;
	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);

	this->init();
}

void Game::advance() {

	_scene->update();
}

void Game::start() {

	_state = RUNNING;

	for (int i = 0; i < 10; i++) {
		_stationsList.push_back(spawnStation());
		_scene->addItem(_stationsList.back());
		printf("Stazione %d, Forma %d, in coordinate %d, %d\n", i, _stationsList.back()->shape(), _stationsList.back()->position().x(), _stationsList.back()->position().y());
	}

	_lines.push_back(new Line);
	_scene->addItem(_lines.back());

	_engine.start();
}

void Game::keyPressEvent(QKeyEvent* e)
{
	// reset game
	if (e->key() == Qt::Key_R || _state == GAME_OVER) {

		reset();
		init();
	}

	// start new game
	if (e->key() == Qt::Key_S) {

		start();
		printf("Game started\n");
	}

}

void Game::mousePressEvent(QGraphicsSceneMouseEvent* e){

	printf("click\n");

	for (auto& s : _stationsList) {

		if (pointerOnStation(s, e->scenePos().toPoint())) {
			printf("stazione cliccata\n");
			break;
		}
		else printf("stazione non cliccata\n");
	}
}
