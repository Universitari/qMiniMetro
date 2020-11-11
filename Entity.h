#pragma once

#include "Object.h"

class Entity : public Object {

public:

	Entity() { };

	virtual std::string name() = 0;
	virtual void animate() = 0;
	virtual void advance() = 0;
	virtual void solveCollisions() = 0;
	virtual void hit(Object* what) = 0;
};
