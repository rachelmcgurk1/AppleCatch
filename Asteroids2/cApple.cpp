/*
=================
cApple.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cApple.h"


/*
=================================================================
Defualt Constructor
=================================================================
*/
cApple::cApple() : cSprite()
{
	this->appleVelocity = { 0, 0 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cApple::update(double deltaTime)
{

	this->setSpriteRotAngle(this->getSpriteRotAngle() +(10.0f * deltaTime)); 
	if (this->getSpriteRotAngle() > 360)
	{
		this->setSpriteRotAngle(this->getSpriteRotAngle() -360);
	}

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->getSpriteTranslation().x;// *deltaTime *appleVelocity.x;
	currentSpritePos.y += this->getSpriteTranslation().y;// *deltaTime *appleVelocity.y;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	cout << "Apple position - x: " << this->getSpritePos().x << " y: " << this->getSpritePos().y << " deltaTime: " << deltaTime << endl;
	this->setBoundingRect(this->getSpritePos());


}
/*
=================================================================
  Sets the velocity for the Apple
=================================================================
*/
void cApple::setAppleVelocity(SDL_Point AppleVel)
{
	appleVelocity = AppleVel;
}
/*
=================================================================
  Gets the Apple velocity
=================================================================
*/
SDL_Point cApple::getAppleVelocity()
{
	return appleVelocity;
}
