#include "Line.h"

int Line::_linesNumber = 0;

Line::Line(QPoint startP){

    _linePoints[0] = startP;
    _linePoints[1] = startP;
    _linesNumber++;
    _pointsCounter = 1;
    _circularLine = false;
    _name = Name(_linesNumber-1);

    switch (_name) {
        case(CIRCLE):
            _color = QColor(255, 206, 0);
            break;
        case(CENTRAL):
            _color = QColor(220, 36, 31);
            break;
        case(PICCADILLY):
            _color = QColor(0, 25, 168);
            break;
        case(VICTORIA):
            _color = QColor(0, 160, 255);
            break;
        case(DISTRICT):
            _color = QColor(0, 114, 41);
            break;
        case(HAMMERSMITH):
            _color = QColor(215, 153, 175);
            break;
        case(BAKERLOO):
            _color = QColor(137, 78, 36);
            break;
    }
}

Line::~Line(){

    _linesNumber--;
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


    //if (_line.isNull()) return;
    if (_pointsCounter == 0) return;
    pen.setWidth(10);
    pen.setColor(_color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    //painter->drawLine(_line);
    painter->drawPolyline(_linePoints, _pointsCounter+1);

}

QRectF Line::boundingRect() const{

    return QRectF(QPointF(0, 0), QPointF(1920, 1080));
}

void Line::setNextPoint(QPoint nextP) {

    _linePoints[_pointsCounter] = nextP;
    _pointsCounter++;
    _linePoints[_pointsCounter] = nextP;
    printf("inseriti %d punti\n", _pointsCounter);
}

bool Line::validPoint(QPoint p){

    if (p == _linePoints[0] && !_circularLine && _pointsCounter > 2) {
        _circularLine = true;
        return true;
    }

    for (int i = 0; i < _pointsCounter; i++) {
        if (p == _linePoints[i])
            return false;
    }
    return true;
}
