#include "Map.h"

/*Map* Map::instance() {

	static Map uniqueInstance;
	return &uniqueInstance;
}*/

Map* Map::uniqueInstance = 0;

Map* Map::instance() {

	if (uniqueInstance == 0)
		uniqueInstance = new Map;
	return uniqueInstance;
}

void Map::load(QString mapAddress, QGraphicsScene* scene) {

	QPixmap map(mapAddress);

	// Debug
	// bool x = map.isNull();
	// if (x) printf("map e' vuoto");
	// else printf("map non e' vuoto");

	scene->addPixmap(map);

}