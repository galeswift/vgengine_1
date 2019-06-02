#include "SimpleGame.h"
#include "Hero.h"
#include "Monster.h"
Hero *MyHero;
Monster *MyMonster;

void SimpleGame::KeyDown(const int& iKeyEnum)
{
		MyMonster->DoDamage( MyHero );
}

void SimpleGame::KeyUp(const int& iKeyEnum)
{
}

void SimpleGame::MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{
}

void SimpleGame::MouseButtonUp(const int& iButton,const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
}

void SimpleGame::MouseButtonDown(const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
	MyMonster->DoDamage( MyHero );
}


void SimpleGame::End()
{
	delete MyHero;
	delete MyMonster;
}

void SimpleGame::Initialized()
{
	MyHero = new Hero( this );
	MyMonster = new Monster( this );
}

void SimpleGame::Update( const int &iElapsedTime )
{
}

void SimpleGame::Render( SDL_Surface *DestinationSurface )
{	
	MyHero->Draw( this );
	MyMonster->Draw( this );
}
