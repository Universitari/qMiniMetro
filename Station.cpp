#include "Station.h"

const unsigned int Station::_maxPass = 10;

Station::Station(QPoint pos) {

	_position = pos;
	_shape = Shape(rand() % GAME_PROGRESSION);

}

Station::Station(const Station& s) {

	_position = s._position;
	_currentPass = s._currentPass;
	_shape = s._shape;

}

void Station::paint(QPainter* painter,
	const QStyleOptionGraphicsItem* option,
	QWidget* widget) {

	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(QColor(0, 0, 0)));
	painter->drawRect(_position.x(), _position.y(), 30, 30);
}

QRectF Station::boundingRect() const
{
	qreal penWidth = 1;
	return QRectF();
}