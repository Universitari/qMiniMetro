#pragma once

#include <QPoint>
#include <QPainter>
#include <time.h>
#include "Inert.h"
#include "config.h"

class Station : public Inert {

	enum Shape { SQUARE, TRIANGLE, CIRCLE };

private:

	QPoint _position;
	static const unsigned int _maxPass;
	unsigned int _currentPass = 0;
	Shape _shape;

public:

	Station(QPoint pos);
	Station(const Station& s);

	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;

	std::string name() { return("stazione"); };
	void animate() { }
	void advance() { }
	void solveCollisions() { }
	void hit(Object* what) { }

	// Getters
	QPoint position() { return _position; }

};