#ifndef HERO_H
#define HERO_H

#include "SimpleGame.h"

class Hero
{
public:
	Hero( SimpleGame *Game );
	void Draw( SimpleGame *Game );
	void TakeDamage( int Damage );
	void KillHero();

private:
	SDL_Surface *LivingImage;
	SDL_Surface *DeadImage;
	SDL_Surface *CurrentImage;
	int Health;
};

#endif