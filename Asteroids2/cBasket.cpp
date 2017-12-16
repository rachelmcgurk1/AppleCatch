/*
=================
cBasket.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cBasket.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cBasket::cBasket() : cSprite()
{
	this->basketVelocity = { 0, 0 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cBasket::update(double deltaTime)
{
	this->setBoundingRect(this->getSpritePos());
}
/*
=================================================================
Sets the velocity for the basket
=================================================================
*/
void cBasket::setBasketVelocity(SDL_Point basketVel)
{
	basketVelocity = basketVel;
}
/*
=================================================================
Gets the basket velocity
=================================================================
*/
SDL_Point cBasket::getBasketVelocity()
{
	return basketVelocity;
}
