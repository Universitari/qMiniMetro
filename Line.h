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

	// Graphic elements
	QLine _TcapHead, _neckTcapHead;
	QLine _TcapTail, _neckTcapTail;
	QPolygon* _TcapHitbox;
	QPainterPath _mousePath;
	QPainterPath _path;

	// Logic elements
	bool _circularLine;
	QColor _color;
	Name _name;
	State _state;
	std::list<QPoint> _stations;

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
	QPoint firstPoint() { return _stations.front(); }
	QPoint lastPoint() { return _stations.back(); }
	bool circularLine() { return _circularLine; }
	State state() { return _state; }

	// Utility
	bool validPoint(QPoint p);
	unsigned int size() { return _stations.size(); }
	QPoint nextPointOnLine(QPoint p1, QPoint p2, int length);

	void updateTcapPoint();
	QLine setTcap(QPoint p1, QPoint p2);
	bool pointerOnCap(QPoint pointerPos);

	int sector(QPoint s, QPoint pointerPos);
	QPoint middlePoint(QPoint s, QPoint p);

};
/*            _path.lineTo(nextPointOnLine(middlePoint(pathPos, *std::next(iter)), pathPos, -10));
                pathPos.setX(_path.currentPosition().x());
                pathPos.setY(_path.currentPosition().y());
            _path.quadTo(middlePoint(*iter, *std::next(iter)), nextPointOnLine(middlePoint(*iter, *std::next(iter)), *std::next(std::next(iter)), -10));
                pathPos.setX(_path.currentPosition().x());
                pathPos.setY(_path.currentPosition().y());
            _path.lineTo(nextPointOnLine(pathPos, *std::next(iter), -35));
                pathPos.setX(_path.currentPosition().x());
                pathPos.setY(_path.currentPosition().y());
            _path.quadTo(*std::next(iter), nextPointOnLine(*std::next(iter), *std::next(std::next(iter)), -35));
*/