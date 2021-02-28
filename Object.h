#pragma once

#include <QGraphicsItem>
#include "config.h"

// base class for all game objects
class Object : public QGraphicsItem{

public:

	Object() {};

	// advance (=compute next location)
	virtual void advance() = 0;

};
