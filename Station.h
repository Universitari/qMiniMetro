#pragma once

#include <QPoint>
#include <QPainter>
#include <QJsonObject>
#include <time.h>
#include "config.h"
#include "Object.h"
#include "Utils.h"

class Station : public Object {

	enum Shape { CIRCLE, TRIANGLE, SQUARE, STAR, PENTAGON, RHOMBUS, CROSS, DIAMOND };

private:

	static bool _uniqueStations[5];

	QPoint _position;
	unsigned int _currentPass;
	Shape _shape;
	int _index;
	QTimer* _deathTimer;
	int _deathTimerRemainingTime;

public:

	Station(QPoint pos, int index, int shape = -1);
	Station(const Station& s);
	~Station();

	// Metodi virtuali reimplementati
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);
	QRectF boundingRect() const;
	void advance() { }

	// Savegame functions
	void read(const QJsonObject& json);
	void write(QJsonObject& json) const;

	// Getters
	QPoint position() { return _position; }
	QPoint centerPos() {return QPoint(_position.x() + STATION_SIZE / 2,
									  _position.y() + STATION_SIZE / 2);}
	int stationShape() { return int(_shape); }
	int index() { return _index; }
	int passengers() { return _currentPass; }

	// Utility
	bool pointerOnStation(QPoint pointerPos);
	void addPassenger();
	void removePassengers() { _currentPass = 0; }
	int uniqueShape();
	void resetUniqueStations();
	void pauseDeathTimer();
	void resumeDeathTimer();
};