#pragma once

#include <vector>

#include "Window.hpp"
#include "King.hpp"
#include "Queen.hpp"
#include "Bishop.hpp"
#include "Knight.hpp"
#include "Rook.hpp"
#include "Pawn.hpp"
#include "ResourceManager.hpp"

enum class State
{
	MENU,
	WHITE_TURN,
	BLACK_TURN,
	GAME_OVER
};

class Game {
public:
	Game();
	~Game();
	
	void handleInput();
	void update();
	void render();

	Window* getWindow();

	sf::Time getElapsed();
	void restartClock();

	void createBackground();
	void createPieces();
	void displayMoves();
	bool isInCheck(sf::Vector2i p_kingPos);
	void switchTeam();
	void endTurn(sf::Vector2i p_mousePos);

private:

	const float circleRadius = 20.f;
	bool wkInCheck, bkInCheck;

	Window m_window;
	sf::Clock m_clock;
	sf::Time m_elapsed;

	sf::RectangleShape backgroundArray[64];
	std::vector<sf::CircleShape> circles;
	std::vector<sf::Sprite> sprites;
	std::vector<sf::Vector2f> possibleMoves;

	ResourceManager& m_resourceManager;
	Team playerTurn;
	State gameState;

	sf::Texture whitePawnTex, whiteRookTex, whiteBishopTex, whiteKnightTex, whiteQueenTex, whiteKingTex,
		blackPawnTex, blackRookTex, blackBishopTex, blackKnightTex, blackQueenTex, blackKingTex;

	Piece* m_field[8][8];
	bool pieceSelected;

	sf::Color redHighlight;
	sf::Color yellowHighlight;
	sf::Color lightBack;
	sf::Color darkBack;
	sf::Color grayCircle;
};