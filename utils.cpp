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
