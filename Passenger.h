#pragma once

#include "Object.h"
#include <QPoint>
#include <QPainter>

class Passenger : public Object {

	enum Shape { SQUARE, TRIANGLE, CIRCLE };
	enum Status { WAITING, TRAVELLING };

private:

	Shape _shape;
	Status _status;
	int _trainIndex;
	int _stationIndex;
	QPoint _position;


public:

	Passenger(int stationIndex, QPoint pos, int shape);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return "Passenger"; }
	void animate() {}
	void advance() {}
	void solveCollisions() {}
	void hit(Object* what) {}

	// Getters
	int stationIndex() { return _stationIndex; }

};