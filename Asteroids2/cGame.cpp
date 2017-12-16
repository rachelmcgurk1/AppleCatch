/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();

	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	score = 0;
	appleCount = 0;
	appleTimer = 0;

	theAreaClicked = { 0 , 0 };

	// Store the textures
	textureName = { "apple1", "apple2", "theBasket", "theBackground", "menuBackground" };
	texturesToUse = { "Images/Sprites/apple.png", "Images/Sprites/apple2.png", "Images/Sprites/basket.png", "Images/Sprites/sky.jpg", "Images/Sprites/sky2.png" };
	
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	
	// Store the button textures
	btnNameList = { "exit_btn", "instructions_btn", "menu_btn", "play_btn", "retry_btn" };
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_retry.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 } };
	
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	
	//Sets the game state
	theGameState = MENU;
	theBtnType = EXIT;

	// Create textures for Game Dialogue (text)
	fontList = { "8bit" };
	fontsToUse = { "Fonts/8bit.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Apple Catch" , "Score: ", "How to Play:", "Game Over!" };
	
	theTextureMgr->addTexture("Title", theFontMgr->getFont("8bit")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Instructions", theFontMgr->getFont("8bit")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Lose", theFontMgr->getFont("8bit")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "coin", "click" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/theme.wav", "Audio/coin.wav", "Audio/click.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	// Sets the background and basket for main level
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theBasket.setSpritePos({ 400, 650 });
	theBasket.setTexture(theTextureMgr->getTexture("theBasket"));
	theBasket.setSpriteDimensions(theTextureMgr->getTexture("theBasket")->getTWidth(), theTextureMgr->getTexture("theBasket")->getTHeight());
	theBasket.setBasketVelocity({ 0, 0 });

	//Stores the score
	string stringScore = "Score : " + to_string(score);
	LPCSTR scoreToString = stringScore.c_str();
	theTextureMgr->addTexture("Score", theFontMgr->getFont("8bit")->createTextTexture(theRenderer, scoreToString, SOLID, { 0,0,0,225 }, { 0,0,0,225 }));
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		// Gets the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case MENU:
	{
		// Renders the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());


		// Renders the title
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 325, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		

		// Renders the button
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 470, 300 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 470, 500 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		theButtonMgr->getBtn("instructions_btn")->setSpritePos({ 420, 375 });
		theButtonMgr->getBtn("instructions_btn")->render(theRenderer, &theButtonMgr->getBtn("instructions_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("instructions_btn")->getSpritePos(), theButtonMgr->getBtn("instructions_btn")->getSpriteScale());
		
		SDL_RenderPresent(theRenderer);

	}
	break;

	case INSTRUCTIONS:
	{
		//Renders the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("menuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("menuBackground")->getTWidth(), theTextureMgr->getTexture("menuBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Renders the instructions title
		tempTextTexture = theTextureMgr->getTexture("Instructions");
		pos = { 325, 110, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);


		// Renders the button
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 470, 600 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());

		SDL_RenderPresent(theRenderer);

	}
	break;

	case PLAYING:
	{
		//Renders the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		
		// Renders each apple in the vector array
		for (int draw = 0; draw < theApples.size(); draw++)
		{
			theApples[draw]->render(theRenderer, &theApples[draw]->getSpriteDimensions(), &theApples[draw]->getSpritePos(), theApples[draw]->getSpriteRotAngle(), &theApples[draw]->getSpriteCentre(), theApples[draw]->getSpriteScale());
		}
		
		// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		//Renders the score
		string stringScore = "Score : " + to_string(score);
		LPCSTR scoreToString = stringScore.c_str();
		theTextureMgr->deleteTexture("Score");
		theTextureMgr->addTexture("Score", theFontMgr->getFont("8bit")->createTextTexture(theRenderer, scoreToString, SOLID, { 0,0,0,225 }, { 0,0,0,225 }));

		cTexture* tempTextTexture2 = theTextureMgr->getTexture("Score");
		SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		FPoint scale2 = { 1, 1 };
		tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);
		
		// Renders the basket
		theBasket.render(theRenderer, &theBasket.getSpriteDimensions(), &theBasket.getSpritePos(), theBasket.getSpriteRotAngle(), &theBasket.getSpriteCentre(), theBasket.getSpriteScale());
		
		SDL_RenderPresent(theRenderer);
	}
	break;

	case END:
	{
		//Renders the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Renders the end screen text
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 325, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("Lose");
		pos = { 350, 300, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Renders the buttons
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 400, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("retry_btn")->setSpritePos({ 560, 500 });
		theButtonMgr->getBtn("retry_btn")->render(theRenderer, &theButtonMgr->getBtn("retry_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("retry_btn")->getSpritePos(), theButtonMgr->getBtn("retry_btn")->getSpriteScale());

		//Renders the player's final score
		cTexture* tempTextTexture2 = theTextureMgr->getTexture("Score");
		SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		FPoint scale2 = { 1, 1 };
		tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);

		SDL_RenderPresent(theRenderer);
	}
	break;

	case QUIT:
	{
		//Ends the game loops and quits the game
		loop = false;
	}
	break;
		default:
			break;
	}

}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{

	switch (theGameState)
	{
	case MENU:
	{
		// Checks if a button is clicked and changes the game state
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
		theGameState = theButtonMgr->getBtn("instructions_btn")->update(theGameState, INSTRUCTIONS, theAreaClicked);
	}
	break;

	case INSTRUCTIONS:
	{
		// Checks if a button is clicked and changes the game state
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
	}
	break;

	case PLAYING:
	{
		// Updates the basket position
		theBasket.update(deltaTime);

		//Spawns the apples
		spawnApples();
		appleTimer -= deltaTime;

		//Resets the score if the player restarts
		if (restart == true)
		{
			theScore = 0;
			restart = false;
			scoreChanged = true;
		}

		// Updates the visibility and position of each apple
		vector<cApple*>::iterator appleIterator = theApples.begin();
		while (appleIterator != theApples.end())
		{
			if ((*appleIterator)->isActive() == false)
			{
				appleIterator = theApples.erase(appleIterator);
			}
			else
			{
				//Changes the game state to game over if an apple falls below the screen
				if ((*appleIterator)->getSpritePos().y > 900)
				{
					theGameState = END;
				}
				
				(*appleIterator)->update(deltaTime);
				++appleIterator;
		
			}

			//Updates the velocity of the apple
			for (int apple = 0; apple < theApples.size(); apple++)
			{
				theApples[apple]->setAppleVelocity({ 0, 30 });
			}
		}

		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/

		for (appleIterator = theApples.begin(); appleIterator != theApples.end(); ++appleIterator)
		{
			if ((*appleIterator)->collidedWith(&(*appleIterator)->getBoundingRect(), &theBasket.getBoundingRect()))
			{
				//Deletes apple if collison occurs
				(*appleIterator)->setActive(false);

				//Adds to score
				score += 1;

				//Plays sound
				theSoundMgr->getSnd("coin")->play(0);

				applesCaught++;


			}
		}
	}	
	break;
	
	case END:
	{

		// Checks if a button is clicked and changes the game state		
		theGameState = theButtonMgr->getBtn("retry_btn")->update(theGameState, RETRY, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, RESTART, theAreaClicked);

	}
	break;

	case RESTART:
	{
		//Resets the game is a player restarts
		theBasket.setSpritePos({ 400, 650 });
		theApples.clear();
		appleCount = 0;
		applesCaught = 0;
		score = 0;
		restart = true;
		theGameState = MENU;
	}
	break;

	case RETRY:
	{
		theBasket.setSpritePos({ 400, 650 });
		theApples.clear();
		appleCount = 0;
		applesCaught = 0;
		score = 0;
		restart = true;
		theGameState = PLAYING;
	}
	break;

	default:
		break;
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				//Allows player to click on an area with left mouse button, plays sound
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
					theSoundMgr->getSnd("click")->play(0);
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				//Exits the game
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
				}
				break;

				case SDLK_UP:
				{
				}
				break;

				//Moves the basket right with the right arrow key
				case SDLK_RIGHT:
				{
					if (theBasket.getSpritePos().x < (renderWidth - theBasket.getSpritePos().w))
					{
						theBasket.setSpritePos({ theBasket.getSpritePos().x + 10 , theBasket.getSpritePos().y });
					}
				}
				break;

				//Moves the basket left with the left arrow key
				case SDLK_LEFT:
				{
					if (theBasket.getSpritePos().x > 0)
					{
						theBasket.setSpritePos({ theBasket.getSpritePos().x - 10, theBasket.getSpritePos().y });
					}
				}
				break;

				case SDLK_SPACE:
				{
					
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

void cGame::spawnApples()
{
	if (appleTimer <= 0)
	{
		// Creates a vector array for the apples
		theApples.push_back(new cApple);
		int apple = theApples.size() - 1;
		theApples[apple]->setSpritePos({ 100 * (rand() % 10), -10 * (rand() % 10) });
		theApples[apple]->setSpriteTranslation({ 0, 2/*(rand() % 8 + 1)*/ });
		int randApple = rand() % 2;
		theApples[apple]->setTexture(theTextureMgr->getTexture(textureName[randApple]));
		theApples[apple]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randApple])->getTWidth(), theTextureMgr->getTexture(textureName[randApple])->getTHeight());
		theApples[apple]->setAppleVelocity({ 3, 3 });
		theApples[apple]->setActive(true);
		appleTimer = 1.5f;
	}
}
double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

