#pragma once

#include "Object.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

class Button : public Object {

	enum Type {LINE, TRAIN};

private:

	QPoint _position;
	QPoint _mousePos;
	QColor _color;
	Type _type;
	bool _addingTrain;
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
	void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

};