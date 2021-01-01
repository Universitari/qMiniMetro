#include "Train.h"

Train::Train(int lineIndex, int index, QPoint _centerPoint, QPainterPath linePath){

	_maxPass = 6;
	_lineIndex = lineIndex;
	_index = index;
	_color = setColor(_lineIndex);
	_path = linePath;
	_oldPath = _path;
	_rotationAngle = 90;
	_increment = 0;
	_circular = false;
	_length = _path.length();
	_direction = FORWARD;
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

	if(_path.elementAt(0) != _oldPath.elementAt(0) && 
	  (_direction == FORWARD || _direction == BACKWARD))
		_increment += _path.length() - _length;

	_length = _path.length();
	_oldPath = _path;
	
	float t = _path.percentAtLength(_increment);

	if (!_circular) {
		if (t == 1)
			_direction = BACKWARD;
		else if (t == 0)
			_direction = FORWARD;
	}
	else
		if (t == 1 && _direction == FORWARD)
			_increment = 0;
		else if (t == 0 && _direction == BACKWARD)
			_increment = _path.length();

	QLineF _line = QLineF(_trainRect->center(), _path.pointAtPercent(t));
	_trainRect->translate(_line.dx(), _line.dy());
	// float m = _path.slopeAtPercent(t);
	_rotationAngle = 90 - _path.angleAtPercent(t);
	
	if (_direction == BACKWARD)
		_increment -= TRAIN_SPEED;
	else 
		_increment += TRAIN_SPEED;
}
