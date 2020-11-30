#pragma once

#include "Entity.h"
#include "utils.h"
#include <QPainter>

class Train : public Entity {

private:

	QPoint _position;
	unsigned int _maxPass;
	QRect *_trainRect;
	int _lineIndex;
	QColor _color;
	int _rotationAngle;

public:

	Train(int index, QPoint _centerPoint);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;

	virtual std::string name() { return "treno"; };
	virtual void animate() {};
	virtual void advance() {};
	virtual void solveCollisions() {};
	virtual void hit(Object* what) {};

};
