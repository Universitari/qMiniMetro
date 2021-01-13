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
	QObject::connect(&_passengerTimer, SIGNAL(timeout()), this, SLOT(spawnPassenger()));
	QObject::connect(&_stationsTimer, SIGNAL(timeout()), this, SLOT(spawnStation()));

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

	_stationsTimer.stop();
	_passengerTimer.stop();

	for (auto& s : _stationsList)
		delete s;
	_stationsList.clear();

	for (auto& l : _linesList)
		delete l;
	_linesList.clear();

	for (auto& b : _deleteButtons)
		delete b;
	_deleteButtons.clear();

	for (auto& t : _trainsList)
		delete t;
	_trainsList.clear();

	for (auto& p : _passengersList)
		delete p;
	_passengersList.clear();

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].clear();

	_stationsNumber = -1;
	_activeStation = -1;
	_activeLine = -1;

	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);

	_stationsTimer.setInterval(20000);
	_passengerTimer.setInterval(10000 / sqrt(5));
	_state = READY;

	this->init();
}

void Game::advance() {

	// Trains movement
	for (auto& t : _trainsList) {
		if (t != 0) {
			if (t->passengers() != 6) {
				auto iter = _graph[t->lineIndex()].begin();
				int i = 0;
				for (iter; iter < _graph[t->lineIndex()].end(); iter++) {
					if (!(*iter).empty()) { // if the station is connected to the line

						if (t->collidesWithItem(_stationsList.at(i), Qt::IntersectsItemBoundingRect) && !t->colliding()) {
							t->setColliding(true);
							if (t->state() == 1) // state == MOVING
								t->setState(0); // set state = STOPPED
								
						}
						else {
							t->setState(1); // state == MOVING
							if (!t->collidesWithItem(_stationsList.at(i), Qt::IntersectsItemBoundingRect))
								t->setColliding(false);
						}

						//if (t->state() == 2 && distance(t->position(), _stationsList.at(i)->centerPos()) < 0.1) // state == BRAKING
						//	t->setState(3); // set state = STOPPED
						
						if (t->state() == 0) { // state == STOPPED
							for (auto& p : _passengersList) {
								if (p->stationIndex() == i) {
									t->incrementPassengers();
									p->setTicket(t->passengers());
									p->getOnTrain(t->index(), t->passengerPos(p->ticket()));
									p->setRotation(t->rotationAngle());

									reorgPassengers(i);
								}
							}
							t->setState(1); // set state = MOVING
						}
					}
					i++;
				}
			}
			t->advance();
		}

	}

	for (auto& p : _passengersList)
		if (p != 0)
			if (p->trainIndex() != -1) {
				p->setPos(_trainsList.at(p->trainIndex())->passengerPos(p->ticket()));
				p->moveTransformPoint(_trainsList.at(p->trainIndex())->position());
				p->setRotation(_trainsList.at(p->trainIndex())->rotationAngle());
			}

	_scene->update();
	
}

void Game::start() {

	if (_state == READY) {

		for (int i = 0; i < MAX_LINES; i++){
			_deleteButtons.push_back(new Button(i));
			_scene->addItem(_deleteButtons.back());
		}

		for (int i = 0; i < 3; i++) {
			_stationsNumber++;
			Station* station = new Station(startPos[i], _stationsNumber);
			_stationsList.push_back(station);
			_scene->addItem(_stationsList.back());
			for(int i = 0; i < MAX_LINES; i++)
				_graph[i].emplace_back();
		}

		for (int i = 0; i < MAX_LINES; i++) {
			Line* tmp = 0;
			_linesList.push_back(tmp);
			_linesList.shrink_to_fit();
		}

		for (int i = 0; i < MAX_TRAINS; i++) {
			Train* tmp = 0;
			_trainsList.push_back(tmp);
			_trainsList.shrink_to_fit();
		}


		_engine.start();
		_passengerTimer.start();
		_stationsTimer.start();
		_state = RUNNING;
		printf("Game started\n");
	}
}

void Game::spawnStation() {

	QPoint spawnPoint;
	spawnPoint.setX(2 * STATION_SIZE + rand() % (WINDOW_WIDTH - 4 * STATION_SIZE - PASSENGER_SIZE * (MAX_PASS_STATION / 2)));
	spawnPoint.setY(2 * STATION_SIZE + rand() % (WINDOW_HEIGHT - 8 * STATION_SIZE));
	bool found = false;

	do {

		if (spawnAreaAvailable(spawnPoint, _stationsNumber))
			for (auto& s : _stationsList)
				if (distance(spawnPoint, s->position()) < STATION_SIZE * 4) {

					spawnPoint.setX(2 * STATION_SIZE + rand() % (WINDOW_WIDTH - 4 * STATION_SIZE - PASSENGER_SIZE * (MAX_PASS_STATION / 2)));
					spawnPoint.setY(2 * STATION_SIZE + rand() % (WINDOW_HEIGHT - 8 * STATION_SIZE));
					// printf("cambiate coordinate\n");
					found = true;
					break;
				}
				else
					found = false;
			
		else {
			spawnPoint.setX(2 * STATION_SIZE + rand() % (WINDOW_WIDTH - 4 * STATION_SIZE - PASSENGER_SIZE * (MAX_PASS_STATION / 2)));
			spawnPoint.setY(2 * STATION_SIZE + rand() % (WINDOW_HEIGHT - 8 * STATION_SIZE));
			found = true;
		}
		} while (found);

	_stationsNumber++;
	Station* newStation = new Station(spawnPoint, _stationsNumber);
	_stationsList.push_back(newStation);
	_scene->addItem(_stationsList.back());
	
	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].emplace_back();

	if (_stationsNumber == MAX_STATIONS)
		_stationsTimer.stop();
}

