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

    switch (_index) {
    case(0):
        // Yellow
        _color = QColor(255, 206, 0);
        break;
    case(1):
        // Red
        _color = QColor(220, 36, 31);
        break;
    case(2):
        // Dark blue
        _color = QColor(0, 25, 168);
        break;
    case(3):
        // Blue
        _color = QColor(0, 160, 255);
        break;
    case(4):
        // Green
        _color = QColor(0, 114, 41);
        break;
    case(5):
        // Pink
        _color = QColor(215, 153, 175);
        break;
    case(6):
        // Brown
        _color = QColor(137, 78, 36);
        break;
    }
	
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