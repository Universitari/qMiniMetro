#include "Button.h"
#include "Game.h"

Button::Button(int index) {

	_index = index;
	_addingTrain = false;
	_mousePos = QPoint(WINDOW_WIDTH - 60, MENU_HEIGHT);

	switch (_index) {
	case(-1):
		_type = TRAIN;
		_position = QPoint(WINDOW_WIDTH - 60, MENU_HEIGHT);
		break;
	default:
		_type = LINE;
		_position = QPoint(WINDOW_WIDTH / 2 + (floor(MAX_LINES / 2) - _index) * (-BUTTONS_SPACING - BUTTON_SIZE), MENU_HEIGHT);
		break;
	}

	_color = setColor(_index);
	
	setZValue(2);

}

void Button::paint(QPainter* painter, 
				   const QStyleOptionGraphicsItem* option, 
				   QWidget* widget){

	if (_type == LINE) {

		QPen pen;
		pen.setWidth(10);
		pen.setColor(_color);
		painter->setBrush(QBrush(_color));
		painter->setPen(pen);
		painter->setRenderHint(QPainter::Antialiasing);

        if(Game::instance()->lineExists(_index))
            painter->drawEllipse(_position, BUTTON_SIZE_BIG / 2, BUTTON_SIZE_BIG / 2);
        else
            painter->drawEllipse(_position, BUTTON_SIZE / 2, BUTTON_SIZE / 2);
	
	}
	else {
		
		QPen pen;
		pen.setWidth(10);
		pen.setColor(QColor(0, 0, 0));
		painter->setBrush(QBrush(QColor(0, 0, 0)));
		painter->setPen(pen);
		painter->setRenderHint(QPainter::Antialiasing);

		QPixmap train(":/Graphics/Train.png");

		painter->drawEllipse(_position, BUTTON_SIZE_BIG / 2, BUTTON_SIZE_BIG / 2);
		painter->drawPixmap(QPoint(_position.x() - BUTTON_SIZE_BIG / 2, _position.y() - BUTTON_SIZE_BIG / 2), train);

		if (_addingTrain) {

			pen.setWidth(1);
			pen.setColor(QColor(0, 0, 0));
			pen.setJoinStyle(Qt::MiterJoin);
			painter->setBrush(QBrush(QColor(0, 0, 0)));
			painter->setPen(pen);
			painter->drawRect(QRect(_mousePos.x() - TRAIN_HEIGHT/2, _mousePos.y() - TRAIN_WIDTH/2, TRAIN_HEIGHT, TRAIN_WIDTH));
		}
	}

}

QRectF Button::boundingRect() const{

	//da cambiare
	return QRectF(_position.x() - BUTTON_SIZE_BIG /2, _position.y() - BUTTON_SIZE_BIG / 2,
                  BUTTON_SIZE_BIG, BUTTON_SIZE_BIG);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent* e) {

	if (_type == LINE) {
		// printf("stai cliccando sul tasto %d\n", _index);
		Game::instance()->deleteLine(_index);
		QGraphicsItem::mousePressEvent(e);
	}
	else {
		if(Game::instance()->availableTrains())
			_addingTrain = true;
	}
		

}

void Button::mouseMoveEvent(QGraphicsSceneMouseEvent* e){

	if (_addingTrain) {
		
		_mousePos = QPoint(e->pos().x(), e->pos().y());
		update();
	}
	QGraphicsItem::mouseMoveEvent(e);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent* e){

	Game::instance()->addTrain(QRect(_mousePos.x() - TRAIN_HEIGHT / 2, _mousePos.y() - TRAIN_WIDTH / 2, TRAIN_HEIGHT, TRAIN_WIDTH));

	_addingTrain = false;
	_mousePos = QPoint(WINDOW_WIDTH - 60, MENU_HEIGHT);
}
