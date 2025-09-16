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

// Game state enum
enum class State
{
	MENU,
	SETTINGS,
	CREATE_GAME,
	PLAYING_GAME,
	GAME_OVER
};

// Win state enum
enum class GameOutcome
{
	WHITE_WINS,
	BLACK_WINS,
	STALEMATE
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
	void renderBoard();

	// Clock and time
	sf::Time getElapsed();
	void restartClock();

	// Functions refering to states
	void createBackground();
	void createPieces();
	void createTexts();
	void loadFonts();
	void createTextures();
	void changeGamestate(State p_newState);
	void endTurn(sf::Vector2i p_mousePos);

	// Functions relating to the king
	bool isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam);
	sf::Vector2i getKing(Team p_kingTeam);
	int numberOfPieces();

	// Functions regarding to the next move
	bool willBeInCheck(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_team);
	void createPotentialBoard(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_pieceTeam);
	sf::Vector2i getPotentialKing(Team p_kingTeam);

	// Functions for piece moves
	void displayMoves();
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos);
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos, std::vector<sf::Vector2f>& p_moves);
	int getTotalMoveCount(Team p_team);

	// Returns the window
	Window* getWindow();

	void menuState(sf::Vector2i mousePos, bool leftButtonClicked);
	void settingsState(sf::Vector2i mousePos, bool leftButtonClicked);
	void gameOverState(sf::Vector2i mousePos, bool leftButtonClicked);

	bool playingGameState(sf::Vector2i actualMousePos, std::optional<sf::Event> event, bool leftButtonClicked);

private:
	// Bools and consts
	const float circleRadius = 20.f;
	bool whiteKingInCheck, blackKingInCheck;
	bool pieceSelected, pieceMoved;
	bool playAgain, lockClick, buttonPressed;

	// Window and time
	Window m_window;
	sf::Clock m_clock;
	sf::Time m_elapsed;

	// Vectors and arrays
	std::vector<sf::CircleShape> moveCircles;
	std::vector<sf::Sprite> sprites;
	std::vector<sf::Vector2f> possibleMoves;
	Piece* m_field[8][8];
	Piece* m_potentialField[8][8];
	sf::RectangleShape backgroundArray[64];

	// King positions and states
	sf::Vector2i blackKingPos, whiteKingPos;
	Team playerTurn;
	State gameState;
	GameOutcome gameOutcome;

	// Textures and shapes
	sf::Texture whitePawnTex, whiteRookTex, whiteBishopTex, whiteKnightTex, whiteQueenTex, whiteKingTex;
	sf::Texture blackPawnTex, blackRookTex, blackBishopTex, blackKnightTex, blackQueenTex, blackKingTex;
	sf::RectangleShape background, gameOverBackground;

	// Texts, buttons, and fonts
	sf::Font myriadBold, myriadRegular, myriadSemibold;
	sf::Text titleText, settingsTitleText, gameOverTitleText, winnerText;
	Button startButton, settingsButton, exitButton, settingsBackButton, playAgainButton, mainMenuButton;

	// Colors
	sf::Color redHighlight, yellowHighlight;
	sf::Color lightBrown, darkBrown, grayCircle;
	sf::Color textHighlight;
};