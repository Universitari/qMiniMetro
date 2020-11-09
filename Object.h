#pragma once

#include "config.h"
#include <QGraphicsItem>

// base class for all game objects
class Object : public QGraphicsItem{

protected:

	int _animation_counter;
	int _animation_divisor;

public:

	Object();

	// object name
	virtual std::string name() = 0;

	// animate (=compute next texture)
	virtual void animate() = 0;
	virtual void nextAnimation() { _animation_counter++; }

	// advance (=compute next location)
	virtual void advance() = 0;

	// solve collisions
	virtual void solveCollisions() = 0;

	// object hit by another object
	virtual void hit(Object* what) = 0;
};
