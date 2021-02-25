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
	_scene->setSceneRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	setInteractive(true);
	setMouseTracking(true);
	setRenderHints(QPainter::Antialiasing
				 | QPainter::SmoothPixmapTransform);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//showFullScreen();

	this->reset();
	
	scale(GAME_SCALE, GAME_SCALE);


	QObject::connect(&_engine, SIGNAL(timeout()), this, SLOT(advance()));
	QObject::connect(&_passengerTimer, SIGNAL(timeout()), this, SLOT(spawnPassenger()));
	QObject::connect(&_stationsTimer, SIGNAL(timeout()), this, SLOT(spawnStation()));
	QObject::connect(&_passengersInOutTimer, SIGNAL(timeout()), this, SLOT(passengersInOut()));

}

void Game::init() {

	if (_state == READY)
	{
		_menuScene = new QGraphicsScene;
		_menuScene->setSceneRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		_menuScene->clear();
		setScene(_menuScene);

		// set window dimensions
		setFixedWidth(GAME_SCALE * _menuScene->width());
		setFixedHeight(GAME_SCALE * _menuScene->height() + 2);

		QGraphicsPixmapItem* _mainMenu = new QGraphicsPixmapItem(QPixmap(":/Graphics/MainMenu.png"));
		_mainMenu->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
		_menuScene->addItem(_mainMenu);
		QGraphicsPixmapItem* _arrows = new QGraphicsPixmapItem(QPixmap(":/Graphics/arrows.png"));
		_arrows->setPos(740, 400);
		_arrows->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
		_menuScene->addItem(_arrows);

		QPushButton* _newGameButton = new QPushButton("New Game");
		QPushButton* _continueButton = new QPushButton("Continue");
		QPushButton* _exitButton = new QPushButton("Exit");

		_newGameButton->setStyleSheet({ "QPushButton{height: 90px; width: 300px; text-align: center; background: #538cbd; color:#f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{color: #f0f0f0; background: #336fa2;}" });
		_continueButton->setStyleSheet({ "QPushButton{height: 90px; width: 300px; text-align: center; background: #538cbd; color:#f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{color: #f0f0f0; background: #336fa2;}" });
		_exitButton->setStyleSheet({ "QPushButton{height: 90px; width: 300px; text-align: center; background: #538cbd; color:#f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{color: #f0f0f0; background: #336fa2;}" });
		
		_newGameButton->setGeometry(840, 400, 300, 90);
		_continueButton->setGeometry(840, 510, 300, 90);
		_exitButton->setGeometry(840, 620, 300, 90);

		_menuScene->addWidget(_newGameButton);
		_menuScene->addWidget(_continueButton);
		_menuScene->addWidget(_exitButton);

		QObject::connect(_newGameButton, SIGNAL(clicked()), this, SLOT(start()));
		QObject::connect(_continueButton, SIGNAL(clicked()), this, SLOT(loadGame()));
		QObject::connect(_exitButton, SIGNAL(clicked()), this, SLOT(exitGame()));

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

	AI::instance()->clearGraph();

	AI::instance()->clearBigGraph();

	if(_scoreText)
		delete _scoreText;

	_stationsNumber = -1;
	_activeStation = -1;
	_activeLine = -1;
	_score = 0;

	_engine.setInterval(1000 / GAME_FPS);
	_engine.setTimerType(Qt::PreciseTimer);

	_passengersInOutTimer.setTimerType(Qt::PreciseTimer);

	_stationsTimer.setInterval(1000);
	_passengerTimer.setInterval(2000);
	_passengersInOutTimer.setInterval(500);
	_state = READY;
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
					if (t)
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

					AI::instance()->deleteGraphLine(l->nome());
					_scene->removeItem(l);
					l = 0;
				}
			}
	}

	// Trains movement
	for (auto& t : _trainsVec)
		if (t != 0) {
			if (t->deleting() && t->passengers() == 0) {
				t->setVisible(false);
				t = 0;
			}
			else
				t->advance();
		}


	// Passengers get on trains
	for (auto& t : _trainsVec) {
		if (t != 0) {
			if (trainArrived(t->index())) {

				// Train stops under normal circumstances
				if (passengersGetOn(t->index(), t->currentStation()) || passengersArrived(t->index(), t->currentStation()))
					t->setState(0);

				// Train stops because the line will be deleted
				if (_linesVec.at(t->lineIndex())->deleting() || t->deleting())
					t->setState(0);
			}

			if (t->state() == 0)
				if ((t->passengers() == 6 && !passengersArrived(t->index(), t->currentStation()) && (!_linesVec.at(t->lineIndex())->deleting()) && !t->deleting()) ||
					(!passengersArrived(t->index(), t->currentStation()) &&
						!passengersGetOn(t->index(), t->currentStation())) &&
					(!_linesVec.at(t->lineIndex())->deleting() && !t->deleting())) {

					t->setState(1);
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

		AI::instance();

		_scene->clear();
		setScene(_scene);

		QGraphicsPixmapItem* Map = new QGraphicsPixmapItem(QPixmap(":/Graphics/LondonMap.png"));
		Map->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
		_scene->addItem(Map);
		
		_scoreText = new QGraphicsTextItem;
		QFont font("Comfortaa");
		font.setPointSizeF(24);
		_scoreText->setFont(font);
		_scoreText->setPlainText(QString("Score: ") + QString::number(_score));
		_scoreText->setPos(15, 0);
		_scene->addItem(_scoreText);

		_backToMenuButton = new QPushButton;
		_backToMenuButton->setStyleSheet({ "QPushButton{height: 60px; width: 60px; text-align: center; background: #f0f0f0; color: #f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{}" });
		_backToMenuButton->setIcon(QPixmap(":/Graphics/backToMenu.png"));
		_backToMenuButton->setIconSize(QSize(45, 45));
		_backToMenuButton->setGeometry(1853, 15, 45, 45);
		_scene->addWidget(_backToMenuButton);

		_saveButton = new QPushButton;
		_saveButton->setStyleSheet({ "QPushButton{height: 60px; width: 60px; text-align: center; background: #f0f0f0; color: #f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{}" });
		_saveButton->setIcon(QPixmap(":/Graphics/saveIcon.png"));
		_saveButton->setIconSize(QSize(45, 45));
		_saveButton->setGeometry(1860, 75, 45, 45);
		_scene->addWidget(_saveButton);

		_pauseButton = new QPushButton;
		_pauseButton->setStyleSheet({ "QPushButton{height: 60px; width: 60px; text-align: center; background: #f0f0f0; color: #f0f0f0; font-family:'Comfortaa'; font-size:40px; font-weight: bold; border: 0px;}"
									   "QPushButton:hover{}" });
		_pauseButton->setIcon(QPixmap(":/Graphics/pauseButton.png"));
		_pauseButton->setIconSize(QSize(45, 45));
		_pauseButton->setGeometry(1860, 135, 45, 45);
		_scene->addWidget(_pauseButton);

		QObject::connect(_backToMenuButton, SIGNAL(clicked()), this, SLOT(reset()));
		QObject::connect(_saveButton, SIGNAL(clicked()), this, SLOT(saveGame()));
		QObject::connect(_pauseButton, SIGNAL(clicked()), this, SLOT(togglePause()));

		QPixmap trashBinPixmap(":/Graphics/trashBin.png");
		trashBinPixmap = trashBinPixmap.scaledToHeight(60, Qt::TransformationMode::SmoothTransformation);
		_trashBin = new QGraphicsPixmapItem(trashBinPixmap);
		_trashBin->setPos(QPoint(1250, 990));
		_scene->addItem(_trashBin);

		for (int i = -1; i < MAX_LINES; i++){
			_deleteButtons.push_back(new Button(i));
			_scene->addItem(_deleteButtons.back());
		}

		for (int i = 0; i < 3; i++) {
			_stationsNumber++;
			Station* station = new Station(startPos[i], _stationsNumber, i);
			_stationsVec.push_back(station);
			_scene->addItem(_stationsVec.back());

			AI::instance()->addStation();
			AI::instance()->update();
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

		_menuScene->deleteLater();

		_engine.start();
		_passengerTimer.start();
		_stationsTimer.start();
		_passengersInOutTimer.start();
		_state = RUNNING;
		printf("Game started\n");
	}
}

void Game::spawnStation() {

	QPoint spawnPoint = spawnPos();

	bool found = false;

	do {

		if (spawnAreaAvailable(spawnPoint, _stationsNumber))
			for (auto& s : _stationsVec)
				if (distance(spawnPoint, s->position()) < STATION_SIZE * 6) {

					spawnPoint = spawnPos();
					// printf("cambiate coordinate\n");
					found = true;
					break;
				}
				else
					found = false;
			
		else {
			spawnPoint = spawnPos();
			found = true;
		}
		} while (found);

	_stationsNumber++;
	Station* newStation = new Station(spawnPoint, _stationsNumber);
	_stationsVec.push_back(newStation);
	_scene->addItem(_stationsVec.back());
	
	AI::instance()->addStation();
	AI::instance()->update();

	if (_stationsNumber >= MAX_STATIONS)
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

						// He is arrived at his destination
						if ((*iter)->passengerShape() == _stationsVec.at(t->currentStation())->stationShape()) {

							t->decrementPassengers((*iter)->ticket());
							(*iter)->getOffTrain(t->currentStation());
							(*iter)->setPos(passPosStation(t->currentStation()));
							(*iter)->setRotation(0);
							_score++;
							_scoreText->setPlainText(QString("Score: ") + QString::number(_score));
							_scene->removeItem((*iter));
							(*iter)->setVisible(false);
							_passengersVec.erase(iter);
							
							break;
						}

						// if the line or the train are being deleted
						if ((_linesVec.at(t->lineIndex())->deleting() || t->deleting()) || 
							!acceptableStation(t, *iter)) {

							t->decrementPassengers((*iter)->ticket());
							(*iter)->getOffTrain(t->currentStation());
							(*iter)->setPos(passPosStation(t->currentStation()));
							(*iter)->setRotation(0);
							_stationsVec.at(t->currentStation())->addPassenger();

							break;
						}

					}

					// One passenger gets on the train
					if ((*iter)->stationIndex() == t->currentStation())
						if(t->passengers() < 6 && 
							(!_linesVec.at(t->lineIndex())->deleting() && !t->deleting()) &&
							acceptableStation(t, *iter)){

							//printf("Next passenger station: %d\n", AI::instance()->nextStationInShortestPath(t->currentStation(), (*iter)->finalStation()));
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

			//printf("Station: %d, shape: %d\n", s->index(), shape);

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
			if (!acceptableStation(_trainsVec.at(TrainIndex), p))
				return true;
	}

	return false;
}

bool Game::passengersGetOn(int TrainIndex, int StationIndex){

	for (auto& p : _passengersVec) {
		if (p->stationIndex() == StationIndex)
			if (acceptableStation(_trainsVec.at(TrainIndex), p))
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
				if (_linesVec.at(lineIndex)->circularLine())
					t->setCircular(true);
				_scene->addItem(t);
				break;
			}
			i++;
		}
		if (_linesVec.at(lineIndex)->circularLine())
			for (auto& t : _trainsVec) {
				if (t)
					if (t->direction() == 1) { // BACKWARD
						AI::instance()->setOrientation(false, nearestStation(_linesVec.at(lineIndex)->firstPoint()), t);
						break;
					}
			}

	}

}

