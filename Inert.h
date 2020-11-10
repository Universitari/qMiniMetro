#pragma once

#include "Object.h"

class Inert : public Object {

public:

	Inert() { };
	
	virtual std::string name() = 0;
	virtual void animate() { };
	virtual void advance() { };
	virtual void solveCollisions() { };
	virtual void hit(Object* what) = 0;
};
