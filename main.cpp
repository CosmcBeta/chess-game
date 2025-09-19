#include "src/Game.hpp"

int main() 
{
	Game* game = new Game;
	while (!game->getWindow()->isDone()) 
	{
		game->handleInput();
		game->update();
		game->render();
		game->restartClock();
	}
	delete game;
	return 0;
}