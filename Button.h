#pragma once

#include "Inert.h"
#include <QPainter>

class Button : public Inert {

	enum Type {LINE, TRAIN};

private:

	QPoint _position;
	Type _type;
	int _index;
	QColor _color;

public:

	Button(int index);

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return("button"); };
	void animate() { }
	void advance() { }
	void solveCollisions() { }
	void hit(Object* what) { }

	// Event Handling
	void mousePressEvent(QGraphicsSceneMouseEvent* e);

};