#include <iostream>
#include "CGame.h"

int main()
{
	CGame game;
	if (!game.init(1200, 600))
		return -1;
	game.run();
	return 0;
}