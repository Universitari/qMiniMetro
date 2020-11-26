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

void Game::reset() {

	_stationsList.clear();
	_linesList.clear();
	_graph.clear();
	_stationsNumber = -1;
	_activeStation = -1;
	_activeLine = -1;

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

	_stationsNumber++;
	Station* newStation = new Station(spawnPoint, _stationsNumber);
	_graph.emplace_back();
	return newStation;
}

void Game::addLine(QPoint stationPos, int index) {

	printf("sono dentro addLine\n");

		QPoint centerPoint(stationPos.x() + STATION_SIZE / 2,
						   stationPos.y() + STATION_SIZE / 2);
		
		Line* newLine;
		newLine = new Line(centerPoint);
		_mousePressed = true;
		_linesList.push_back(newLine);
		_activeLine = _linesList.size() - 1;
		_scene->addItem(_linesList.at(_activeLine));
		_activeStation = index;

}

void Game::addStationToLine(QPoint stationPos, int index){

		//QPoint currentPoint(e->pos().x() / GAME_SCALE,
		//	e->pos().y() / GAME_SCALE);
		//_linesList.at(_activeLine)->setCurrentPoint(currentPoint);

		QPoint centerPoint(stationPos.x() + STATION_SIZE / 2,
						   stationPos.y() + STATION_SIZE / 2);

		if (_linesList.at(_activeLine)->validPoint(centerPoint)) {
			_linesList.at(_activeLine)->setNextPoint(centerPoint);
			_graph.at(_activeStation).push_back(index);
			_graph.at(index).push_back(_activeStation);
			_activeStation = index;

			if (_linesList.at(_activeLine)->circularLine()) {

				//_linesList.at(_activeLine)->setCurrentPoint(_linesList.at(_activeLine)->lastPoint());
				_linesList.at(_activeLine)->updateTcapPoint();
				_mousePressed = false;
			}
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

	if (e->key() == Qt::Key_P) {

		for (int i = 0; i < _graph.size(); i++) {
			std::cout << "Station " << i << " connected to station ";
			for (auto& i : _graph.at(i))
				std::cout << i << ", ";
			std::cout << std::endl;
		}
	}
}

/*void Game::mousePressEvent(QMouseEvent* e){

	// printf("Cursor in pos = %d, %d\n", e->pos().x(), e->pos().y());

	if (!_linesList.empty()) {
		int i = 0;
		for (auto& l : _linesList) {

			if (l->pointerOnCap(e->pos()) && !l->circularLine()) {
				_mousePressed = true;
				_activeLine = i;
			}
			i++;
		}
	}

}*/

/*void Game::mouseMoveEvent(QMouseEvent* e){
	
	
}*/

void Game::mouseReleaseEvent(QMouseEvent* e){ 

	if (_mousePressed) {
	
		// printf("Cursor released in pos = %d, %d\n", e->pos().x(), e->pos().y());
		//_linesList.at(_activeLine)->setCurrentPoint(_linesList.at(_activeLine)->lastPoint());
		_linesList.at(_activeLine)->updateTcapPoint();
		_mousePressed = false;

		if (_linesList.at(_activeLine)->size() < 3) {
			_scene->removeItem(_linesList.at(_activeLine));
			delete _linesList.at(_activeLine);
			_linesList.pop_back();
		}
	}
}