void Game::spawnPassenger(){

	int index;
	int i = 0;
	bool stationsFull = false;

	// Are all stations full?
	do {
		index = (rand() % (_stationsNumber + 1));
		if (i++ == 100)
			stationsFull = true;
	} while (_stationsList.at(index)->passengers() >= MAX_PASS_STATION && !stationsFull);

	if (!stationsFull) {

		QPoint position = passPosStation(index);
		
		int shape;
		do {
			shape = randomShape();
		} while (shape == _stationsList.at(index)->stationShape());

		_stationsList.at(index)->addPassenger();

		Passenger* passenger = new Passenger(index, position, shape);
		_passengersList.push_back(passenger);
		_scene->addItem(_passengersList.back());
	}
	else printf("YOU DIED\n"); // implement death

	_passengerTimer.setInterval((10000 / sqrt(_stationsNumber)) + ((rand() % 3) - 1)*(rand() % 1000));

}

int Game::randomShape(){

	int index = rand() % (_stationsNumber+1);
	return _stationsList.at(index)->stationShape();
}

void Game::deleteLine(int lineIndex){

	// it crashed with the delete action
	if (lineExists(lineIndex)) {

		for (auto& t : _trainsList) 
			if (t != 0) 
				if (t->lineIndex() == lineIndex) {
					// if the program doesn't behave correctly, try this
					// t->setPath(QPainterPath());
					t->setVisible(false);
					t = 0;
				}
		
		_scene->removeItem(_linesList.at(lineIndex));
		// delete _linesList.at(lineIndex);
		_linesList.at(lineIndex) = 0;
		
	}
}

QPoint Game::passPosStation(int stationIndex)
{
	QPoint position = _stationsList.at(stationIndex)->position();

	position.setX(position.x() + STATION_SIZE + 5);
	if (_stationsList.at(stationIndex)->passengers() < MAX_PASS_STATION / 2)
		position.setY(position.y() - PASSENGER_SIZE - 1);
	else position.setY(position.y() + 1);

	position.setX(position.x() + (_stationsList.at(stationIndex)->passengers() % (MAX_PASS_STATION / 2)) * (2 + PASSENGER_SIZE));
	return position;
}

void Game::reorgPassengers(int stationIndex){

	_stationsList.at(stationIndex)->removePassengers();

	for (auto& p : _passengersList) {
		if (p->stationIndex() == stationIndex) {

			p->setPos(passPosStation(stationIndex));
			_stationsList.at(stationIndex)->addPassenger();
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

		for (int j = 0; j < MAX_LINES; j++) {
			std::cout << "---------- Graph " << j << " ----------\n";
			for (int i = 0; i < _graph[j].size(); i++) {
				std::cout << "Station " << i << " connected to station ";
				for (auto& i : _graph[j].at(i))
					std::cout << i << ", ";
				std::cout << std::endl;
			}
		}

		std::cout << "---------- Trains ----------\n";
		for (auto& t : _trainsList)
			if(t != 0)
				std::cout << "Train " << t->index() << " on line "
					<< t->lineIndex() << "\n";

	}
	if (e->key() == Qt::Key_D && _state == RUNNING) {
		bool visible = true;
		if (_stationsList.at(0)->isVisible())
			visible = false;
		for (auto& s : _stationsList)
			s->setVisible(visible);
	}

	if (e->key() == Qt::Key_O) {
		/*
		for (int i = 0; i < 10; i++) {

			Passenger* passenger = spawnPassenger();
			if (passenger == 0) {
				printf("HAI PERSO\n");
				break;
			}
			else {
				_passengersList.push_back(passenger);
				_scene->addItem(_passengersList.back());
			}
		}
		*/
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

					_graph[_activeLine].at(_activeStation).push_back(s->index());
					_graph[_activeLine].at(s->index()).push_back(_activeStation);
					_activeStation = s->index();

					if (_linesList.at(_activeLine)->circularLine()) {
						
						_linesList.at(_activeLine)->updateTcapPoint();
						
						bool hasTrain = false;
						int id = 0;

						for (auto& t : _trainsList)
							if (t != 0) {
								if (t->lineIndex() == _activeLine) {
									t->setPath(_linesList.at(_activeLine)->path());
									t->setCircular(true);
									hasTrain = true;
								}
							}

						if (!hasTrain) {
							for (auto& t : _trainsList) {
								if (t == 0) {
									t = new Train(_activeLine, id, _linesList.at(_activeLine)->firstPoint(), _linesList.at(_activeLine)->path());
									t->setCircular(true);
									_scene->addItem(t);
									break;
								}
								id++;
							}
						}

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

		if (_linesList.at(_activeLine)->size() < 2) {
			_scene->removeItem(_linesList.at(_activeLine));
			//delete _linesList.at(_activeLine);
			_linesList.at(_activeLine) = 0;
		}

		if (lineExists(_activeLine)){

			_linesList.at(_activeLine)->updateTcapPoint();
			
			bool hasTrain = false;
			int id = 0;

			for (auto& t : _trainsList)
				if (t != 0) {
					if (t->lineIndex() == _activeLine) {
						t->setPath(_linesList.at(_activeLine)->path());
						hasTrain = true;
					}
				}

			if (!hasTrain) {
				for (auto& t : _trainsList) {
					if (t == 0) {
						t = new Train(_activeLine, id, _linesList.at(_activeLine)->firstPoint(), _linesList.at(_activeLine)->path());
						_scene->addItem(t);
						break;
					}
					id++;
				}
			}


		}

		_mousePressed = false;
				
	}

	_activeLine = -1;

	QGraphicsView::mouseReleaseEvent(e);
}
