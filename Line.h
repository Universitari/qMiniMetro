#pragma once

#include <QPen>
#include <QPoint>
#include <QPainter>
#include <QLine>
#include <QJsonObject>
#include <QJsonArray>
#include "Object.h"
#include "utils.h"

class Line : public Object {

	enum Name { CIRCLE, CENTRAL, PICCADILLY, VICTORIA, DISTRICT, HAMMERSMITH, BAKERLOO };
	enum State { INITIAL, MOD_TAIL, MOD_HEAD };

private:

	// Graphic elements
	QLine _TcapHead, _neckTcapHead;
	QLine _TcapTail, _neckTcapTail;
	QPolygon* _TcapHitbox;
	QPainterPath _mousePath;
	QPainterPath _path;

	// Logic elements
	bool _circularLine;
	bool _deleting;
	QColor _color;
	Name _name;
	State _state;
	std::vector<QPoint> _stations;

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

	// Savegame functions
	void read(const QJsonObject& json);
	void write(QJsonObject& json) const;

	// Setters
	void setNextPoint(QPoint nextP);
	void setCurrentPoint(QPoint currP);
	void setCircularLine(bool flag) { _circularLine = flag; }
	void setDeleting(bool flag) { _deleting = flag; update(); }

	// Getters
	QPoint firstPoint() { return _stations.front(); }
	QPoint lastPoint() { return _stations.back(); }
	QPoint stationPoint(int index) { return _stations.at(index); }

	bool circularLine() { return _circularLine; }
	bool deleting() { return _deleting; }
	State state() { return _state; }
	QPainterPath path() { return _path; }

	// Utility
	bool validPoint(QPoint p);
	int size() { return _stations.size(); }

	void updateTcapPoint();
	QLine setTcap(QPoint p1, QPoint p2);
	bool pointerOnCap(QPoint pointerPos);

	int sector(QPoint s, QPoint pointerPos);
	QPoint middlePoint(QPoint s, QPoint p);

	bool pathColliding(QRect rect);
};