#include "Line.h"

int Line::_linesNumber = 0;

Line::Line(QPoint startP){

    _line.setP1(startP);
    _line.setP2(startP);
    _name = _linesNumber++;

}


void Line::paint(QPainter* painter,
	const QStyleOptionGraphicsItem* option,
	QWidget* widget) {

    QPen pen;

    painter->setRenderHint(QPainter::Antialiasing);

    // Show bounding rect
    pen.setWidth(2);
    pen.setColor(QColor(Qt::darkGray));
    painter->setPen(pen);
    // painter->drawRect(boundingRect().adjusted(5, 5, -5, -5));

    if (_line.isNull()) return;
    pen.setWidth(10);
    pen.setColor(QColor(Qt::darkBlue));
    painter->setPen(pen);
    painter->drawLine(_line);

}

QRectF Line::boundingRect() const{

    return QRectF(QPointF(0, 0), QPointF(1920, 1080));
}