bool Game::availableTrains(){

	if (_trainsVec.at(MAX_TRAINS - 1) == 0)
		return true;
	else 
		return false;
}

void Game::updatePassengersDestinations(){

	for (auto& p : _passengersVec)
		if(p->stationIndex() != -1)
			p->updateFinalStations();

}

bool Game::acceptableStation(Train* t, Passenger* p){

	for (auto& finalSt : p->finalStations())
		if (AI::instance()->nextStationInShortestPath(t->currentStation(), finalSt) == t->nextStation())
			return true;

	return false;

}

void Game::togglePause(){

	if (_state == RUNNING) {
		_state = PAUSED;
		_engine.stop();
		_passengerTimer.stop();
		_stationsTimer.stop();
		_passengersInOutTimer.stop();
		_pauseButton->setIcon(QPixmap(":/Graphics/playButton.png"));
	}
	else {
		_state = RUNNING;
		_engine.start();
		_passengerTimer.start();
		if (!(_stationsNumber >= MAX_STATIONS))
			_stationsTimer.start();
		_passengersInOutTimer.start();
		_pauseButton->setIcon(QPixmap(":/Graphics/pauseButton.png"));
	}

}

bool Game::loadGame() {

	start();

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
		
		for (auto& s : _stationsVec)
			delete s;
		_stationsVec.clear();

		for (int i = 0; i < stationsArray.size(); i++) {
			QJsonObject stationObj = stationsArray[i].toObject();
			Station *s = new Station(QPoint(0, 0), -1);
			s->read(stationObj);
			s->setVisible(true);
			_stationsVec.push_back(s);
			_scene->addItem(s);
		}
	}

	// Load Game variables
	if (json.contains("Active line") && json["Active line"].isDouble())
		_activeLine = json["Active line"].toInt();

	if (json.contains("Active Station") && json["Active Station"].isDouble())
		_activeStation = json["Active Station"].toInt();

	if (json.contains("Stations number") && json["Stations number"].isDouble())
		_stationsNumber = json["Stations number"].toInt();

	if (_stationsNumber >= MAX_STATIONS)
		_stationsTimer.stop();

	// Load the graph
	AI::instance()->read(json);
	AI::instance()->update();

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
	AI::instance()->write(json);

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
	if (e->key() == Qt::Key_R && _state == RUNNING) {

		reset();
		init();
	}

	// starts new game
	if (e->key() == Qt::Key_S) {

		start();
	}

	if (e->key() == Qt::Key_P) {

		togglePause();
	}

	if (e->key() == Qt::Key_G && DEBUG) {

		AI::instance()->printGraph();

		std::cout << "---------- Trains ----------\n";
		for (auto& t : _trainsVec)
			if(t != 0)
				std::cout << "Train " << t->index() << " on line "
					<< t->lineIndex() << "\n";

	}
	if (e->key() == Qt::Key_D && DEBUG) {

			printf("------------ Debug Commands ------------\n");
			printf("  V KEY - Toggle stations visibility\n");
			printf("  G KEY - Print game graph\n");
			printf("  UP ARROW - increase game speed\n");
			printf("  DOWN ARROW - decrease game speed\n");
			printf("  RIGHT ARROW - reset game speed\n");
	}

	if (e->key() == Qt::Key_V && DEBUG) {

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

		AI::instance()->printBigGraph();
	}

	if (e->key() == Qt::Key_Down && DEBUG) {

		if (1000 / (GAME_FPS * _fpsMultiplier - 0.1) >= 0) {
			_fpsMultiplier -= 0.1;
			_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
			_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
			_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
			_stationsTimer.setInterval(10000 / _fpsMultiplier);

		}
	}

	if (e->key() == Qt::Key_Up && DEBUG) {

		_fpsMultiplier += 0.1;
		_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
		_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
		_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
		_stationsTimer.setInterval(10000 / _fpsMultiplier);

	}

	if (e->key() == Qt::Key_Right && DEBUG) {

		_fpsMultiplier = 1;
		_engine.setInterval(1000 / (GAME_FPS * _fpsMultiplier));
		_passengerTimer.setInterval((2000 / (sqrt(_stationsNumber) * _fpsMultiplier)) + ((rand() % 3) - 1) * (rand() % 100));
		_passengersInOutTimer.setInterval(500 / _fpsMultiplier);
		_stationsTimer.setInterval(10000 / _fpsMultiplier);

	}
}

