#pragma once

#include "Object.h"
#include "Utils.h"
#include <QPainter>

class Train : public Object {

	enum Direction{FORWARD, BACKWARD};
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
	int _currentStation;
	int _nextStation;
	bool _circular;
	bool _deleting;
	int _index;
	float _distanceFromStation; 
	bool _seats[6]; // false = occupied, true = available
	QPoint _oldPos;

public:

	Train(int lineIndex, int index, QPoint centerPoint, QPainterPath linePath, int stationIndex);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	virtual void advance();

	// Setters
	void setPath(QPainterPath linePath) { _path = linePath; }
	void setCircular(bool flag) { _circular = flag; }
	void setState(int state) { _state = State(state); }
	void setCurrentStation(int index) { _currentStation = index; }
	void setNextStation(int index) { _nextStation = index; }
	void setDistanceFromStation(float distance) { _distanceFromStation = distance; }
	void setOldPos(QPoint p) { _oldPos = p; }
	void setTrainPosition(QPoint p);
	void setDeleting(bool flag) { _deleting = flag; }

	// Getter
	int lineIndex() { return _lineIndex; }
	int index() { return _index; }
	QPoint position() { return _trainRect->center(); }
	int rotationAngle() { return _rotationAngle; }
	int passengers() { return _passengers; }
	int state() { return int(_state); }
	int currentStation() { return _currentStation; }
	int nextStation() { return _nextStation; }
	int direction() { return int(_direction); }
	float distanceFromStation() { return _distanceFromStation; }
	bool circular() { return _circular; }
	bool deleting() { return _deleting; }

	// Utility
	QPoint passengerPos(int ticket);
	void incrementPassengers(int passengerTicket);
	void decrementPassengers(int passengerTicket);
	int firstSeatAvailable();
	void resetPos() { setTrainPosition(_oldPos); }

};
