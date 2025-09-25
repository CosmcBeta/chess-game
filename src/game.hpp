#pragma once

#include <vector>
#include <SFML/Audio.hpp>

#include "window.hpp"
#include "king.hpp"
#include "queen.hpp"
#include "bishop.hpp"
#include "knight.hpp"
#include "rook.hpp"
#include "pawn.hpp"
#include "button.hpp"
#include "theme.hpp"


// Game state enum
enum class State
{
	Menu,
	Settings,
	CreateGame,
	PlayingGame,
	GameOver,
	Pause
};

// Win state enum
enum class GameOutcome
{
	WhiteWins,
	BlackWins,
	Stalemate
};

// Game class
class Game 
{
public:
	// Constructor and deconstructor
	Game();

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
	void changeGamestate(State p_newState);
	void endTurn(sf::Vector2i p_mousePos);

	// Functions relating to the king
	bool isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam);
	sf::Vector2i getKing(Team kingTeam, bool currentBoard);
	int numberOfPieces();

	// Functions regarding to the next move
	bool willBeInCheck(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_team);
	void createPotentialBoard(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_pieceTeam);

	// Functions for piece moves
	void displayMoves();
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos);
	void removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos, std::vector<Move>& p_moves);
	int getTotalMoveCount(Team p_team);

	// Returns the window
	Window* getWindow();

	void menuState(sf::Vector2i mousePos, bool leftButtonClicked);
	void settingsState(sf::Vector2i mousePos, bool leftButtonClicked);
	void gameOverState(sf::Vector2i mousePos, bool leftButtonClicked);

	bool playingGameState(sf::Vector2i actualMousePos, std::optional<sf::Event> event, bool leftButtonClicked);

	void updateTheme();
	void setAudio(bool p_audioOn);
	void pauseState(sf::Vector2i mousePos, bool leftButtonClicked, std::optional<sf::Event> event);

	void loadFont(sf::Font& font, const std::string& path);
	void loadAudio(sf::SoundBuffer& buffer, const std::string& path);
	void loadTexture(sf::Texture& texture, const std::string& path);

	void loadResources();

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
	std::vector<Move> possibleMoves;
	Board m_field;
	Board m_potentialField;
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
	Theme theme;

	Move m_previousMove;

	sf::Text settingsColorText, settingsAudioText;
	Button settingsColorChoiceBrown, settingsColorChoiceGreen, settingsAudioChoiceYes, settingsAudioChoiceNo;
	Button settingsColorChoiceBlue;

	sf::SoundBuffer pieceMoveBuffer, captureBuffer, gameStartBuffer, gameEndBuffer, castleBuffer, buttonClickBuffer;
	sf::Sound pieceMoveSound, captureSound, gameStartSound, gameEndSound, castleSound, buttonClickSound;

	sf::Text pauseTitle;
	Button pauseDrawButton, pauseWhiteForfeitButton, pauseBlackForfeitButton, pauseMenuButton, pauseQuitButton, returnToGame;
	sf::RectangleShape pauseBackground;
};