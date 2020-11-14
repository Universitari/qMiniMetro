#pragma once

#include <QPen>
#include <QPoint>
#include <QPainter>
#include <QLine>
#include <QGraphicsSceneMouseEvent>
#include "Entity.h"

class Line : public Entity {

private:

	QPoint _startPoint;
	QLine _line;
	bool _pressed = false;
	int _name;
	static int _linesNumber;

public:

	Line();

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

	// Event handling

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
};