#include "Hero.h"

Hero::Hero( SimpleGame *Game )
{
	LivingImage = Game->Image_Load("Images\\Firion.png");
	DeadImage = Game->Image_Load("Images\\Firion - Dead.png");
	CurrentImage = LivingImage;

	Health = 100;
}

void Hero::Draw( SimpleGame *Game )
{
	Game->String_Draw( "Health: "+String_Convert(Health), 400, 280 );
	Game->Image_Draw( CurrentImage, 400, 300 );
}

void Hero::KillHero()
{
	CurrentImage = DeadImage;
}

void Hero::TakeDamage( int Damage )
{
	Health -= Damage;

	if( Health <= 0 )
	{
		KillHero();
	}
}	