#include "Station.h"

Station::Station(QPoint pos, int index) {

	_position = pos;

	switch (index) {
		case(0): {
			_shape = Shape(0);
			break;
		}
		case(1): {
			_shape = Shape(1);
			break;
		}
		case(2): {
			_shape = Shape(2);
			break;
		}
		default: {
			_shape = Shape(rand() % GAME_PROGRESSION);
			break;
		}
	}
	
	_index = index;
	_currentPass = 0;
	setZValue(4);
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
	return QRectF(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
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