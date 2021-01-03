#include "utils.h"

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

    if (distance(spawnPoint, QPoint(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2)) < (200 + stationsNum * 10))
        return true;
    else
        return false;
}
