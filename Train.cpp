#include "Train.h"

Train::Train(int lineIndex, int index, QPoint centerPoint, QPainterPath linePath){

	_passengers = 0;
	_lineIndex = lineIndex;
	_index = index;
	_color = setColor(_lineIndex);
	_path = linePath;
	_oldPath = _path;
	_rotationAngle = 90;
	_increment = 0;
	_circular = false;
	_colliding = false;
	_speedMultiplier = 1;
	_length = _path.length();
	_direction = FORWARD;
	_state = MOVING;
	_trainRect = new QRect(centerPoint.x() - TRAIN_WIDTH / 2, 
						   centerPoint.y() - TRAIN_HEIGHT / 2,
						   TRAIN_WIDTH, TRAIN_HEIGHT);
	setZValue(2);

}

void Train::paint(QPainter* painter, 
	const QStyleOptionGraphicsItem* option, 
	QWidget* widget){

	QPen pen;
	painter->setRenderHint(QPainter::Antialiasing);
	pen.setWidth(1);
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

	if (_state == STOPPED)
		_speedMultiplier = 0;

	/*if (_state == DEPARTING) {
		_speedMultiplier += 0.008;
		if (_speedMultiplier > 1)
			_state = MOVING;
	}

	if (_state == BRAKING) {
		_speedMultiplier -= 0.008;
		if (_speedMultiplier <= 0) {
			_speedMultiplier = 0;
			//_state = STOPPED;
		}
	}*/

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

	QLineF line = QLineF(_trainRect->center(), _path.pointAtPercent(t));
	_trainRect->translate(line.dx(), line.dy());
	// float m = _path.slopeAtPercent(t);
	_rotationAngle = 90 - _path.angleAtPercent(t);
	
	if (_direction == BACKWARD)
		_increment -= TRAIN_SPEED * _speedMultiplier;
	else 
		_increment += TRAIN_SPEED * _speedMultiplier;
}

QPoint Train::passengerPos(int ticket){
	
	switch (ticket) {
		case 1: {
			return QPoint(position().x() - (PASSENGER_SIZE) - 1, position().y() - 1.5 * PASSENGER_SIZE - 2);
			break;
		}
		case 2: {
			return QPoint(position().x() + 3, position().y() - 1.5 * PASSENGER_SIZE - 2);
			break;
		}
		case 3: {
			return QPoint(position().x() - (PASSENGER_SIZE) - 1, position().y() - PASSENGER_SIZE / 2);
			break;
		}
		case 4: {
			return QPoint(position().x() + 3, position().y() - PASSENGER_SIZE / 2);
			break;
		}
		case 5: {
			return QPoint(position().x() - (PASSENGER_SIZE) - 1, position().y() + PASSENGER_SIZE/2 + 2);
			break;
		}
		case 6: {
			return QPoint(position().x() + 3, position().y() + PASSENGER_SIZE/2 + 2);
			break;
		}
		default:
			return QPoint(10, 10);
			break;
	}
}
