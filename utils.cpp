#include "Utils.h"
#include "Game.h"

float angularCoeff(QPoint p1, QPoint p2) {

	float x1 = p1.x();
	float y1 = p1.y();
	float x2 = p2.x();
	float y2 = p2.y();

	float m = (y2 - y1) / abs(x2 - x1);

	//printf("P1: %.2f, %.2f\nP2: %.2f, %.2f\nangular coefficient: %f\n",
	//	x1, y1, x2, y2, m);

	return m;
}

float distance(QPoint p1, QPoint p2){
	return sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
}

QColor setColor(int index){

    switch (index) {
    case(0):
        // Yellow
        return QColor(255, 206, 0);
    case(1):
        // Red
        return QColor(220, 36, 31);
    case(2):
        // Dark blue
        return QColor(0, 25, 168);
    case(3):
        // Blue
        return QColor(0, 160, 255);
    case(4):
        // Green
        return QColor(0, 114, 41);
    case(5):
        // Pink
        return QColor(215, 153, 175);
    case(6):
        // Brown
        return QColor(137, 78, 36);
    default:
        // Black
        return QColor(0, 0, 0);
    }
}

void rotate(QPainter* p, const QRect& r, qreal angle) {
    p->translate(r.center());
    p->rotate(angle);
    p->translate(-r.center());
}

bool spawnAreaAvailable(QPoint spawnPoint, int stationsNum){

    if (distance(spawnPoint, QPoint(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2)) < (300 + stationsNum * 30))
        return true;
    else
        return false;
}

QPoint nextPointOnLine(QPoint p1, QPoint p2, int length) {

    float m = angularCoeff(p1, p2);
    float angle = atan(m);
    int x = cos(angle) * length;
    int y = sin(angle) * length;

    if (p1.x() > p2.x())
        x = -x;

    return QPoint(p1.x() - x, p1.y() - y);
}

QPoint spawnPos(){

    QPoint spawnPoint;
    int xRand = rand() % ((WINDOW_WIDTH - 4 * STATION_SIZE - PASSENGER_SIZE * (MAX_PASS_STATION / 2)) / STATION_GRID);
    xRand *= STATION_GRID;
    int yRand = rand() % ((WINDOW_HEIGHT - 8 * STATION_SIZE) / STATION_GRID);
    yRand *= STATION_GRID;
    spawnPoint.setX(2 * STATION_SIZE + xRand);
    spawnPoint.setY(2 * STATION_SIZE + 5 + yRand);

    return spawnPoint;
}

QPolygonF triangle(QPoint pos, int size){

    QPolygonF triangle;

    triangle << QPointF(pos.x() + size / 2, pos.y())
             << QPointF(pos.x(), pos.y() + size)
             << QPointF(pos.x() + size, pos.y() + size);

    return triangle;
}

QPolygonF star(QPoint pos, int size){

    QPolygonF star;
    float r = size / 3.55;
    float R = size / 1.55;

    for (int i = 0; i < 5; i++) {
        star << QPointF(pos.x() + size / 2 + R * std::cos(PI / 2 + 2 * PI / 5 * i),
                        pos.y() + size / 2 - R * std::sin(PI / 2 + 2 * PI / 5 * i))
            
             << QPointF(pos.x() + size / 2 + r * std::cos(PI / 2 + (2 * PI) / 10 + 2 * PI / 5 * i),
                        pos.y() + size / 2 - r * std::sin(PI / 2 + (2 * PI) / 10 + 2 * PI / 5 * i));
    }

    return star;
}

QPolygonF pentagon(QPoint pos, int size){

    QPolygonF pentagon;
    float R = size / 1.7;

    for (int i = 0; i < 5; i++) {
        pentagon << QPointF(pos.x() + size / 2 + R * std::cos(PI / 2 + 2 * PI / 5 * i),
                            pos.y() + size / 2 - R * std::sin(PI / 2 + 2 * PI / 5 * i));
    }

    return pentagon;
}

QPolygonF rhombus(QPoint pos, int size){

    QPolygonF rhombus;
    float R = size / 1.7;

    for (int i = 0; i < 4; i++) {
        rhombus << QPointF(pos.x() + size / 2 + R * std::cos(PI / 2 + 2 * PI / 4 * i),
                           pos.y() + size / 2 - R * std::sin(PI / 2 + 2 * PI / 4 * i));
    }

    return rhombus;
}

QPolygonF cross(QPoint pos, int size){

    QPolygonF cross;
    float n = size * 2 / 3;

    cross << QPointF(pos.x() + n / 2, pos.y())
          << QPointF(pos.x() + n, pos.y())
          << QPointF(pos.x() + n, pos.y() + n / 2)
          << QPointF(pos.x() + n + n / 2, pos.y() + n / 2)
          << QPointF(pos.x() + n + n / 2, pos.y() + n)
          << QPointF(pos.x() + n, pos.y() + n)
          << QPointF(pos.x() + n, pos.y() + n + n / 2)
          << QPointF(pos.x() + n / 2, pos.y() + n + n / 2)
          << QPointF(pos.x() + n / 2, pos.y() + n)
          << QPointF(pos.x(), pos.y() + n)
          << QPointF(pos.x(), pos.y() + n / 2)
          << QPointF(pos.x() + n / 2, pos.y() + n / 2);

    return cross;
}

QPolygonF diamond(QPoint pos, int size){

    QPolygonF diamond;

    float n = size / 3;
    float offset = size * 0.1;

    diamond << QPointF(pos.x() + n / 2, pos.y() + offset)
            << QPointF(pos.x() + size - n / 2, pos.y() + offset)
            << QPointF(pos.x() + size, pos.y() + n / 2 + offset)
            << QPointF(pos.x() + size/ 2, pos.y() + size* 0.9)
            << QPointF(pos.x(), pos.y() + n / 2 + offset);

    return diamond;
}
