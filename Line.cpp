#include "Line.h"

Line::Line(QPoint startP, int index) {

    _linePoints[0] = startP;
    _linePoints[1] = startP;
    _linePoints[2] = startP;
    _pointsCounter = 2;
    _circularLine = false;
    _name = Name(index);
    _state = INITIAL;
    setZValue(1);

    _path.moveTo(startP);
    _stations.push_back(startP);

    _color = setColor(_name);
}

Line::~Line(){

    printf("cancellata linea %d\n", int(_name));
    delete _TcapHitbox;
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


    if (_pointsCounter == 0) return;
    pen.setWidth(10);
    pen.setColor(_color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    //painter->drawLine(_line);
    //painter->drawPolyline(_linePoints, _pointsCounter+1);
    painter->drawPath(_path);
    painter->drawPath(_mousePath);

    pen.setCapStyle(Qt::SquareCap);
    painter->setPen(pen);

    if (_state != MOD_TAIL) {
        painter->drawLine(_neckTcapTail);
        painter->drawLine(_TcapTail);
    }
    if (_state != MOD_HEAD) {
        painter->drawLine(_neckTcapHead);
        painter->drawLine(_TcapHead);
    }

}

QRectF Line::boundingRect() const{

    return QRectF(QPointF(0, 0), QPointF(1920, 1080));
}

void Line::setNextPoint(QPoint nextP) {

    if (_state == MOD_TAIL || _state == INITIAL) {

        _path.moveTo(lastPoint());
        _path.lineTo(nextP);
        _stations.push_back(nextP);
    }
    else if (_state == MOD_HEAD) {

        _path.moveTo(startPoint());
        _path.lineTo(nextP);
        _stations.push_front(nextP);
    
    }

    /*
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
    }-
    printf("inseriti %d punti\n", _pointsCounter - 1);
    */
    printf("inseriti %d punti\n", _stations.size());
}

void Line::setCurrentPoint(QPoint currP) { 
    
    if (_state == MOD_TAIL || _state == INITIAL) {
        _mousePath.clear();
        _mousePath.moveTo(_stations.back());
        _mousePath.lineTo(currP);
    }
    else {
        _mousePath.clear();
        _mousePath.moveTo(_stations.front());
        _mousePath.lineTo(currP);
    }

    /*
    if (_state == MOD_TAIL || _state == INITIAL)
        _linePoints[_pointsCounter] = currP;
    else 
        _linePoints[0] = currP;
    */
}


bool Line::validPoint(QPoint p){

    if (!_circularLine && _stations.size() > 2) {
        if ((p == startPoint() && (_state == MOD_TAIL || _state == INITIAL)) ||
            (p == lastPoint() && _state == MOD_HEAD)){

            _circularLine = true;
            return true;
        }
    }

    /*for (int i = 0; i < _pointsCounter; i++) {
        if (p == _linePoints[i])
            return false;
    }*/

    for (auto& s : _stations) {
        if (p == s)
            return false;
    }

    return true;
}

QPoint Line::TcapPoint(QPoint p1, QPoint p2){

    float m = angularCoeff(p1, p2);
    float angle = atan(m);
    int length = 40;
    int x = cos(angle) * length;
    int y = sin(angle) * length;
 
    if (p1.x() > p2.x())
        x = -x;

    /*
    if(edgePoint == startPoint())
        return QPoint(edgePoint.x() - x, edgePoint.y() - y);
    else
        return QPoint(edgePoint.x() + x, edgePoint.y() + y);
    */

    return QPoint(p1.x() - x, p1.y() - y);
}

void Line::updateTcapPoint(){

    std::list<QPoint>::iterator iter = std::next(_stations.begin());

    if (_state == MOD_HEAD || _state == INITIAL) {
        //setCurrentPoint(startPoint());
        //_linePoints[0] = TcapPoint(_linePoints[1], _linePoints[2], _linePoints[0]);
        //_TcapHead = setTcap(_linePoints[1], _linePoints[0]);
        
        //_path.moveTo(startPoint());
        //_path.lineTo(TcapPoint(startPoint(), *iter));
        _neckTcapHead = QLine(startPoint(), TcapPoint(startPoint(), *iter));
        _TcapHead = setTcap(startPoint(), TcapPoint(startPoint(), *iter));
    }
    
    iter = std::prev(std::prev(_stations.end()));

    if (_state == MOD_TAIL || _state == INITIAL) {
        //setCurrentPoint(lastPoint());
        //_linePoints[_pointsCounter] = TcapPoint(_linePoints[_pointsCounter - 2], _linePoints[_pointsCounter - 1], _linePoints[_pointsCounter]);
        //_TcapTail = setTcap(_linePoints[_pointsCounter - 1], _linePoints[_pointsCounter]);
        
        //_path.moveTo(lastPoint());
        //_path.lineTo(TcapPoint(lastPoint(), *iter));
        _neckTcapTail = QLine(lastPoint(), TcapPoint(lastPoint(), *iter));
        _TcapTail = setTcap(lastPoint(), TcapPoint(lastPoint(), *iter));
    }

    _mousePath.clear();
    
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
    //printf("angular coeff: %f\n", m);
    float angle = atan(m);
    int length = 10;

    angle += PI / 2;
    int x = cos(angle) * length;
    int y = sin(angle) * length;

    if (_TcapHead.p1().x() > _TcapHead.p2().x())
        x = -x;

    _TcapHitbox = new QPolygon(4);

    _TcapHitbox->setPoint(0, _TcapHead.p1().x() - x, _TcapHead.p1().y() - y);
    _TcapHitbox->setPoint(1, _TcapHead.p1().x() + x, _TcapHead.p1().y() + y);
    _TcapHitbox->setPoint(2, _TcapHead.p2().x() + x, _TcapHead.p2().y() + y);
    _TcapHitbox->setPoint(3, _TcapHead.p2().x() - x, _TcapHead.p2().y() - y);

    pointerPos.setX(pointerPos.x() / GAME_SCALE);
    pointerPos.setY(pointerPos.y() / GAME_SCALE);
    
    if (_TcapHitbox->containsPoint(pointerPos, Qt::OddEvenFill) && !_circularLine) {
        //printf("stai premendo sul tcap di testa\n");
        _state = MOD_HEAD;
        return true;
    }

    m = angularCoeff(_TcapTail.p1(), _TcapTail.p2());
    angle = atan(m);
    angle += PI / 2;

    x = cos(angle) * length;
    y = sin(angle) * length;

    if (_TcapTail.p1().x() > _TcapTail.p2().x())
        x = -x;

    _TcapHitbox->setPoint(0, _TcapTail.p1().x() - x, _TcapTail.p1().y() - y);
    _TcapHitbox->setPoint(1, _TcapTail.p1().x() + x, _TcapTail.p1().y() + y);
    _TcapHitbox->setPoint(2, _TcapTail.p2().x() + x, _TcapTail.p2().y() + y);
    _TcapHitbox->setPoint(3, _TcapTail.p2().x() - x, _TcapTail.p2().y() - y);

    if (_TcapHitbox->containsPoint(pointerPos, Qt::OddEvenFill) && !_circularLine) {
        //printf("stai premendo sul tcap di coda\n");
        _state = MOD_TAIL;
        return true;
    }
        
    return false;
}
