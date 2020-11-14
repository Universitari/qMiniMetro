#pragma once

#include "Station.h"

bool inline pointerOnStation(Station* s, QPoint pointerPos) {

	if (pointerPos.x() >= s->boundingRect().x() &&
		pointerPos.x() <= (s->boundingRect().x() + 30)) {
		if (pointerPos.y() >= s->boundingRect().y() &&
			pointerPos.y() <= (s->boundingRect().y() + 30))
			return true;
	} 
	else 
		return false;

}