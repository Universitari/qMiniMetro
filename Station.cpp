#include "Station.h"
#include "Game.h"

bool Station::_uniqueStations[5] = { 0, 0, 0, 0, 0 };

Station::Station(QPoint pos, int index, int shape) {

	_position = pos;
	_deathTimer = 0;

	if (shape == -1) {
		int randomShape = 1 + rand() % 100;
		if (randomShape <= 40)
			_shape = Shape(0);
		else if (randomShape <= 70)
			_shape = Shape(1);
		else if (randomShape <= 90)
			_shape = Shape(2);
		else if (randomShape <= 100)
			_shape = Shape(uniqueShape());
	}
	else
		_shape = Shape(shape);

	_deathTimerRemainingTime = 0;
	_index = index;
	_currentPass = 0;
	setZValue(4);
}

Station::Station(const Station& s) {

	_position = s._position;
	_currentPass = s._currentPass;
	_shape = s._shape;
}

Station::~Station(){

	if(_deathTimer)
		delete _deathTimer;

}

void Station::paint(QPainter* painter,
	const QStyleOptionGraphicsItem* option,
	QWidget* widget) {

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(5);
	painter->setBrush(QBrush(QColor(255, 255, 255)));
	pen.setJoinStyle(Qt::MiterJoin);

	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);
	
	if (_shape == SQUARE) {

		painter->drawRect(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
	}
	else if (_shape == TRIANGLE) {

		QPolygonF _triangle = triangle(_position, STATION_SIZE);
		painter->drawPolygon(_triangle);
	}
	else if (_shape == CIRCLE) {

		painter->drawEllipse(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
	}
	else if (_shape == STAR) {

		QPolygonF _star = star(_position, STATION_SIZE);

		pen.setWidth(4);
		painter->setPen(pen);
		painter->drawPolygon(_star);
	}
	else if (_shape == PENTAGON) {

		QPolygonF _pentagon = pentagon(_position, STATION_SIZE);
		painter->drawPolygon(_pentagon);
	}
	else if (_shape == RHOMBUS) {

		QPolygonF _rhombus = rhombus(_position, STATION_SIZE);
		painter->drawPolygon(_rhombus);
	}
	else if (_shape == CROSS) {

		QPolygonF _cross = cross(_position, STATION_SIZE);

		pen.setWidth(4);
		painter->setPen(pen);
		painter->drawPolygon(_cross);
	}
	else if (_shape == DIAMOND) {

		QPolygonF _diamond = diamond(_position, STATION_SIZE);
		painter->drawPolygon(_diamond);
	}

	// Progress bar for death
	if (_deathTimer) {

		int increment;
		if (_deathTimerRemainingTime == 0)
			increment = STATION_SIZE * (DEATH_TIME*1000 - _deathTimer->remainingTime()) / (DEATH_TIME * 1000);
		else
			increment = STATION_SIZE * (DEATH_TIME*1000 - _deathTimerRemainingTime) / (DEATH_TIME * 1000);

		pen.setWidth(2);
		pen.setColor(QColor(0, 0, 0));
		painter->setPen(pen);
		painter->drawRect(_position.x() - 4, _position.y() + STATION_SIZE + 6, STATION_SIZE + 8, 8);

		pen.setColor(QColor(150, 0, 0));
		pen.setWidth(6);
		painter->setPen(pen);

		painter->drawLine(QPointF(_position.x(), _position.y() + STATION_SIZE + 10),
						  QPointF(_position.x() + increment, _position.y() + STATION_SIZE + 10));

	}

}

QRectF Station::boundingRect() const{

	qreal penWidth = 1;
	return QRectF(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
}

void Station::read(const QJsonObject& json){

	if (json.contains("Shape") && json["Shape"].isDouble())
		_shape = Shape(json["Shape"].toInt());

	if (json.contains("Index") && json["Index"].isDouble())
		_index = json["Index"].toInt();	
		
	if (json.contains("Position (x)") && json["Position (x)"].isDouble())
		_position.setX(json["Position (x)"].toInt());

	if (json.contains("Position (y)") && json["Position (y)"].isDouble())
		_position.setY(json["Position (y)"].toInt());

	if (json.contains("Unique Stations") && json["Unique Stations"].isArray()) {
		QJsonArray uniqueStationsArray = json["Unique Stations"].toArray();

		for (int i = 0; i < 5; i++)
			_uniqueStations[i] = uniqueStationsArray[i].toBool();
	}
}

void Station::write(QJsonObject& json) const{

	json["Shape"] = int(_shape);
	json["Index"] = _index;
	json["Position (x)"] = _position.x();
	json["Position (y)"] = _position.y();

	QJsonArray uniqueStationsArray;
	for (int i = 0; i < 5; i++)
		uniqueStationsArray.append(_uniqueStations[i]);

	json["Unique Stations"] = uniqueStationsArray;

}

bool Station::pointerOnStation(QPoint pointerPos) {

	if (pointerPos.x() >= position().x() * GAME_SCALE &&
		pointerPos.x() <= (position().x() + STATION_SIZE) * GAME_SCALE &&
		pointerPos.y() >= position().y() * GAME_SCALE &&
		pointerPos.y() <= (position().y() + STATION_SIZE) * GAME_SCALE)
		return true;
	else
		return false;
}

void Station::addPassenger(){

	_currentPass++;

	if (_currentPass == MAX_PASS_STATION && _deathTimer == 0) {

		_deathTimer = new QTimer;
		_deathTimer->setInterval(DEATH_TIME * 1000);
		_deathTimer->setTimerType(Qt::PreciseTimer);
		_deathTimer->setSingleShot(true);
		_deathTimer->start();
		QObject::connect(_deathTimer, SIGNAL(timeout()), Game::instance(), SLOT(death()));
	} else
		if (_currentPass < MAX_PASS_STATION && _deathTimer != 0) {
			//_deathTimer->stop();
			delete _deathTimer;
			_deathTimer = 0;
		}
}

int Station::uniqueShape(){

	int x;

	for (auto& s : _uniqueStations)
		if (s)
			x++;

	if (x >= MAX_UNIQUE_STATIONS)
		return(rand() % 3);

	do {
		x = rand() % 5;
	} while (_uniqueStations[x]);

	_uniqueStations[x] = true;

	return(x+3);
}

void Station::resetUniqueStations(){

	for (int i = 0; i < 5; i++)
		_uniqueStations[i] = false;
}

void Station::pauseDeathTimer(){

	if (_deathTimer) {
		_deathTimerRemainingTime = _deathTimer->remainingTime();
		_deathTimer->stop();
	}

}

void Station::resumeDeathTimer(){

	if (_deathTimer) {
		_deathTimer->setInterval(_deathTimerRemainingTime);
		_deathTimer->start();
	}

	_deathTimerRemainingTime = 0;

}
