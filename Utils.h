#pragma once

#include <QPoint>
#include <QPainter>
#include <Vector>

float angularCoeff(QPoint p1, QPoint p2);
float distance(QPoint p1, QPoint p2);
QColor setColor(int index);
void rotate(QPainter* p, const QRect& r, qreal angle);