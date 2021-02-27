#pragma once

#include <QPoint>
#include <QPainter>
#include <Vector>

float angularCoeff(QPoint p1, QPoint p2);
float distance(QPoint p1, QPoint p2);
QColor setColor(int index);
void rotate(QPainter* p, const QRect& r, qreal angle);
bool spawnAreaAvailable(QPoint spawnPoint, int stationsNum);
QPoint nextPointOnLine(QPoint p1, QPoint p2, int length);
QPoint spawnPos();
QPolygonF triangle(QPoint pos, int size);
QPolygonF star(QPoint pos, int size);
QPolygonF pentagon(QPoint pos, int size);
QPolygonF rhombus(QPoint pos, int size);
QPolygonF cross(QPoint pos, int size);
QPolygonF diamond(QPoint pos, int size);
