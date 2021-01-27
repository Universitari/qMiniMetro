#pragma once

#include "Object.h"
#include "utils.h"
#include <QPoint>
#include <QPainter>

class Passenger : public Object {

	enum Shape { SQUARE, TRIANGLE, CIRCLE };

private:

	Shape _shape;
	int _trainIndex;
	int _stationIndex;
	QPoint _position;
	int _ticket;

public:

	Passenger(int stationIndex, QPoint pos, int shape);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return "Passenger"; }
	void animate() {}
	void advance();
	void solveCollisions() {}
	void hit(Object* what) {}

	// Setters
	void setPos(QPoint pos) { _position = pos; }
	void setTicket(int passengers) { _ticket = passengers; }

	// Getters
	int stationIndex() { return _stationIndex; }
	int trainIndex() { return _trainIndex; }
	int ticket() { return _ticket; }
	int passengerShape() { return int(_shape); }

	// Utility
	void getOnTrain(int trainIndex);
	void moveTransformPoint(QPoint point) { setTransformOriginPoint(point); }

};