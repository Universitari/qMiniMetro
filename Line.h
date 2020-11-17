#pragma once

#include <QPen>
#include <QPoint>
#include <QPainter>
#include <QLine>
#include "Entity.h"

class Line : public Entity {

	enum Name{CIRCLE, CENTRAL, PICCADILLY, VICTORIA, DISTRICT, HAMMERSMITH, BAKERLOO};

private:

	QPoint _linePoints[MAX_STATIONS];
	int _pointsCounter;
	static int _linesNumber;
	bool _circularLine;
	QColor _color;
	Name _name;

public:

	Line(QPoint stationPoint);
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
	void setCurrentPoint(QPoint currP) { _linePoints[_pointsCounter] = currP; }
	void setCircularLine(bool flag) { _circularLine = flag; }

	// Getters
	QPoint startPoint() { return _linePoints[0]; }
	QPoint lastPoint() { return _linePoints[_pointsCounter - 1]; }
	bool circularLine() { return _circularLine; }

	// Utility
	bool validPoint(QPoint p);
	unsigned int size() { return _pointsCounter; }

};