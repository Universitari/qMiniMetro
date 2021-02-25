#include "Station.h"
#include "Game.h"

Station::Station(QPoint pos, int index, int shape) {

	_position = pos;

	if (shape == -1) {
		int randomShape = 1 + rand() % 100;
		if (randomShape <= 40)
			_shape = Shape(0);
		else if (randomShape <= 70)
			_shape = Shape(1);
		else if (randomShape <= 90)
			_shape = Shape(2);
		else if (randomShape <= 100)
			_shape = Shape(3);
	}
	else
		_shape = Shape(shape);

	_index = index;
	_currentPass = 0;
	setZValue(4);
}

Station::Station(const Station& s) {

	_position = s._position;
	_currentPass = s._currentPass;
	_shape = s._shape;
}

void Station::paint(QPainter* painter,
	const QStyleOptionGraphicsItem* option,
	QWidget* widget) {

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(5);
	painter->setBrush(QBrush(QColor(255, 255, 255)));
	pen.setJoinStyle(Qt::MiterJoin);

	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);
	
	if (_shape == SQUARE) {

		painter->drawRect(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
	}
	else if (_shape == TRIANGLE) {

		QPolygon triangle;
		triangle	<< QPoint(_position.x() + STATION_SIZE/2, _position.y())
					<< QPoint(_position.x(), _position.y() + STATION_SIZE)
					<< QPoint(_position.x() + STATION_SIZE, _position.y() + STATION_SIZE);
		painter->drawPolygon(triangle);
	}
	else if (_shape == CIRCLE) {

		painter->drawEllipse(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
	}
	else if (_shape == STAR) {

		QPolygonF starPolygon;
		float r = STATION_SIZE / 3.55;
		float R = STATION_SIZE / 1.55;

		for (int i = 0; i < 5; i++) {
			starPolygon << QPointF(_position.x() + STATION_SIZE/2 + R * std::cos(PI/2 + 2*PI/5 * i),
								   _position.y() + STATION_SIZE / 2 - R * std::sin(PI/2 + 2*PI/5 * i));
			
			starPolygon << QPointF(_position.x() + STATION_SIZE / 2 + r * std::cos(PI/2+(2*PI)/10 + 2*PI/5 * i),
								   _position.y() + STATION_SIZE / 2 - r * std::sin(PI / 2 + (2 * PI) / 10 + 2 * PI / 5 * i));
		}

		pen.setWidth(4);
		painter->setPen(pen);
		painter->drawPolygon(starPolygon);
	}
}

QRectF Station::boundingRect() const{

	qreal penWidth = 1;
	return QRectF(_position.x(), _position.y(), STATION_SIZE, STATION_SIZE);
}

void Station::read(const QJsonObject& json){

	if (json.contains("Shape") && json["Shape"].isDouble())
		_shape = Shape(json["Shape"].toInt());

	if (json.contains("Index") && json["Index"].isDouble())
		_index = json["Index"].toInt();	
		
	if (json.contains("Position (x)") && json["Position (x)"].isDouble())
		_position.setX(json["Position (x)"].toInt());

	if (json.contains("Position (y)") && json["Position (y)"].isDouble())
		_position.setY(json["Position (y)"].toInt());

}

void Station::write(QJsonObject& json) const{

	json["Shape"] = int(_shape);
	json["Index"] = _index;
	json["Position (x)"] = _position.x();
	json["Position (y)"] = _position.y();

}

bool Station::pointerOnStation(QPoint pointerPos) {

	if (pointerPos.x() >= position().x() * GAME_SCALE &&
		pointerPos.x() <= (position().x() + STATION_SIZE) * GAME_SCALE &&
		pointerPos.y() >= position().y() * GAME_SCALE &&
		pointerPos.y() <= (position().y() + STATION_SIZE) * GAME_SCALE)
		return true;
	else
		return false;
}