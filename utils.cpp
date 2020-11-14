#include "utils.h"

bool pointerOnStation(Station* s, QPoint pointerPos) {

	if (pointerPos.x() >= s->position().x() * GAME_SCALE &&
		pointerPos.x() <= (s->position().x() + STATION_SIZE) * GAME_SCALE &&
		pointerPos.y() >= s->position().y() * GAME_SCALE &&
		pointerPos.y() <= (s->position().y() + STATION_SIZE) * GAME_SCALE)
			return true;
	else 
		return false;
}