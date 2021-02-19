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
	setRenderHints(QPainter::Antialiasing
				 | QPainter::SmoothPixmapTransform);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//showFullScreen();

	this->reset();
	
	scale(GAME_SCALE, GAME_SCALE);


	QObject::connect(&_engine, SIGNAL(timeout()), this, SLOT(advance()));
	//QObject::connect(&_passengerTimer, SIGNAL(timeout()), this, SLOT(spawnPassenger()));
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
		QGraphicsPixmapItem *_mainMenu = new QGraphicsPixmapItem(QPixmap(":/Graphics/MainMenu.png"));
		QGraphicsPixmapItem *_newGame = new QGraphicsPixmapItem(QPixmap(":/Graphics/Button_New_Game.png"));
		QGraphicsPixmapItem *_continue = new QGraphicsPixmapItem(QPixmap(":/Graphics/Button_Continue.png"));
		QGraphicsPixmapItem* _exit = new QGraphicsPixmapItem(QPixmap(":/Graphics/Button_Exit.png"));

		_newGame->setPos(WINDOW_WIDTH/2 - 250, WINDOW_HEIGHT/2 - 45 - 90 - 5);
		_continue->setPos(WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT/2 - 45);
		_exit->setPos(WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT/2 - 45 + 90 + 5);
		_scene->addItem(_mainMenu); 
		_scene->addItem(_newGame);
		_scene->addItem(_continue);
		_scene->addItem(_exit);
		
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

	AI::instance()->clearGraph();

	if(_scoreText)
		delete _scoreText;

	_stationsNumber = -1;
	_activeStation = -1;
	_activeLine = -1;
	_score = 0;

	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);

	_passengersInOutTimer.setTimerType(Qt::PreciseTimer);

	_stationsTimer.setInterval(10000);
	_passengerTimer.setInterval(2000);
	_passengersInOutTimer.setInterval(500);
	_state = READY;
	_debug = false;
	_fpsMultiplier = 1;

	this->init();
}

