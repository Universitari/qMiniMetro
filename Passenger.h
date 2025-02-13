#pragma once

#include "Object.h"
#include "Utils.h"
#include <cmath>
#include <vector>
#include <QPoint>
#include <QPainter>

class Passenger : public Object {

	enum Shape { CIRCLE, TRIANGLE, SQUARE, STAR, PENTAGON, RHOMBUS, CROSS, DIAMOND };

private:

	Shape _shape;
	int _trainIndex;
	int _stationIndex;
	QPoint _position;
	int _ticket;
	std::vector<int> _finalStations;

public:

	Passenger(int stationIndex, QPoint pos, int shape);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	void advance();

	// Setters
	void setPos(QPoint pos) { _position = pos; }
	void setTicket(int passengers) { _ticket = passengers; }

	// Getters
	int stationIndex() { return _stationIndex; }
	int trainIndex() { return _trainIndex; }
	int ticket() { return _ticket; }
	int passengerShape() { return int(_shape); }
	std::vector<int> finalStations() { return _finalStations; }

	// Utility
	void getOnTrain(int trainIndex);
	void getOffTrain(int stationIndex);
	void moveTransformPoint(QPoint point) { setTransformOriginPoint(point); }
	void updateFinalStations();


};