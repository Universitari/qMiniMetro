#pragma once

#include <QPen>
#include <QPoint>
#include <QPainter>
#include <QLine>
#include "Entity.h"
#include "utils.h"

class Line : public Entity {

	enum Name{CIRCLE, CENTRAL, PICCADILLY, VICTORIA, DISTRICT, HAMMERSMITH, BAKERLOO};
	enum State{INITIAL, MOD_TAIL, MOD_HEAD};

private:

	QPoint _linePoints[MAX_STATIONS];
	int _pointsCounter;
	bool _circularLine;
	QColor _color;
	Name _name;
	QLine _TcapHead;
	QLine _TcapTail;
	State _state;
	QPolygon* _TcapHitbox;

public:

	Line(QPoint stationPoint, int index);
	~Line();

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	std::string name() { return "Linea"; }
	int nome() { return _name; }
	void animate() {}
	void advance() {}
	void solveCollisions() {}
	void hit(Object* what) {}

	// Setters
	void setNextPoint(QPoint nextP);
	void setCurrentPoint(QPoint currP);
	void setCircularLine(bool flag) { _circularLine = flag; }

	// Getters
	QPoint startPoint() { return _linePoints[1]; }
	QPoint lastPoint() { return _linePoints[_pointsCounter - 1]; }
	bool circularLine() { return _circularLine; }
	State state() { return _state; }

	// Utility
	bool validPoint(QPoint p);
	unsigned int size() { return _pointsCounter; }
	QPoint TcapPoint(QPoint p1, QPoint p2, QPoint edgePoint);
	void updateTcapPoint();
	QLine setTcap(QPoint p1, QPoint p2);
	bool pointerOnCap(QPoint pointerPos);

};