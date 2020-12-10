#include "Train.h"

Train::Train(int index, QPoint _centerPoint, QPainterPath linePath){

	_maxPass = 6;
	_lineIndex = index;
	_color = setColor(_lineIndex);
	_path = linePath;
	_rotationAngle = 90;
	_increment = 0;
	_trainRect = new QRect(_centerPoint.x() - TRAIN_WIDTH / 2, 
						   _centerPoint.y() - TRAIN_HEIGHT / 2,
						   TRAIN_WIDTH, TRAIN_HEIGHT);
	setZValue(2);

}

void Train::paint(QPainter* painter, 
	const QStyleOptionGraphicsItem* option, 
	QWidget* widget){

	QPen pen;
	painter->setRenderHint(QPainter::Antialiasing);
	pen.setWidth(10);
	pen.setColor(_color);
	pen.setJoinStyle(Qt::MiterJoin);
	painter->setBrush(QBrush(_color));
	painter->setPen(pen);
	
	rotate(painter, *_trainRect, _rotationAngle);
	painter->drawRect(*_trainRect);
	//painter->drawPolygon(_trainPolygon);

}

QRectF Train::boundingRect() const{

	return QRectF(*_trainRect);
}

void Train::advance(){


	float t = _path.percentAtLength(_increment);
	QLineF _line = QLineF(_trainRect->center(), _path.pointAtPercent(t));
	_trainRect->translate(_line.dx(), _line.dy());
	//float m = _path.slopeAtPercent(t);
	_rotationAngle = _path.angleAtPercent(t);
	_increment += TRAIN_SPEED;

}
