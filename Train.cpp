#include "Train.h"

Train::Train(int index, QPoint _centerPoint){

	_maxPass = 6;
	_lineIndex = index;
	_color = setColor(_lineIndex);
	_rotationAngle = 0;
	_trainRect = new QRect(_centerPoint.x() - TRAIN_WIDTH / 2 + STATION_SIZE/2, 
						   _centerPoint.y() - TRAIN_HEIGHT / 2 + STATION_SIZE / 2,
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