void Game::mousePressEvent(QMouseEvent* e){

	if (!_mousePressed) {

		// Delete train
		for (auto& t : _trainsVec) {
			if (t)
				if (t->boundingRect().contains(QPoint(e->pos().x()/ GAME_SCALE, e->pos().y()/ GAME_SCALE))) {
					for(auto& t2 : _trainsVec)
						if(t2)
							if (t2->lineIndex() == t->lineIndex() && t != t2) {

								_activeTrain = t->index();
								t->setOldPos(QPoint(t->pos().x(), t->pos().y()));
								if(_state != PAUSED)
									togglePause();
								for (auto& p : _passengersVec)
									if (p->trainIndex() == _activeTrain)
										p->setVisible(false);

								break;
							}
					break;
				}
		}

		// Add new line
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
	
	// Train moves with the cursor
	if (_activeTrain != -1)
		_trainsVec.at(_activeTrain)->setTrainPosition(QPoint(e->pos().x()/ GAME_SCALE, e->pos().y()/ GAME_SCALE));

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

					AI::instance()->addLink(_activeStation, s->index(), _activeLine);
					AI::instance()->update();

					_activeStation = s->index();

					if (_linesVec.at(_activeLine)->circularLine()) {
						
						_linesVec.at(_activeLine)->updateTcapPoint();
						
						bool hasTrain = false;
						bool forwardTrain = false;
						bool backwardTrain = false;
						int id = 0;

						for (auto& t : _trainsVec)
							if (t != 0) {
								if (t->lineIndex() == _activeLine) {
									t->setPath(_linesVec.at(_activeLine)->path());
									t->setCircular(true);
									hasTrain = true;
									id = t->index();

									if (t->direction() == 0)
										forwardTrain = true;
									else if (t->direction() == 1)
										backwardTrain = true;
								}
							}

						if (forwardTrain && backwardTrain)
							AI::instance()->setOrientation(true, _activeStation, _trainsVec.at(id));

						id = 0;

						if (!hasTrain) {
							for (auto& t : _trainsVec) {
								if (t == 0) {

									t = new Train(_activeLine, id,
												  _linesVec.at(_activeLine)->firstPoint(),
												  _linesVec.at(_activeLine)->path(), 
												  nearestStation(_linesVec.at(_activeLine)->firstPoint()));
									t->setCircular(true);
									AI::instance()->setOrientation(true, _activeStation, t);
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

	if (_state == RUNNING || _state == PAUSED)
		_scene->update();

	QGraphicsView::mouseMoveEvent(e);
}

void Game::mouseReleaseEvent(QMouseEvent* e){ 

	if (_activeTrain != -1) {
		if(_trainsVec.at(_activeTrain)->collidesWithItem(_trashBin))
			_trainsVec.at(_activeTrain)->setDeleting(true);
		_trainsVec.at(_activeTrain)->resetPos();

		for (auto& p : _passengersVec)
			if (p->trainIndex() == _activeTrain)
				p->setVisible(true);

		togglePause();
	}

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
	_activeTrain = -1;

	if (_state == RUNNING || _state == PAUSED)
		_scene->update();

	QGraphicsView::mouseReleaseEvent(e);
}
