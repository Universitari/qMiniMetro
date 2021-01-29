#pragma once

#include "Object.h"
#include "utils.h"
#include <QPainter>

class Train : public Object {

	enum Direction{FORWARD, BACKWARD, CIRCULAR};
	enum State{STOPPED, MOVING};

private:

	// Graphic elements
	QRect* _trainRect;
	QColor _color;
	QPainterPath _path;
	QPainterPath _oldPath;
	int _rotationAngle;
	float _speedMultiplier;
	
	// Logic elements
	Direction _direction;
	State _state;
	float _increment;
	float _length;
	int _passengers;
	int _lineIndex;
	int _stationIndex;
	bool _circular;
	int _index;
	bool _seats[6]; // false = occupied, true = available

public:

	Train(int lineIndex, int index, QPoint _centerPoint, QPainterPath linePath);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;

	virtual std::string name() { return "treno"; };
	virtual void animate() {};
	virtual void advance();
	virtual void solveCollisions() {};
	virtual void hit(Object* what) {};

	// Setters
	void setPath(QPainterPath linePath) { _path = linePath; }
	void setCircular(bool flag) { _circular = flag; }
	void setState(int state) { _state = State(state); }
	void setStationIndex(int index) { _stationIndex = index; }

	// Getter
	int lineIndex() { return _lineIndex; }
	int index() { return _index; }
	QPoint position() { return _trainRect->center(); }
	int rotationAngle() { return _rotationAngle; }
	int passengers() { return _passengers; }
	int state() { return int(_state); }
	int stationIndex() { return _stationIndex; }

	// Utility
	QPoint passengerPos(int ticket);
	void incrementPassengers(int passengerTicket);
	void decrementPassengers(int passengerTicket);
	int firstSeatAvailable();

};
