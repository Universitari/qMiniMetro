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
	QObject::connect(&_passengersInOutTimer, SIGNAL(timeout()), this, SLOT(passengersInOut()));

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
	_passengersInOutTimer.stop();


	for (auto& s : _stationsVec)
		delete s;
	_stationsVec.clear();

	for (auto& l : _linesVec)
		delete l;
	_linesVec.clear();

	for (auto& b : _deleteButtons)
		delete b;
	_deleteButtons.clear();

	for (auto& t : _trainsVec)
		delete t;
	_trainsVec.clear();

	for (auto& p : _passengersVec)
		delete p;
	_passengersVec.clear();

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].clear();

	_stationsNumber = -1;
	_activeStation = -1;
	_activeLine = -1;
	_score = 0;

	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);

	_passengersInOutTimer.setTimerType(Qt::PreciseTimer);

	_stationsTimer.setInterval(10000);
	_passengerTimer.setInterval(2000 / sqrt(5));
	_passengersInOutTimer.setInterval(500);
	_state = READY;

	this->init();
}

void Game::advance() {

	// Trains movement
	for (auto& t : _trainsVec)
		if (t != 0)
			t->advance();


	// Passengers get on trains
	for (auto& t : _trainsVec) {
		if (t != 0) {
			if (trainArrived(t->index())) {
				if ((_stationsVec.at(t->currentStation())->passengers() > 0) || passengersArrived(t->index(), t->currentStation())) {
					t->setState(0);
					//printf("train stopped!\n");
					}
				}
			if(t->state() == 0)
				if ((_stationsVec.at(t->currentStation())->passengers() == 0 || t->passengers() == 6) && !passengersArrived(t->index(), t->currentStation())) {
					t->setState(1);
					//printf("train moving!\n");
				}
		}
	}

	for (auto& p : _passengersVec)
		if (p != 0)
			if (p->trainIndex() != -1) {
				p->setPos(_trainsVec.at(p->trainIndex())->passengerPos(p->ticket()));
				p->moveTransformPoint(_trainsVec.at(p->trainIndex())->position());
				p->setRotation(_trainsVec.at(p->trainIndex())->rotationAngle());
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
			_stationsVec.push_back(station);
			_scene->addItem(_stationsVec.back());
			for(int i = 0; i < MAX_LINES; i++)
				_graph[i].emplace_back();
		}

		for (int i = 0; i < MAX_LINES; i++) {
			Line* tmp = 0;
			_linesVec.push_back(tmp);
			_linesVec.shrink_to_fit();
		}

		for (int i = 0; i < MAX_TRAINS; i++) {
			Train* tmp = 0;
			_trainsVec.push_back(tmp);
			_trainsVec.shrink_to_fit();
		}


		_engine.start();
		_passengerTimer.start();
		_stationsTimer.start();
		_passengersInOutTimer.start();
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
			for (auto& s : _stationsVec)
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
	_stationsVec.push_back(newStation);
	_scene->addItem(_stationsVec.back());
	
	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].emplace_back();

	if (_stationsNumber == MAX_STATIONS)
		_stationsTimer.stop();
}

void Game::passengersInOut(){

	for(auto& t : _trainsVec)
		if(t != 0)
			if (t->state() == 0) {

				auto iter = _passengersVec.begin();

				for (iter; iter < _passengersVec.end(); iter++) {

					// One passengers gets off the train
					if((*iter)->trainIndex() == t->index())
						if ((*iter)->passengerShape() == _stationsVec.at(t->currentStation())->stationShape()) {

							t->decrementPassengers((*iter)->ticket());
							//printf("Score = %d\n", ++_score);
							_scene->removeItem((*iter));
							(*iter)->setVisible(false);
							_passengersVec.erase(iter);
							// memory leak

							break;
						}

					// One passenger gets on the train
					if ((*iter)->stationIndex() == t->currentStation() && t->passengers() < 6) {

						(*iter)->setTicket(t->firstSeatAvailable());
						t->incrementPassengers((*iter)->ticket());
						(*iter)->getOnTrain(t->index());
						(*iter)->setPos(t->passengerPos((*iter)->ticket()));
						(*iter)->setRotation(t->rotationAngle());
						reorgPassengers(t->currentStation());

						break;
					}
				}
			}
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
	} while (_stationsVec.at(index)->passengers() >= MAX_PASS_STATION && !stationsFull);

	if (!stationsFull) {

		QPoint position = passPosStation(index);
		
		int shape;
		do {
			shape = randomShape();
		} while (shape == _stationsVec.at(index)->stationShape());

		_stationsVec.at(index)->addPassenger();

		Passenger* passenger = new Passenger(index, position, shape);
		_passengersVec.push_back(passenger);
		_scene->addItem(_passengersVec.back());
	}
	else printf("YOU DIED\n"); // implement death

	_passengerTimer.setInterval((2000 / sqrt(_stationsNumber)) + ((rand() % 3) - 1)*(rand() % 100));

}

