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

// Game state enum
enum class State
{
	MENU,
	WHITE_TURN,
	BLACK_TURN,
	GAME_OVER
};

// Game class
class Game {
public:
	// Constructor and deconstructor
	Game();
	~Game();
	
	// Input, Updates, Rendering - order called in main function
	void handleInput();
	void update();
	void render();

	// not used might remove
	Window* getWindow();

	// Clock and time
	sf::Time getElapsed();
	void restartClock();

	// Functions for gameplay
	void createBackground();
	void createPieces();
	void displayMoves();
	bool isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam);
	void removeInvalidMoves(Team p_kingTeam);
	sf::Vector2i getKing(Team p_kingTeam);
	void endTurn(sf::Vector2i p_mousePos);

private:
	// Bools and consts
	const float circleRadius = 20.f;
	bool wkInCheck, bkInCheck;
	bool pieceSelected;

	// Window and time
	Window m_window;
	sf::Clock m_clock;
	sf::Time m_elapsed;

	// Vectors and arrays
	std::vector<sf::CircleShape> circles;
	std::vector<sf::Sprite> sprites;
	std::vector<sf::Vector2f> possibleMoves;
	Piece* m_field[8][8];
	sf::RectangleShape backgroundArray[64];

	// King positions and game states
	sf::Vector2i bkPos, wkPos;
	Team playerTurn;
	State gameState;

	// Textures
	ResourceManager& m_resourceManager;
	sf::Texture whitePawnTex, whiteRookTex, whiteBishopTex, whiteKnightTex, whiteQueenTex, whiteKingTex,
		blackPawnTex, blackRookTex, blackBishopTex, blackKnightTex, blackQueenTex, blackKingTex;

	// Colors
	sf::Color redHighlight;
	sf::Color yellowHighlight;
	sf::Color lightBack;
	sf::Color darkBack;
	sf::Color grayCircle;
};