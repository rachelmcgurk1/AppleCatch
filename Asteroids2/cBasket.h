/*
=================
cBasket.h
- Header file for class definition - SPECIFICATION
- Header file for the Basket class which is a child of cSprite class
=================
*/
#ifndef _CBASKET_H
#define _CBASKET_H
#include "cSprite.h"

class cBasket : public cSprite
{
private:
	SDL_Point basketVelocity;

public:
	cBasket();
	void update(double deltaTime);		// Basket update method
	void setBasketVelocity(SDL_Point basketVel);   // Sets the velocity for the basket
	SDL_Point getBasketVelocity();				 // Gets the basket velocity
};
#endif