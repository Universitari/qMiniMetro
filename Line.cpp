#include "Line.h"

int Line::_linesNumber = 0;

Line::Line(QPoint startP){

    _linePoints[0] = startP;
    _linePoints[1] = startP;
    _linePoints[2] = startP;
    _pointsCounter = 2;
    _circularLine = false;
    _name = Name(_linesNumber);
    _state = INITIAL;
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
    if(_state != MOD_TAIL)
        painter->drawLine(_TcapTail);
    if(_state != MOD_HEAD)
        painter->drawLine(_TcapHead);

}

QRectF Line::boundingRect() const{

    return QRectF(QPointF(0, 0), QPointF(1920, 1080));
}

void Line::setNextPoint(QPoint nextP) {

    if (_state == MOD_TAIL || _state == INITIAL) {

        _linePoints[_pointsCounter] = nextP;
        _pointsCounter++;
        _linePoints[_pointsCounter] = nextP;
    }
    else if (_state == MOD_HEAD) {
        
        for (int i = _pointsCounter; i > 0; i--) {
            _linePoints[i + 1] = _linePoints[i];
        }
        _linePoints[0] = nextP;
        _linePoints[1] = nextP;
        _pointsCounter++;
    }
    printf("inseriti %d punti\n", _pointsCounter - 1);
}

void Line::setCurrentPoint(QPoint currP) { 
    
    if (_state == MOD_TAIL || _state == INITIAL)
        _linePoints[_pointsCounter] = currP;
    else 
        _linePoints[0] = currP;
}


bool Line::validPoint(QPoint p){

    if (!_circularLine && _pointsCounter > 3) {
        if ((p == startPoint() && (_state == MOD_TAIL || _state == INITIAL)) ||
            (p == lastPoint() && _state == MOD_HEAD)){

            _circularLine = true;
            return true;
        }
    }

    for (int i = 0; i < _pointsCounter; i++) {
        if (p == _linePoints[i])
            return false;
    }

    return true;
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

    if (_state == MOD_HEAD || _state == INITIAL) {
        setCurrentPoint(startPoint());
        _linePoints[0] = TcapPoint(_linePoints[1], _linePoints[2], _linePoints[0]);
        _TcapHead = setTcap(_linePoints[1], _linePoints[0]);
    }

    if (_state == MOD_TAIL || _state == INITIAL) {
        setCurrentPoint(lastPoint());
        _linePoints[_pointsCounter] = TcapPoint(_linePoints[_pointsCounter - 2], _linePoints[_pointsCounter - 1], _linePoints[_pointsCounter]);
        _TcapTail = setTcap(_linePoints[_pointsCounter - 1], _linePoints[_pointsCounter]);
    }

    _state = INITIAL;
}

QLine Line::setTcap(QPoint p1, QPoint p2){

    QLine l;

    float m = angularCoeff(p1, p2);

    float angle = atan(m) + PI / 2;
    int length = 15;
    int x = p2.x() + cos(angle) * length;
    int y = p2.y() + sin(angle) * length;

    if (p1.x() > p2.x())
        x = p2.x() - (cos(angle) * length);

    l.setP1(QPoint(x, y));

    angle = atan(m) - PI / 2;
    x = p2.x() + cos(angle) * length;
    y = p2.y() + sin(angle) * length;

    if ( p1.x() > p2.x())
        x = p2.x() - (cos(angle) * length);

    l.setP2(QPoint(x, y));

    return l;
}

bool Line::pointerOnCap(QPoint pointerPos){

    float m = angularCoeff(_TcapHead.p1(), _TcapHead.p2());
    float angle = atan(m);
    int length = 20;

    angle += PI / 2;
    int x = cos(angle) * length;
    int y = sin(angle) * length;

    QPolygon Tcap(4);

    Tcap.setPoint(0, _TcapHead.p1().x() + x, _TcapHead.p1().y() - y);
    Tcap.setPoint(1, _TcapHead.p1().x() - x, _TcapHead.p1().y() + y);
    Tcap.setPoint(2, _TcapHead.p2().x() - x, _TcapHead.p2().y() + y);
    Tcap.setPoint(3, _TcapHead.p2().x() + x, _TcapHead.p2().y() - y);

    pointerPos.setX(pointerPos.x() / GAME_SCALE);
    pointerPos.setY(pointerPos.y() / GAME_SCALE);
    
    if (Tcap.containsPoint(pointerPos, Qt::OddEvenFill) && !_circularLine) {
        printf("stai premendo sul tcap di testa\n");
        _state = MOD_HEAD;
        return true;
    }

    m = angularCoeff(_TcapTail.p1(), _TcapTail.p2());
    angle = atan(m);
    angle += PI / 2;

    x = cos(angle) * length;
    y = sin(angle) * length;

    Tcap.setPoint(0, _TcapTail.p1().x() + x, _TcapTail.p1().y() - y);
    Tcap.setPoint(1, _TcapTail.p1().x() - x, _TcapTail.p1().y() + y);
    Tcap.setPoint(2, _TcapTail.p2().x() - x, _TcapTail.p2().y() + y);
    Tcap.setPoint(3, _TcapTail.p2().x() + x, _TcapTail.p2().y() - y);

    if (Tcap.containsPoint(pointerPos, Qt::OddEvenFill) && !_circularLine) {
        printf("stai premendo sul tcap di coda\n");
        _state = MOD_TAIL;
        return true;
    }
        
    return false;
}
