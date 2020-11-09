#include "Map.h"

Map* Map::instance() {

	static Map uniqueInstance;
	return &uniqueInstance;
}