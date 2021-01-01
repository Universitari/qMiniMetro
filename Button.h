#pragma once

#include "Object.h"
#include <QPainter>

class Button : public Object {

	enum Type {LINE, TRAIN};

private:

	QPoint _position;
	QColor _color;
	Type _type;
	int _index = -1;

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