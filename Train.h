#pragma once

#include "Object.h"
#include "utils.h"
#include <QPainter>

class Train : public Object {

	enum Direction{FORWARD, BACKWARD, CIRCULAR};

private:

	// Graphic elements
	QRect* _trainRect;
	QColor _color;
	QPainterPath _path;
	QPainterPath _oldPath;
	int _rotationAngle;
	
	// Logic elements
	Direction _direction;
	float _increment;
	float _length;
	int _maxPass;
	int _passengers;
	int _lineIndex;
	bool _circular;
	int _index;

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

	// Getter
	int lineIndex() { return _lineIndex; }
	int index() { return _index; }
	QPoint position() { return _trainRect->center(); }
	int rotationAngle() { return _rotationAngle; }
	int passengers() { return _passengers; }

	// Utility
	QPoint passengerPos(int ticket);
	void incrementPassengers() { _passengers++; }

};
