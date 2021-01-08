#include "Passenger.h"

Passenger::Passenger(int stationIndex, QPoint pos, int shape) {

	_status = WAITING;
	_shape = Shape(shape); 
	_trainIndex = -1;
	_stationIndex = stationIndex;
	_position = pos;
	_rotationAngle = 0;
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

		QRect rect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
		rotate(painter, rect, _rotationAngle);
		painter->drawRect(rect);
	}
	else if (_shape == TRIANGLE) {

		QPolygon triangle;
		triangle << QPoint(_position.x() + PASSENGER_SIZE / 2, _position.y())
				 << QPoint(_position.x(), _position.y() + PASSENGER_SIZE)
				 << QPoint(_position.x() + PASSENGER_SIZE, _position.y() + PASSENGER_SIZE);
		
		rotate(painter, triangle.boundingRect(), _rotationAngle);
		painter->drawPolygon(triangle);
	}
	else if (_shape == CIRCLE) {

		QRect rect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
		rotate(painter, rect, _rotationAngle);
		painter->drawEllipse(rect);
	}

}

QRectF Passenger::boundingRect() const{

	return QRectF(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
}

void Passenger::getOnTrain(int trainIndex, QPoint pos){

	_trainIndex = trainIndex;
	_stationIndex = -1;

	_position = pos;

}

void Passenger::translate(QLineF shiftLine) { 

	_position.setX(_position.x() + shiftLine.dx());
	_position.setY(_position.y() + shiftLine.dy());
}

void Passenger::advance() {

}
