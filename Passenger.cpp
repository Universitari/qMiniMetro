#include "Passenger.h"
class Train;
Passenger::Passenger(int stationIndex, QPoint pos, int shape) {

	_shape = Shape(shape); 
	_trainIndex = -1;
	_ticket = -1;
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
	painter->setOpacity(1);

	if (_trainIndex != -1) {
		pen.setColor(QColor(255, 255, 255));
		painter->setBrush(QBrush(QColor(255, 255, 255)));
		painter->setOpacity(0.65);
	}

	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);


	if (_shape == SQUARE) {

		QRect rect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
		//rotate(painter, rect, _rotationAngle);
		painter->drawRect(rect);
	}
	else if (_shape == TRIANGLE) {

		QPolygon triangle;
		triangle << QPoint(_position.x() + PASSENGER_SIZE / 2, _position.y())
				 << QPoint(_position.x(), _position.y() + PASSENGER_SIZE)
				 << QPoint(_position.x() + PASSENGER_SIZE, _position.y() + PASSENGER_SIZE);
		
		//rotate(painter, triangle.boundingRect(), _rotationAngle);
		painter->drawPolygon(triangle);
	}
	else if (_shape == CIRCLE) {

		QRect rect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
		//rotate(painter, rect, _rotationAngle);
		painter->drawEllipse(rect);
	}

}

QRectF Passenger::boundingRect() const{

	return QRectF(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
}

void Passenger::getOnTrain(int trainIndex){

	_trainIndex = trainIndex;
	_stationIndex = -1;

}

void Passenger::getOffTrain(int stationIndex){

	_trainIndex = -1;
	_stationIndex = stationIndex;

}

void Passenger::advance() {
	
}
