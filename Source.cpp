#include <iostream>
#include "CGame.h"

int main()
{
	std::ofstream archivo("datos.csv");
	CGame game;
	if (!game.init(1200, 600, archivo))
		return -1;
	game.run(archivo);
	return 0;
}