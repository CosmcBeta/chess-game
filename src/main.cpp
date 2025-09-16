#include "Game.hpp"

int main() 
{
	Game* game = new Game;
	while (!game->getWindow()->isDone()) 
	{
		game->handleInput();
		game->update();
		game->render();
		game->restartClock();
		//std::cout << game->getWindow()->isDone() << "\n";
	}
	delete game;
	return 0;
}

// g++ -std=c++17 src/*.cpp $(pkg-config --libs --cflags sfml-window sfml-system sfml-graphics) -o main; ./main