int Game::randomShape(){

	int index = rand() % (_stationsNumber+1);
	return _stationsVec.at(index)->stationShape();
}

void Game::deleteLine(int lineIndex){

	// it crashed with the delete action
	if (lineExists(lineIndex)) {

		for (auto& t : _trainsVec) 
			if (t != 0) 
				if (t->lineIndex() == lineIndex) {
					// if the program doesn't behave correctly, try this
					// t->setPath(QPainterPath());
					t->setVisible(false);
					t = 0;
				}
		
		_scene->removeItem(_linesVec.at(lineIndex));
		// delete _linesList.at(lineIndex);
		_linesVec.at(lineIndex) = 0;
		
	}
}

QPoint Game::passPosStation(int stationIndex)
{
	QPoint position = _stationsVec.at(stationIndex)->position();

	position.setX(position.x() + STATION_SIZE + 5);
	if (_stationsVec.at(stationIndex)->passengers() < MAX_PASS_STATION / 2)
		position.setY(position.y() - PASSENGER_SIZE - 1);
	else position.setY(position.y() + 1);

	position.setX(position.x() + (_stationsVec.at(stationIndex)->passengers() % (MAX_PASS_STATION / 2)) * (2 + PASSENGER_SIZE));
	return position;
}

void Game::reorgPassengers(int stationIndex){

	_stationsVec.at(stationIndex)->removePassengers();

	for (auto& p : _passengersVec) {
		if (p->stationIndex() == stationIndex) {

			p->setPos(passPosStation(stationIndex));
			_stationsVec.at(stationIndex)->addPassenger();
		}
	}
}

bool Game::passengersArrived(int TrainIndex, int StationIndex){

	for (auto& p : _passengersVec) {
		if (p->trainIndex() == TrainIndex)
			if (_stationsVec.at(StationIndex)->stationShape() == p->passengerShape())
				return true;
	}

	return false;
}

int Game::nearestStation(QPoint trainPos){

	int nearestStation = -1;
	float dist = std::numeric_limits<float>::max();
	int i = 0;

	for (auto& s : _stationsVec) {

		if (distance(trainPos, s->position()) < dist) {
			dist = distance(trainPos, s->position());
			nearestStation = i;
		}
		i++;
	}

	return nearestStation;
}

int Game::nextStation(int lineIndex, int stationIndex, int trainIndex){

	int index = -1;

	for (int i = 0; i < _linesVec.at(lineIndex)->size(); i++) {

		if (_linesVec.at(lineIndex)->stationPoint(i) == _stationsVec.at(stationIndex)->centerPos()) {

			if (_trainsVec.at(trainIndex)->state() == 0) // FORWARD
				if (i != _linesVec.at(lineIndex)->size() - 1)
					index = (i + 1);
				else index = (i - 1);
			if (_trainsVec.at(trainIndex)->state() == 1) // BACKWARD
				if (i != 0)
					index = (i - 1);
				else index = (i + 1);
			if (_trainsVec.at(trainIndex)->circular()) // CIRCULAR
				if (i != _linesVec.at(lineIndex)->size() - 1)
					index = (i + 1);
				else index = 1;
		}

	}

	for (int i = 0; i < _stationsVec.size(); i++)
		if (_stationsVec.at(i)->centerPos() == _linesVec.at(lineIndex)->stationPoint(index))
			return i;

	return -1;

}

