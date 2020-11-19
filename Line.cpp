#include "Line.h"

int Line::_linesNumber = 0;

Line::Line(QPoint startP){

    _linePoints[0] = startP;
    _linePoints[1] = startP;
    _linePoints[2] = startP;
    _pointsCounter = 2;
    _circularLine = false;
    _name = Name(_linesNumber);
    _linesNumber++;
    setZValue(1);

    switch (_name) {
        case(CIRCLE):
            // Yellow
            _color = QColor(255, 206, 0);
            break;
        case(CENTRAL):
            // Red
            _color = QColor(220, 36, 31);
            break;
        case(PICCADILLY):
            // Dark blue
            _color = QColor(0, 25, 168);
            break;
        case(VICTORIA):
            // Blue
            _color = QColor(0, 160, 255);
            break;
        case(DISTRICT):
            // Green
            _color = QColor(0, 114, 41);
            break;
        case(HAMMERSMITH):
            // Pink
            _color = QColor(215, 153, 175);
            break;
        case(BAKERLOO):
            // Brown
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

    pen.setCapStyle(Qt::SquareCap);
    painter->setPen(pen);
    painter->drawLine(_TcapTail);
    painter->drawLine(_TcapHead);

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

float Line::angularCoeff(QPoint p1, QPoint p2) {

    float x1 = p1.x();
    float y1 = p1.y();
    float x2 = p2.x();
    float y2 = p2.y();

    float m = (y2 - y1) / abs(x2 - x1);
 
    printf("P1: %.2f, %.2f\nP2: %.2f, %.2f\nangular coefficient: %f\n",
            x1, y1, x2, y2, m);
   
    return m;
}

QPoint Line::TcapPoint(QPoint p1, QPoint p2, QPoint edgePoint){

    float m = angularCoeff(p1, p2);
    float angle = atan(m);
    int length = 40;
    int x = cos(angle) * length;
    int y = sin(angle) * length;
 
    if (p1.x() > p2.x())
        x = -x;

    if(edgePoint == _linePoints[0])
        return QPoint(edgePoint.x() - x, edgePoint.y() - y);
    else
        return QPoint(edgePoint.x() + x, edgePoint.y() + y);
}

void Line::updateTcapPoint(){

    _linePoints[0] = TcapPoint(_linePoints[1], _linePoints[2], _linePoints[0]);
    _linePoints[_pointsCounter] = TcapPoint(_linePoints[_pointsCounter-2], _linePoints[_pointsCounter-1], _linePoints[_pointsCounter]);
    
    _TcapTail = setTcap(_linePoints[_pointsCounter - 1], _linePoints[_pointsCounter]);
    _TcapHead = setTcap(_linePoints[1], _linePoints[0]);
}

QLine Line::setTcap(QPoint p1, QPoint p2){

    QLine l;

    float m = angularCoeff(p1, p2);

    float angle = atan(m) + 3.14159 / 2;
    int length = 15;
    int x = p2.x() + cos(angle) * length;
    int y = p2.y() + sin(angle) * length;

    if (p1.x() > p2.x())
        x = p2.x() - (cos(angle) * length);

    l.setP1(QPoint(x, y));

    angle = atan(m) - 3.14159 / 2;
    x = p2.x() + cos(angle) * length;
    y = p2.y() + sin(angle) * length;

    if ( p1.x() > p2.x())
        x = p2.x() - (cos(angle) * length);

    l.setP2(QPoint(x, y));

    return l;
}
