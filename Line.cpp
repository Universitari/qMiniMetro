#include "Line.h"

Line::Line(QPoint startP, int index) {

    _circularLine = false;
    _deleting = false;
    _name = Name(index);
    _state = INITIAL;
    setZValue(1);

    _path.moveTo(startP);
    _stations.push_back(startP);

    _color = setColor(_name);
}

Line::~Line(){

    printf("cancellata linea %d\n", int(_name));

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

    if (_deleting)
        painter->setOpacity(0.5);
    else
        painter->setOpacity(1);

    if (_stations.size() == 0) return;
    pen.setWidth(LINE_WIDTH);
    pen.setColor(_color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    painter->drawPath(_path);
    painter->drawPath(_mousePath);

    pen.setCapStyle(Qt::SquareCap);
    painter->setPen(pen);

    if (_circularLine) {
        painter->drawLine(_neckTcapTail);
        painter->drawLine(_TcapTail);
    }
    else {

        if (_state != MOD_TAIL) {
            painter->drawLine(_neckTcapTail);
            painter->drawLine(_TcapTail);
        }

        if (_state != MOD_HEAD) {
            painter->drawLine(_neckTcapHead);
            painter->drawLine(_TcapHead);
        }
    }
}

QRectF Line::boundingRect() const{

    return QRectF(QPointF(0, 0), QPointF(1920, 1080));
}

void Line::read(const QJsonObject& json){

    if (json.contains("Circular line") && json["Circular line"].isBool())
        _circularLine = json["Circular line"].toBool();

    if (json.contains("State") && json["State"].isDouble())
        _state = State(json["State"].toInt());

    if (json.contains("Stations") && json["Stations"].isArray()) {
        QJsonArray stationsArray = json["Stations"].toArray();

        _stations.clear();

        QPoint p;
        QJsonObject obj = stationsArray.at(0).toObject();

        p.setX(obj["x"].toInt());
        p.setY(obj["y"].toInt());

        _path.moveTo(p);
        _stations.push_back(p);

        for (int i = 1; i < stationsArray.size(); i++) {

            obj = stationsArray.at(i).toObject();

            p.setX(obj["x"].toInt());
            p.setY(obj["y"].toInt());

            setNextPoint(p);
        }

        updateTcapPoint();
    }

}

void Line::write(QJsonObject& json) const{

    json["Circular line"] = _circularLine;
    json["State"] = int(_state);

    QJsonArray stationsArray;
    for (auto& s : _stations) {
        QJsonObject stationObj;
        stationObj["x"] = s.x();
        stationObj["y"] = s.y();
        stationsArray.append(stationObj);
    }
    json["Stations"] = stationsArray;

}

void Line::setNextPoint(QPoint nextP) {

    if (_state == MOD_TAIL || _state == INITIAL)
        _stations.push_back(nextP);

    else if (_state == MOD_HEAD)
        _stations.insert(_stations.begin(), nextP);

    _path.clear();
    
    std::vector<QPoint>::iterator iter = _stations.begin();
    _path.moveTo(*iter);

    QPoint p;

    while (iter != std::prev(_stations.end())) {
    
        if ((*iter).x() == (*std::next(iter)).x() || (*iter).y() == (*std::next(iter)).y())
            _path.lineTo(*std::next(iter));
        else if(angularCoeff(*iter, *std::next(iter)) == 1 || angularCoeff(*iter, *std::next(iter)) == -1)
            _path.lineTo(*std::next(iter));
        else {
            _path.lineTo(nextPointOnLine(middlePoint(*iter, *std::next(iter)), *iter, MIDPOINT_CURVE));
            _path.quadTo(middlePoint(*iter, *std::next(iter)), nextPointOnLine(middlePoint(*iter, *std::next(iter)), *std::next(iter), MIDPOINT_CURVE));
            if (std::next(std::next(iter)) != _stations.end()) {
                p = nextPointOnLine(*std::next(iter), middlePoint(*iter, *std::next(iter)), STATION_CURVE);
                _path.lineTo(p);

                p = nextPointOnLine(*std::next(iter), *std::next(std::next(iter)), STATION_CURVE);
                _path.quadTo(*std::next(iter), p);
            }
            else _path.lineTo(*std::next(iter));
        }

        iter = std::next(iter);
    }

    // printf("inseriti %d punti\n", _stations.size());
}

void Line::setCurrentPoint(QPoint currP) { 
    
    /*if (_state == MOD_TAIL || _state == INITIAL) {
        _mousePath.clear();
        _mousePath.moveTo(_stations.back());
        _mousePath.lineTo(currP);
    }
    else {
        _mousePath.clear();
        _mousePath.moveTo(_stations.front());
        _mousePath.lineTo(currP);
    }*/
    if (_state == MOD_TAIL || _state == INITIAL) {
        _mousePath.clear();
        _mousePath.moveTo(_stations.back());

        if(middlePoint(_stations.back(), currP) == currP)
            _mousePath.lineTo(currP);
        else {
            _mousePath.lineTo(nextPointOnLine(middlePoint(_stations.back(), currP), _stations.back(), MIDPOINT_CURVE));
            _mousePath.quadTo(middlePoint(_stations.back(), currP), nextPointOnLine(middlePoint(_stations.back(), currP), currP, MIDPOINT_CURVE));
            _mousePath.lineTo(currP);
        }
    }
    else {
        _mousePath.clear();
        _mousePath.moveTo(_stations.front());

        if (middlePoint(_stations.front(), currP) == currP)
            _mousePath.lineTo(currP);
        else {
            _mousePath.lineTo(nextPointOnLine(middlePoint(_stations.front(), currP), _stations.front(), -15));
            _mousePath.quadTo(middlePoint(_stations.front(), currP), nextPointOnLine(middlePoint(_stations.front(), currP), currP, -15));
            _mousePath.lineTo(currP);
        }
    }
}

bool Line::validPoint(QPoint p){

    if (!_circularLine && _stations.size() > 2) {
        if ((p == firstPoint() && (_state == MOD_TAIL || _state == INITIAL)) ||
            (p == lastPoint() && _state == MOD_HEAD)){

            _circularLine = true;
            return true;
        }
    }

    for (auto& s : _stations) {
        if (p == s)
            return false;
    }

    return true;
}

void Line::updateTcapPoint(){

    std::vector<QPoint>::iterator iter = std::next(_stations.begin());

    QPoint point;

    if (sector(firstPoint(), *iter) == -1) // orizzontale o verticale
        point = nextPointOnLine(firstPoint(), *iter, TCAP_DISTANCE);
    else if (sector(firstPoint(), *iter) == -2) // diagonale
        point = nextPointOnLine(firstPoint(), *iter, TCAP_DISTANCE);
    else // non è orizzontale, verticale o diagonale
        point = nextPointOnLine(firstPoint(), middlePoint(firstPoint(), *iter), TCAP_DISTANCE);

    if (_state == MOD_HEAD || _state == INITIAL) {

        _neckTcapHead = QLine(firstPoint(), point);
        _TcapHead = setTcap(firstPoint(), point);
    }
    
    iter = std::prev(std::prev(_stations.end()));

    if (sector(lastPoint(), *iter) == -1) // orizzontale o verticale
        point = nextPointOnLine(lastPoint(), *iter, TCAP_DISTANCE);
    else if (sector(lastPoint(), *iter) == -2) // diagonale
        point = nextPointOnLine(lastPoint(), *iter, TCAP_DISTANCE);
    else // non è orizzontale, verticale o diagonale
        point = nextPointOnLine(lastPoint(), middlePoint(*iter, lastPoint()), TCAP_DISTANCE);

    if (_state == MOD_TAIL || _state == INITIAL) {

        _neckTcapTail = QLine(lastPoint(), point);
        _TcapTail = setTcap(lastPoint(), point);
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
        _state = MOD_TAIL;
        return true;
    }
        
    return false;
}

int Line::sector(QPoint s, QPoint p) {

    if (p.x() == s.x() || p.y() == s.y())
        return -1;
    else if (angularCoeff(s, p) == 1 || angularCoeff(s, p) == -1)
        return -2;
    else if (p.x() > s.x() && p.y() > s.y())
        return 1;
    else if (p.x() < s.x() && p.y() > s.y())
        return 2;
    else if (p.x() < s.x() && p.y() < s.y())
        return 3;
    else if (p.x() > s.x() && p.y() < s.y())
        return 4;
    
}

QPoint Line::middlePoint(QPoint s, QPoint p){

    QLineF line1(s, p);
    QLineF line2;

    int quadrant = sector(s, p);

    switch (quadrant) {
    case(1): {

        if (line1.dx() > line1.dy()) {

            line2.setP1(QPoint(p.x() - (p.y() - s.y()), s.y()));
            line2.setP2(p);

            line1.setP1(QPoint(s.x() + (p.y() - s.y()), p.y()));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(p.x() - (p.y() - s.y()), s.y());
            else return QPoint(s.x() + (p.y() - s.y()), p.y());
        }
        else {

            line2.setP1(QPoint(s.x(), p.y() - (p.x() - s.x())));
            line2.setP2(p);

            line1.setP1(QPoint(p.x(), s.y() + (p.x() - s.x())));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(s.x(), p.y() - (p.x() - s.x()));
            else return QPoint(p.x(), s.y() + (p.x() - s.x()));
        }
    }

    case(2): {

        if (abs(line1.dx()) > abs(line1.dy())) {

            line2.setP1(QPoint(p.x() + (p.y() - s.y()), s.y()));
            line2.setP2(p);

            line1.setP1(QPoint(s.x() - (p.y() - s.y()), p.y()));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(p.x() + (p.y() - s.y()), s.y());
            else return QPoint(s.x() - (p.y() - s.y()), p.y());
        }
        else {

            line2.setP1(QPoint(s.x(), p.y() - (s.x() - p.x())));
            line2.setP2(p);

            line1.setP1(QPoint(p.x(), s.y() + (s.x() - p.x())));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(s.x(), p.y() - (s.x() - p.x()));
            else return QPoint(p.x(), s.y() + (s.x() - p.x()));
        }
    }

    case(3): {

        if (abs(line1.dx()) > abs(line1.dy())) {

            line2.setP1(QPoint(p.x() - (p.y() - s.y()), s.y()));
            line2.setP2(p);

            line1.setP1(QPoint(s.x() + (p.y() - s.y()), p.y()));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(p.x() - (p.y() - s.y()), s.y());
            else return QPoint(s.x() + (p.y() - s.y()), p.y());
        }
        else {

            line2.setP1(QPoint(s.x(), p.y() + (s.x() - p.x())));
            line2.setP2(p);

            line1.setP1(QPoint(p.x(), s.y() - (s.x() - p.x())));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(s.x(), p.y() + (s.x() - p.x()));
            else return QPoint(p.x(), s.y() - (s.x() - p.x()));
        }
    }
    case(4): {

        if (abs(line1.dx()) > abs(line1.dy())) {

            line2.setP1(QPoint(p.x() + (p.y() - s.y()), s.y()));
            line2.setP2(p);

            line1.setP1(QPoint(s.x() - (p.y() - s.y()), p.y()));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(p.x() + (p.y() - s.y()), s.y());
            else return QPoint(s.x() - (p.y() - s.y()), p.y());
        }
        else {

            line2.setP1(QPoint(s.x(), p.y() + (p.x() - s.x())));
            line2.setP2(p);

            line1.setP1(QPoint(p.x(), s.y() + (s.x() - p.x())));
            line1.setP2(p);

            if (line2.length() < line1.length())
                return QPoint(s.x(), p.y() + (p.x() - s.x()));
            else return QPoint(p.x(), s.y() + (s.x() - p.x()));
        }
    }
    case(-1): return p;
    case(-2): return p;
    }
}

bool Line::pathColliding(QRect rect){

    float i = 0;

    while( i < 1){
        if (rect.contains(QPoint(_path.pointAtPercent(i).x(), _path.pointAtPercent(i).y())))
            return true;
        i += 0.001;
    }

    return false;
}