bool Game::trainArrived(int trainIndex){

	auto t = _trainsVec.at(trainIndex);
	QPoint trainPos = t->position();
	QPoint nextStationPos = _stationsVec.at(t->nextStation())->centerPos();

	if (distance(trainPos, nextStationPos) > t->distanceFromStation()) {

		t->setCurrentStation(t->nextStation());
		t->setNextStation(nextStation(t->lineIndex(), t->currentStation(), t->direction()));
		printf("current station: %d\nnext station: %d\n", t->currentStation(), t->nextStation());
		t->setDistanceFromStation(std::numeric_limits<float>::max());
		return true;
	}
	else t->setDistanceFromStation(distance(trainPos, nextStationPos));

	return false;
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
		for (auto& t : _trainsVec)
			if(t != 0)
				std::cout << "Train " << t->index() << " on line "
					<< t->lineIndex() << "\n";

	}
	if (e->key() == Qt::Key_D && _state == RUNNING) {
		bool visible = true;
		if (_stationsVec.at(0)->isVisible())
			visible = false;
		for (auto& s : _stationsVec)
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
		for (auto& l : _linesVec) {
			if (!l) {
				_activeLine = i;
				break;
			}
			i++;
		}
		//printf("active line: %d\n", _activeLine);

		if (_activeLine != -1)
			for (auto& s : _stationsVec) {

				if (s->pointerOnStation(e->pos())) {

					QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
									   s->position().y() + STATION_SIZE / 2);

					_linesVec.at(_activeLine) = new Line(centerPoint, _activeLine);
					_mousePressed = true;
					_scene->addItem(_linesVec.at(_activeLine));
					_activeStation = s->index();

					break;
				}
			}
	}

	// Edit Line
	int i = 0;
	for (auto& l : _linesVec) {

		if(l != 0)
			if (l->pointerOnCap(e->pos()) && !l->circularLine()) {
				_mousePressed = true;
				_activeLine = i;

				if (l->state() == 2) {
					for (auto& s : _stationsVec)
						if (s->centerPos() == l->firstPoint()) {
							_activeStation = s->index();
							break;
						}
				} 
				else
					for (auto& s : _stationsVec)
						if (s->centerPos() == l->lastPoint()) {
							_activeStation = s->index();
							break;
						}
			}
		i++;
	}

	QGraphicsView::mousePressEvent(e);
}

void Game::mouseMoveEvent(QMouseEvent* e){
	
	if (_mousePressed) {
		QPoint currentPoint(e->pos().x() / GAME_SCALE,
							e->pos().y() / GAME_SCALE);
		_linesVec.at(_activeLine)->setCurrentPoint(currentPoint);

		for (auto& s : _stationsVec) {

			if (s->pointerOnStation(e->pos())) {

				QPoint centerPoint(s->position().x() + STATION_SIZE / 2,
								   s->position().y() + STATION_SIZE / 2);

				if (_linesVec.at(_activeLine)->validPoint(centerPoint)) {
					_linesVec.at(_activeLine)->setNextPoint(centerPoint);

					_graph[_activeLine].at(_activeStation).push_back(s->index());
					_graph[_activeLine].at(s->index()).push_back(_activeStation);
					_activeStation = s->index();

					if (_linesVec.at(_activeLine)->circularLine()) {
						
						_linesVec.at(_activeLine)->updateTcapPoint();
						
						bool hasTrain = false;
						int id = 0;

						for (auto& t : _trainsVec)
							if (t != 0) {
								if (t->lineIndex() == _activeLine) {
									t->setPath(_linesVec.at(_activeLine)->path());
									t->setCircular(true);
									hasTrain = true;
								}
							}

						if (!hasTrain) {
							for (auto& t : _trainsVec) {
								if (t == 0) {

									t = new Train(_activeLine, id,
												  _linesVec.at(_activeLine)->firstPoint(),
												  _linesVec.at(_activeLine)->path(), 
												  nearestStation(_linesVec.at(_activeLine)->firstPoint()));
									t->setCircular(true);
									t->setNextStation(nextStation(_activeLine, t->currentStation(), t->direction()));

									
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

		if (_linesVec.at(_activeLine)->size() < 2) {
			_scene->removeItem(_linesVec.at(_activeLine));
			//delete _linesList.at(_activeLine);
			_linesVec.at(_activeLine) = 0;
		}

		if (lineExists(_activeLine)){

			_linesVec.at(_activeLine)->updateTcapPoint();
			
			bool hasTrain = false;
			int id = 0;

			for (auto& t : _trainsVec)
				if (t != 0) {
					if (t->lineIndex() == _activeLine) {
						t->setPath(_linesVec.at(_activeLine)->path());
						hasTrain = true;
					}
				}

			if (!hasTrain) {
				for (auto& t : _trainsVec) {
					if (t == 0) {

						t = new Train(_activeLine, id,
							_linesVec.at(_activeLine)->firstPoint(),
							_linesVec.at(_activeLine)->path(),
							nearestStation(_linesVec.at(_activeLine)->firstPoint()));
						t->setNextStation(nextStation(_activeLine, t->currentStation(), t->direction()));

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
