#include "Passenger.h"

Passenger::Passenger(int stationIndex, QPoint pos, int shape) {

	_status = WAITING;
	_shape = Shape(shape); 
	// allora praticamente tu devi fare quella funzione epica 
	// che prende la shape da una delle stazioni create all'interno 
	// del vettore in game.
	_trainIndex = -1;
	_stationIndex = stationIndex;
	_position = pos;
	setZValue(3);

}

void Passenger::paint(QPainter* painter, 
					  const QStyleOptionGraphicsItem* option, 
					  QWidget* widget){

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(1);
	painter->setBrush(QBrush(QColor(0, 0, 0)));
	pen.setJoinStyle(Qt::MiterJoin);

	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);

	if (_shape == SQUARE) {

		painter->drawRect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
	}
	else if (_shape == TRIANGLE) {

		QPolygon triangle;
		triangle << QPoint(_position.x() + PASSENGER_SIZE / 2, _position.y())
			<< QPoint(_position.x(), _position.y() + PASSENGER_SIZE)
			<< QPoint(_position.x() + PASSENGER_SIZE, _position.y() + PASSENGER_SIZE);
		painter->drawPolygon(triangle);
	}
	else if (_shape == CIRCLE) {

		painter->drawEllipse(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
	}

}

QRectF Passenger::boundingRect() const{

	return QRectF(0, 0, PASSENGER_SIZE, PASSENGER_SIZE);
}
