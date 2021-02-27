#include "Passenger.h"
#include "AI.h"

class Train;
Passenger::Passenger(int stationIndex, QPoint pos, int shape) {

	_shape = Shape(shape);
	_trainIndex = -1;
	_ticket = -1;
	_stationIndex = stationIndex;
	_position = pos;
	AI::instance()->findFinalStations(stationIndex, shape, _finalStations);
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

		painter->drawRect(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
	}
	else if (_shape == TRIANGLE) {

		QPolygonF _triangle = triangle(_position, PASSENGER_SIZE);
		painter->drawPolygon(_triangle);
	}
	else if (_shape == CIRCLE) {

		painter->drawEllipse(_position.x(), _position.y(), PASSENGER_SIZE, PASSENGER_SIZE);
	}
	else if (_shape == STAR) {

		QPolygonF _star = star(_position, PASSENGER_SIZE);
		painter->drawPolygon(_star);
	}
	else if (_shape == PENTAGON) {

		QPolygonF _pentagon = pentagon(_position, PASSENGER_SIZE);
		painter->drawPolygon(_pentagon);
	}
	else if (_shape == RHOMBUS) {

		QPolygonF _rhombus = rhombus(_position, PASSENGER_SIZE);
		painter->drawPolygon(_rhombus);
	}
	else if (_shape == CROSS) {

		QPolygonF _cross = cross(_position, PASSENGER_SIZE);
		painter->drawPolygon(_cross);
	}
	else if (_shape == DIAMOND) {

		QPolygonF _diamond = diamond(_position, PASSENGER_SIZE);
		painter->drawPolygon(_diamond);
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

void Passenger::updateFinalStations(){
	AI::instance()->findFinalStations(_stationIndex, _shape, _finalStations);
}

void Passenger::advance() {
	
}
