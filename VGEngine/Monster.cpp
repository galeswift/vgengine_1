#include "Monster.h"
Monster::Monster( SimpleGame *Game )
{
	Image = Game->Image_Load("Images\\Green Dragon.png");
	Health = 9000;
	MyDamage = 25;

}

void Monster::DoDamage( Hero *TheHero )
{
	TheHero->TakeDamage( MyDamage );
}

void Monster::Draw( SimpleGame *Game )
{
	Game->String_Draw( "Health: "+String_Convert(Health), 200, 280 );
	Game->Image_Draw( Image, 200, 300 );
}