#include "SimpleGame.h"

int main(int argc, char* argv[])
{
	SimpleGame *Engine = new SimpleGame;
 
	Engine->SetTitle( "Loading..." );
	Engine->Init();
 
	Engine->SetTitle( "VG Engine Running!" );
	Engine->Start();
 
	Engine->SetTitle( "Quitting..." );
 
	delete Engine;

	return 0;
}
