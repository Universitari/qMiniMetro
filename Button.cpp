#include "Button.h"
#include "Game.h"

Button::Button(int index) {

	_index = index;

	switch (_index) {
	case(-1):
		_type = TRAIN;
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

}

QRectF Button::boundingRect() const{

	//da cambiare
	return QRectF(_position.x() - BUTTON_SIZE_BIG /2, _position.y() - BUTTON_SIZE_BIG / 2,
                  BUTTON_SIZE_BIG, BUTTON_SIZE_BIG);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent* e) {

    // printf("stai cliccando sul tasto %d\n", _index);
    Game::instance()->deleteLine(_index);
    QGraphicsItem::mousePressEvent(e);

}