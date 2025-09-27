#include "src/game.hpp"

int main()
{
	Game* game = new Game;
	while (!game->getIsDone()) 
	{
		game->handleInput();
		game->update();
		game->render();
		game->restartClock();
	}
	delete game;
	return 0;
}