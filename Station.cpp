#include "Station.h"

const unsigned int Station::_maxPass = 10;

Station::Station(QPoint pos) {

	_position = pos;
	_shape = Shape(rand() % GAME_PROGRESSION);
	setZValue(2);
}

Station::Station(const Station& s) {

	_position = s._position;
	_currentPass = s._currentPass;
	_shape = s._shape;
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

		QPolygon triangle;
		triangle	<< QPoint(_position.x() + STATION_SIZE/2, _position.y())
					<< QPoint(_position.x(), _position.y() + STATION_SIZE)
					<< QPoint(_position.x() + STATION_SIZE, _position.y() + STATION_SIZE);
		painter->drawPolygon(triangle);
	}
	else if (_shape == CIRCLE) {

		painter->drawEllipse(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
	}
}

QRectF Station::boundingRect() const{

	qreal penWidth = 1;
	return QRectF(0, 0, 30, 30);
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