void Game::advance() {

	
	// Lines
	for (auto& l : _linesVec) {
		bool deleteLine = true;
		if (l)
			if (l->deleting()) {
				for (auto& t : _trainsVec)
					if(t)
						if (t->passengers() != 0 && _linesVec.at(t->lineIndex()) == l) {
							deleteLine = false;
							break;
						}
				if (deleteLine) {
					for (auto& t : _trainsVec)
						if (t)
							if (_linesVec.at(t->lineIndex()) == l) {
								t->setVisible(false);
								t = 0;
							}
					_scene->removeItem(l);
					l = 0;
				}
			}
	}

	// Trains movement
	for (auto& t : _trainsVec)
		if (t != 0)
			t->advance();


	// Passengers get on trains
	for (auto& t : _trainsVec) {
		if (t != 0) {
			if (trainArrived(t->index())) {

				// Train stops under normal circumstances
				if ((_stationsVec.at(t->currentStation())->passengers() > 0) || passengersArrived(t->index(), t->currentStation()))
					t->setState(0);

				// Train stops because the line will be deleted
				if (_linesVec.at(t->lineIndex())->deleting())
					t->setState(0);

				}
			if(t->state() == 0)
				if ((_stationsVec.at(t->currentStation())->passengers() == 0 || t->passengers() == 6) && !passengersArrived(t->index(), t->currentStation()) && !_linesVec.at(t->lineIndex())->deleting()) {
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

		_scene->clear();
		QGraphicsPixmapItem* Map = new QGraphicsPixmapItem(QPixmap(":/Graphics/LondonMap.png"));
		_scene->addItem(Map);
		
		_scoreText = new QGraphicsTextItem;
		_scoreText->setPlainText(QString("Score: ") + QString::number(_score));
		_scoreText->setScale(3);
		_scene->addItem(_scoreText);

		for (int i = -1; i < MAX_LINES; i++){
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
			AI::instance()->addStation();
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

		AI::instance();

		/* EVENT FILTER
		button = new QPushButton();
		button->setIcon(QIcon(QPixmap(":/Graphics/Button_Exit.png")));
		button->setFlat(true);
		button->setBaseSize(500, 90);
		button->setIconSize(QSize(500, 90));
		button->installEventFilter(this);
		_scene->addWidget(button);
		*/
		
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
	AI::instance()->addStation();

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
					if ((*iter)->trainIndex() == t->index()) {

						// if the line is being deleted
						if (_linesVec.at(t->lineIndex())->deleting() || 
							AI::instance()->nextStationInShortestPath(t->currentStation(), (*iter)->finalStation()) != t->nextStation()) {

							t->decrementPassengers((*iter)->ticket());
							(*iter)->getOffTrain(t->currentStation());
							(*iter)->setPos(passPosStation(t->currentStation()));
							(*iter)->setRotation(0);

							if ((*iter)->passengerShape() == _stationsVec.at(t->currentStation())->stationShape()) {

								//printf("Score = %d\n", ++_score);
								_score++;
								_scoreText->setPlainText(QString("Score: ") + QString::number(_score));
								_scene->removeItem((*iter));
								(*iter)->setVisible(false);
								_passengersVec.erase(iter);
							}
							else 
								_stationsVec.at(t->currentStation())->addPassenger();

							break;
						}

					}

					// One passenger gets on the train
					if ((*iter)->stationIndex() == t->currentStation() && 
						t->passengers() < 6 && 
						!_linesVec.at(t->lineIndex())->deleting() &&
						AI::instance()->nextStationInShortestPath(t->currentStation(), (*iter)->finalStation()) == t->nextStation())
					{
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

	srand(time(NULL));

	for (auto& s : _stationsVec) {
		
		if ((rand() % 100) <= SPAWN_CHANCE && s->passengers() < 10) {

			QPoint position = passPosStation(s->index());

			int shape;
			do {
				shape = randomShape();
			} while (shape == _stationsVec.at(s->index())->stationShape());

			_stationsVec.at(s->index())->addPassenger();

			printf("Station: %d, shape: %d\n", s->index(), shape);

			Passenger* passenger = new Passenger(s->index(), position, shape);
			_passengersVec.push_back(passenger);
			_scene->addItem(_passengersVec.back());

		}

	}

}

int Game::randomShape(){

	int index = rand() % (_stationsNumber+1);
	return _stationsVec.at(index)->stationShape();
}

void Game::deleteLine(int lineIndex){

	if (lineExists(lineIndex))
		_linesVec.at(lineIndex)->setDeleting(true);

		/*
		for (auto& t : _trainsVec) 
			if (t != 0) 
				if (t->lineIndex() == lineIndex) {
					t->setVisible(false);
					t = 0;
				}
		
		_scene->removeItem(_linesVec.at(lineIndex));
		_linesVec.at(lineIndex) = 0;
		*/
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

			if (_trainsVec.at(trainIndex)->direction() == 0) { // TRAIN MOVING FORWARD
				
				index = i + 1;

				if(i == _linesVec.at(lineIndex)->size() - 1) // TRAIN ON LAST STATION?
					index = i - 1;

				if (_trainsVec.at(trainIndex)->circular()) // CIRCULAR LINE?
					if (i == _linesVec.at(lineIndex)->size() - 1)
						index = 1;

			}
			else { // TRAIN MOVING BACKWARD

				index = i - 1;

				if (i == 0) // TRAIN ON FIRST STATION?
					index = i + 1;

				if (_trainsVec.at(trainIndex)->circular()) // CIRCULAR LINE?
					if (i == 0)
						index = _linesVec.at(lineIndex)->size() - 2;
			}
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
		t->setNextStation(nextStation(t->lineIndex(), t->currentStation(), t->index()));
		// printf("current station: %d\nnext station: %d\n", t->currentStation(), t->nextStation());
		t->setDistanceFromStation(std::numeric_limits<float>::max());
		return true;
	}
	else t->setDistanceFromStation(distance(trainPos, nextStationPos));

	return false;
}

void Game::addTrain(QRect rect){
	
	int lineIndex = -1;
	int i = 0;

	for (auto& l : _linesVec) {

		if (l)
			if (l->pathColliding(rect)) {
				lineIndex = i;
				break;
			}
		i++;
	}

	i = 0;

	if (lineIndex != -1) {
		for (auto& t : _trainsVec) {
			if (!t) {
				t = new Train(lineIndex, i, 
					_linesVec.at(lineIndex)->firstPoint(),
					_linesVec.at(lineIndex)->path(),
					nearestStation(_linesVec.at(lineIndex)->firstPoint()));
				_scene->addItem(t);
				break;
			}
			i++;
		}
	}

}

bool Game::availableTrains(){

	if (_trainsVec.at(MAX_TRAINS - 1) == 0)
		return true;
	else 
		return false;
}

/*
bool Game::eventFilter(QObject* watched, QEvent* event){

	if (watched == button) {
		if (event->type() == QEvent::HoverEnter) {
			button->resize(500 * 0.5, 90 * 0.5);
			button->setIconSize(QSize(500 * 0.5, 90 * 0.5));
			button->setGeometry(125, 22, 250, 45);
			return true;
		}
		if (event->type() == QEvent::HoverLeave) {
			button->resize(500 , 90);
			button->setIconSize(QSize(500, 90));
			button->setGeometry(0, 0, 500, 22);
			return true;
		}
	}
	return false;
}
*/

bool Game::loadGame() {

	QFile loadFile(QStringLiteral("save.json"));

	if (!loadFile.open(QIODevice::ReadOnly)) {
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	read(loadDoc.object());
	return true;
}

bool Game::saveGame() const {

	QFile saveFile(QStringLiteral("save.json"));

	if (!saveFile.open(QIODevice::WriteOnly)) {
		qWarning("Couldn't open save file.");
		return false;
	}

	QJsonObject gameObject;
	write(gameObject);
	saveFile.write(QJsonDocument(gameObject).toJson());
	return true;
}

void Game::read(const QJsonObject& json){

	// Load stations
	if (json.contains("Stations") && json["Stations"].isArray()) {
		QJsonArray stationsArray = json["Stations"].toArray();
		_stationsVec.clear();

		for (int i = 0; i < stationsArray.size(); i++) {
			QJsonObject stationObj = stationsArray[i].toObject();
			Station *s = new Station(QPoint(0, 0), -1);
			s->read(stationObj);
			_stationsVec.push_back(s);
			_scene->addItem(s);
		}
	}

	// Load Game variables
	if (json.contains("Active line") && json["Active line"].isDouble())
		_activeLine = json["Active line"].toInt();

	if (json.contains("Active Station") && json["Active Station"].isDouble())
		_activeStation = json["Active Station"].toInt();

	if (json.contains("Score") && json["Score"].isDouble())
		_score = json["Score"].toInt();

	if (json.contains("Stations number") && json["Stations number"].isDouble())
		_stationsNumber = json["Stations number"].toInt();

	// Load the graph

	for (int i = 0; i < MAX_LINES; i++)
		_graph[i].clear();

	for (int j = 0; j < MAX_LINES; j++)
		for(int i = 0; i <= _stationsNumber; i++)
			_graph[j].emplace_back();

	if (json.contains("Graph") && json["Graph"].isArray()) {

		QJsonArray arr = json["Graph"].toArray();

		for (int i = 0; i < MAX_LINES; i++) {

			QJsonArray arr2 = arr.at(i).toArray();

			for (int j = 0; j <= _stationsNumber; j++) {
				
				QJsonObject obj = arr2.at(j).toObject();
				int index = -1;
				if (obj.contains("First adjacent station") && obj["First adjacent station"].isDouble()) {
					index = obj["First adjacent station"].toInt();
					_graph[i].at(j).push_back(index);
				}

				if (obj.contains("Second adjacent station") && obj["Second adjacent station"].isDouble()){
					index = obj["Second adjacent station"].toInt();
					_graph[i].at(j).push_back(index);
				}
			}
		}
	}

	// Load the lines and add trains
	if (json.contains("Lines") && json["Lines"].isArray()) {
		QJsonArray linesArray = json["Lines"].toArray();

		for (int i = 0; i < linesArray.size(); i++) {
			QJsonObject lineObj = linesArray[i].toObject();

			_linesVec.at(i) = new Line(QPoint(0, 0), i);
			_linesVec.at(i)->read(lineObj);

			int id = 0;
			for (auto& t : _trainsVec){
				if (!t) {
					t = new Train(i, id,
						_linesVec.at(i)->firstPoint(),
						_linesVec.at(i)->path(),
						nearestStation(_linesVec.at(i)->firstPoint()));
					if (_linesVec.at(i)->circularLine())
						t->setCircular(true);
					_scene->addItem(t);
					break;
				}
				id++;
			}

			_scene->addItem(_linesVec.at(i));
		}

	}
}

void Game::write(QJsonObject& json) const{

	// Save Game variables
	json["Active line"] = _activeLine;
	json["Active Station"] = _activeStation;
	json["Score"] = int(_score);
	json["Stations number"] = _stationsNumber;

	// Save stations array
	QJsonArray stationsArray;
	for (auto& s : _stationsVec) {
		QJsonObject stationObj;
		s->write(stationObj);
		stationsArray.append(stationObj);
	}
	json["Stations"] = stationsArray;
	
	// Save Graph
	QJsonArray graphArray;
	QJsonArray graphStationsArray[300];
	
	for (int i = 0; i < MAX_LINES; i++) {
		for (int j = 0; j <= _stationsNumber; j++) {
			QJsonObject obj;

			if (!_graph[i].at(j).empty())
				obj["First adjacent station"] = _graph[i].at(j).front();
			if (_graph[i].at(j).size() == 2)
				obj["Second adjacent station"] = _graph[i].at(j).back();

			graphStationsArray[i].append(obj);
		}
		graphArray.append(graphStationsArray[i]);
	}
	json["Graph"] = graphArray;

	// Save Lines
	QJsonArray linesArray;
	for (auto& l : _linesVec) {
		if (l) {
			QJsonObject lineObj;
			l->write(lineObj);
			linesArray.append(lineObj);
		}
	}
	json["Lines"] = linesArray;

}

void Game::keyPressEvent(QKeyEvent* e){

	// resets game
	if (e->key() == Qt::Key_R) {

		reset();
		init();
	}

	// starts new game
	if (e->key() == Qt::Key_S) {

		start();
	}

	if (e->key() == Qt::Key_P) {

		if (_state == RUNNING) {
			_state = PAUSED;
			_engine.stop();
			_passengerTimer.stop();
			_stationsTimer.stop();
			_passengersInOutTimer.stop();
		}
		else {
			_state = RUNNING;
			_engine.start();
			_passengerTimer.start();
			_stationsTimer.start();
			_passengersInOutTimer.start();
		}
	}

	if (e->key() == Qt::Key_G && _debug) {

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
	if (e->key() == Qt::Key_D) {

		if (_debug)
			_debug = false;
		else {
			_debug = true;
			printf("------------ Debug Commands ------------\n");
			printf("  V KEY - Toggle stations visibility\n");
			printf("  G KEY - Print game graph\n");
			printf("  UP ARROW - increase game speed\n");
			printf("  DOWN ARROW - decrease game speed\n");
			printf("  RIGHT ARROW - reset game speed\n");
		}

	}

	if (e->key() == Qt::Key_V && _debug) {

		bool visible = true;
		if (_stationsVec.at(0)->isVisible())
			visible = false;
		for (auto& s : _stationsVec)
			s->setVisible(visible);
	}

	if (e->key() == Qt::Key_K ) {

		saveGame();
	}

	if (e->key() == Qt::Key_L) {

		loadGame();
	}

	if (e->key() == Qt::Key_O) {

		AI::instance()->printGraph();
	}

	if (e->key() == Qt::Key_Down && _debug) {

		if (1000 / (GAME_FPS * _fpsMultiplier - 0.1) >= 0) {
			_fpsMultiplier -= 0.1;
			_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
			_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
			_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
			_stationsTimer.setInterval(10000 / _fpsMultiplier);

		}
	}

	if (e->key() == Qt::Key_Up && _debug) {

		_fpsMultiplier += 0.1;
		_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
		_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
		_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
		_stationsTimer.setInterval(10000 / _fpsMultiplier);

	}

	if (e->key() == Qt::Key_Right && _debug) {

		_fpsMultiplier = 1;
		_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
		_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
		_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
		_stationsTimer.setInterval(10000 / _fpsMultiplier);

	}
}

void Game::mousePressEvent(QMouseEvent* e){

	if (_state == READY) {

		if (QRect(QPoint(WINDOW_WIDTH / 2 - 250 + 92, WINDOW_HEIGHT / 2 - 45 - 90 - 5),
			QPoint(WINDOW_WIDTH / 2 - 250 + 500 - 48, WINDOW_HEIGHT / 2 - 45 - 90 - 5 + 90)).contains(e->pos() / GAME_SCALE))
			start();
		else if (false)
			;
		else if (QRect(QPoint(WINDOW_WIDTH / 2 - 250 + 92, WINDOW_HEIGHT / 2 - 45 + 90 + 5),
				 QPoint(WINDOW_WIDTH / 2 - 250 + 229, WINDOW_HEIGHT / 2 - 45 + 90 + 5 + 90)).contains(e->pos() / GAME_SCALE))
			QApplication::quit();
	}

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
					AI::instance()->setAdjacentStation(_activeStation, s->index());
					AI::instance()->update();

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
									//t->setNextStation(nextStation(_activeLine, t->currentStation(), t->index()));
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
									//t->setNextStation(nextStation(_activeLine, t->currentStation(), t->index()));

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

	_scene->update();
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
						//t->setNextStation(nextStation(_activeLine, t->currentStation(), t->index()));

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
	_scene->update();

	QGraphicsView::mouseReleaseEvent(e);
}
