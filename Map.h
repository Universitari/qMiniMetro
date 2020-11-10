#pragma once

#include <QGraphicsScene>
#include <QPixmap>
#include <QString>

class Map {

private:

public:

	// Singleton
	static Map* uniqueInstance;
	static Map* instance();

	void load(QString mapAddress, QGraphicsScene* scene);

};