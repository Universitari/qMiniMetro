#pragma once

#include "Entity.h"
#include "utils.h"
#include <QPainter>

class Train : public Entity {

	enum Direction{FORWARD, BACKWARD, CIRCULAR};

private:

	QPoint _position;
	unsigned int _maxPass;
	QRect *_trainRect;
	int _lineIndex;
	QColor _color;
	int _rotationAngle;
	QPainterPath _path;
	float _increment;
	Direction _direction;
	float _length;
	QPainterPath _oldPath;

public:

	Train(int index, QPoint _centerPoint, QPainterPath linePath);

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
	void setDirection(int dir) { _direction = Direction(dir); }

	// Getter
	int lineIndex() { return _lineIndex; }

};
