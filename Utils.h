#pragma once

#include <QPoint>
#include <QPainter>
#include <Vector>
#include "config.h"

float angularCoeff(QPoint p1, QPoint p2);
float distance(QPoint p1, QPoint p2);
QColor setColor(int index);
void rotate(QPainter* p, const QRect& r, qreal angle);
bool spawnAreaAvailable(QPoint spawnPoint, int stationsNum);
QPoint nextPointOnLine(QPoint p1, QPoint p2, int length);
