#pragma once

#include <QPen>
#include <QPoint>
#include <QPainter>
#include <QLine>
#include <QGraphicsSceneMouseEvent>
#include "Entity.h"

class Line : public Entity {

private:

	QLine _line;
	bool _pressed = false;
	int _name;
	static int _linesNumber;

public:

	Line(QPoint stationPoint);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return "Linea"; };
	int nome() { return _name; };
	void animate() {};
	void advance() {};
	void solveCollisions() {};
	void hit(Object* what) {};

	// Setters
	void setEndPoint(QPoint endP) { _line.setP2(endP); }

	// Getters
	QPoint startPoint() { return _line.p1(); }

	// Event handling
	// void mousePressEvent(QGraphicsSceneMouseEvent* event);
	// void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	// void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
};