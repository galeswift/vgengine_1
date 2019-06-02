#ifndef MONSTER_H
#define MONSTER_H

#include "SimpleGame.h"
#include "Hero.h"

class Monster
{
public:
	Monster( SimpleGame *Game );
	void DoDamage( Hero *TheHero );
	void Draw( SimpleGame *Game );
private:
	int Health;
	int MyDamage;
	SDL_Surface *Image;
};

#endif