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

Station* Game::spawnStation(int x, int y) {
	QPoint spawnPoint(x, y);
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

	_stationsList.clear();
	_linesList.clear();

	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);
	_state = READY;

	this->init();
}

void Game::advance() {

	_scene->update();
}

void Game::start() {

	if (_state == READY) {

		for (int i = 0; i < 10; i++) {
			_stationsList.push_back(spawnStation());
			_scene->addItem(_stationsList.back());
			printf("Stazione %d, Forma %d, in coordinate %d, %d\n", i, _stationsList.back()->shape(), _stationsList.back()->position().x(), _stationsList.back()->position().y());
		}

		_engine.start();
		_state = RUNNING;
		printf("Game started\n");
	}
}

void Game::keyPressEvent(QKeyEvent* e){

	// resets game
	if (e->key() == Qt::Key_R || _state == GAME_OVER) {

		reset();
		init();
	}

	// starts new game
	if (e->key() == Qt::Key_S) {

		start();
	}
}

void Game::mousePressEvent(QMouseEvent* e){

	printf("Cursor in pos = %d, %d\n", e->pos().x(), e->pos().y());
	Line* newLine;

	for (auto& s : _stationsList) {

		if (pointerOnStation(s, e->pos())) {
			
			QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
							   s->position().y() + STATION_SIZE / 2);

			newLine = new Line(centerPoint);
			_mousePressed = true;
			_linesList.push_back(newLine);
			_scene->addItem(_linesList.back());
			
			break;
		}
	}
}

void Game::mouseMoveEvent(QMouseEvent* e){
	
	if (_mousePressed) {
		QPoint currentPoint(e->pos().x() / GAME_SCALE,
							e->pos().y() / GAME_SCALE);
		_linesList.back()->setCurrentPoint(currentPoint);

		for (auto& s : _stationsList) {

			if (pointerOnStation(s, e->pos())) {

				QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
								   s->position().y() + STATION_SIZE / 2);

				if (_linesList.back()->validPoint(centerPoint)) {
					_linesList.back()->setNextPoint(centerPoint);

					if (_linesList.back()->circularLine())
						_mousePressed = false;
				}
			}
		}
	}
}

void Game::mouseReleaseEvent(QMouseEvent* e){ 

	if (_mousePressed) {

		printf("Cursor released in pos = %d, %d\n", e->pos().x(), e->pos().y());
		// _linesList.back()->setCurrentPoint(_linesList.back()->lastPoint());
		_linesList.back()->setCurrentPoint(_linesList.back()->TcapPoint());
		_mousePressed = false;

		if (_linesList.back()->size() < 2) {
			_scene->removeItem(_linesList.back());
			delete _linesList.back();
			_linesList.pop_back();
		}
	}
}
