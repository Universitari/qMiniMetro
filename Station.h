#pragma once

#include <QPoint>
#include <QPainter>
#include <QJsonObject>
#include <time.h>
#include "Object.h"

class Station : public Object {

	enum Shape { SQUARE, TRIANGLE, CIRCLE };

private:

	QPoint _position;
	unsigned int _currentPass;
	Shape _shape;
	int _index;

public:

	Station(QPoint pos, int index);
	Station(const Station& s);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return("stazione"); };
	void animate() { }
	void advance() { }
	void solveCollisions() { }
	void hit(Object* what) { }

	// Savegame functions
	void read(const QJsonObject& json);
	void write(QJsonObject& json) const;

	// Getters
	QPoint position() { return _position; }
	QPoint centerPos() {return QPoint(_position.x() + STATION_SIZE / 2,
									  _position.y() + STATION_SIZE / 2);}
	int stationShape() { return int(_shape); }
	int index() { return _index; }
	int passengers() { return _currentPass; }

	// Utility
	bool pointerOnStation(QPoint pointerPos);
	void addPassenger() { _currentPass++; }
	void removePassengers() { _currentPass = 0; }
};