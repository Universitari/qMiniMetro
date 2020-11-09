#include "Object.h"
#include "Game.h"

Object::Object() : QGraphicsItem(0){

	_animation_counter = 0;
	_animation_divisor = GAME_FPS / 10;
}