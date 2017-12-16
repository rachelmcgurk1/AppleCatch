/*
=================
cApple.h
- Header file for class definition - SPECIFICATION
- Header file for the Apple class which is a child of cSprite class
=================
*/
#ifndef _CAPPLE_H
#define _CAPPLE_H
#include "cSprite.h"

class cApple : public cSprite
{
private:
	SDL_Point appleVelocity;


public:
	cApple();
	void update(double deltaTime);		// Basket update method
	void setAppleVelocity(SDL_Point appleVel);   // Sets the velocity for the apple
	SDL_Point getAppleVelocity();	// Gets the apple velocity
};
#endif