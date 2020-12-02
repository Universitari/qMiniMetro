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
		setFixedHeight(GAME_SCALE * _scene->height() + 2);
		Map::instance()->load(":/Graphics/LondonMap.png", _scene);
	}
}

void Game::reset() {

	for (auto& s : _stationsList)
		delete s;
	_stationsList.clear();

	for (auto& l : _linesList)
		delete l;
	_linesList.clear();

	_graph.clear();

	for (auto& b : _deleteButtons)
		delete b;
	_deleteButtons.clear();

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

		for (int i = 0; i < 20; i++) {
			_stationsList.push_back(spawnStation());
			_scene->addItem(_stationsList.back());
			printf("Stazione %d, Forma %d, in coordinate %d, %d\n", i, _stationsList.back()->shape(), _stationsList.back()->position().x(), _stationsList.back()->position().y());
		}

		for (int i = 0; i < MAX_LINES; i++){
			_deleteButtons.push_back(new Button(i));
			_scene->addItem(_deleteButtons.back());
		}

		for (int i = 0; i < MAX_LINES; i++) {
			Line* tmp = 0;
			_linesList.push_back(tmp);
			_linesList.shrink_to_fit();
		}

		//treno = new Train(0, QPoint(400, 400));
		//_scene->addItem(treno);

		_engine.start();
		_state = RUNNING;
		printf("Game started\n");
	}
}

Station* Game::spawnStation(int x, int y) {
	QPoint spawnPoint(x, y);
	bool found = false;

	if (!_stationsList.empty())
		do {

			for (auto& s : _stationsList) {
				
				if (distance(spawnPoint, s->position()) < STATION_SIZE*4) {

					spawnPoint.setX(2*STATION_SIZE + rand() % (WINDOW_WIDTH - 4*STATION_SIZE));
					spawnPoint.setY(2*STATION_SIZE + rand() % (WINDOW_HEIGHT - 8*STATION_SIZE));
					// printf("cambiate coordinate\n");
					found = true;
					break;
				}
				else
					found = false;

			}
		} while (found);

	_stationsNumber++;
	Station* newStation = new Station(spawnPoint, _stationsNumber);
	_graph.emplace_back();
	return newStation;
}

void Game::deleteLine(int lineIndex){

	// it crashed with the delete action
	if (_linesList.at(lineIndex) != 0) {
		_scene->removeItem(_linesList.at(lineIndex));
		// delete _linesList.at(lineIndex);
		_linesList.at(lineIndex) = 0;
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

void Game::mousePressEvent(QMouseEvent* e){

	// printf("Cursor in pos = %d, %d\n", e->pos().x(), e->pos().y());

	// Add new line
	if (!_mousePressed) {

		int i = 0;
		for (auto& l : _linesList) {
			if (!l) {
				_activeLine = i;
				break;
			}
			i++;
		}
		//printf("active line: %d\n", _activeLine);

		if (_activeLine != -1)
			for (auto& s : _stationsList) {

				if (s->pointerOnStation(e->pos())) {

					QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
									   s->position().y() + STATION_SIZE / 2);

					_linesList.at(_activeLine) = new Line(centerPoint, _activeLine);
					_mousePressed = true;
					_scene->addItem(_linesList.at(_activeLine));
					_activeStation = s->index();

					break;
				}
			}
	}

	// Edit Line
	int i = 0;
	for (auto& l : _linesList) {

		if(l != 0)
			if (l->pointerOnCap(e->pos()) && !l->circularLine()) {
				_mousePressed = true;
				_activeLine = i;
			}
		i++;
	}

	QGraphicsView::mousePressEvent(e);
}

void Game::mouseMoveEvent(QMouseEvent* e){
	
	if (_mousePressed) {
		QPoint currentPoint(e->pos().x() / GAME_SCALE,
							e->pos().y() / GAME_SCALE);
		_linesList.at(_activeLine)->setCurrentPoint(currentPoint);

		for (auto& s : _stationsList) {

			if (s->pointerOnStation(e->pos())) {

				QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
								   s->position().y() + STATION_SIZE / 2);

				if (_linesList.at(_activeLine)->validPoint(centerPoint)) {
					_linesList.at(_activeLine)->setNextPoint(centerPoint);
					_graph.at(_activeStation).push_back(s->index());
					_graph.at(s->index()).push_back(_activeStation);
					_activeStation = s->index();

					if (_linesList.at(_activeLine)->circularLine()) {
						
						_linesList.at(_activeLine)->updateTcapPoint();
						_mousePressed = false;
					}
				}
			}
		}
	}

	QGraphicsView::mouseMoveEvent(e);
}

void Game::mouseReleaseEvent(QMouseEvent* e){ 

	if (_mousePressed) {
	
		// printf("Cursor released in pos = %d, %d\n", e->pos().x(), e->pos().y());
		_linesList.at(_activeLine)->updateTcapPoint();
		_mousePressed = false;

		if (_linesList.at(_activeLine)->size() < 2) {
			_scene->removeItem(_linesList.at(_activeLine));
			delete _linesList.at(_activeLine);
			_linesList.at(_activeLine) = 0;
		}
	}

	_activeLine = -1;

	QGraphicsView::mouseReleaseEvent(e);
}
