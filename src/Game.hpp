#pragma once

#include <vector>

#include "Window.hpp"
#include "King.hpp"
#include "Queen.hpp"
#include "Bishop.hpp"
#include "Knight.hpp"
#include "Rook.hpp"
#include "Pawn.hpp"
#include "Button.hpp"
#include "ResourceManager.hpp"

// Game state enum
enum class State
{
	MENU,
	SETTINGS,
	CREATE_GAME,
	PLAYING_GAME,
	GAME_OVER
};

enum class GameOverState
{
	WHITE_WINS,
	BLACK_WINS,
	DRAW
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

	// Start of state functions
	void createMenu();
	void cleanScreen();
	void createBackground();
	void createPieces();

	// Functions relating to the king
	bool isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam);
	sf::Vector2i getKing(Team p_kingTeam);

	// Functions regarding to the next move
	bool willBeInCheck(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_team);
	void createPotentialBoard(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_pieceTeam);
	sf::Vector2i getPotentialKing(Team p_kingTeam);

	// Functions for displaying moves
	void displayMoves();
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos);
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos, std::vector<sf::Vector2f>& p_moves);

	// Other functions
	int getTotalMoveCount(Team p_team);
	void endTurn(sf::Vector2i p_mousePos);
	void changeGamestate(State p_newState);
	void startGame();
	void openSettings();
	void createGameOverScreen();
	void createTexts();
	void gameOver(GameOverState winner);

private:
	// Bools and consts
	const float circleRadius = 20.f;
	bool wkInCheck, bkInCheck;
	bool pieceSelected, pieceMoved;

	// Window and time
	Window m_window;
	sf::Clock m_clock;
	sf::Time m_elapsed;

	// Vectors and arrays
	std::vector<sf::CircleShape> circles;
	std::vector<sf::Sprite> sprites;
	std::vector<sf::Vector2f> possibleMoves;
	Piece* m_field[8][8];
	Piece* m_potentialField[8][8];
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
	sf::Color lightBrown;
	sf::Color darkBrown;
	sf::Color grayCircle;
	sf::Color textHighlight;

	sf::RectangleShape background, gameOverBackground;
	sf::Font myriadBold, myriadRegular, myriadSemibold;
	sf::Text titleText, settingsTitleText, gameOverTitleText, winnerText;
	Button startButton, settingsButton, exitButton, settingsBackButton, playAgainButton, mainMenuButton;
};