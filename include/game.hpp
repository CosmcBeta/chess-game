#pragma once

#include "button.hpp"
#include "pieces/piece_info.hpp"
#include "theme.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>

constexpr int SQUARE_SIZE = 80;
constexpr float SCALE = 0.24024024f;

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
	void changeGamestate(State newState);
	void endTurn(Position mousePosition);

	// Functions relating to the king
	bool isInCheck(Position kingPosition, Team kingTeam);
	Position getKing(Team kingTeam, bool currentBoard);
	int numberOfPieces();

	// Functions regarding to the next move
	bool willBeInCheck(Position oldPosition, Position newPosition, Team team);
	void createPotentialBoard(Position oldPosition, Position newPosition, Team pieceTeam);

	// Functions for piece moves
	void displayMoves();
	void removeInvalidMoves(Team kingTeam, Position oldPosition);
	void removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves);
	int getTotalMoveCount(Team p_team);

	void menuState(sf::Vector2i mousePosition, bool leftButtonClicked);
	void settingsState(sf::Vector2i mousePosition, bool leftButtonClicked);
	void gameOverState(sf::Vector2i mousePosition, bool leftButtonClicked);

	bool playingGameState(sf::Vector2i actualMousePosition, std::optional<sf::Event> event, bool leftButtonClicked);

	void updateTheme();
	void setAudio(bool audioOn);
	void pauseState(sf::Vector2i mousePosition, bool leftButtonClicked, std::optional<sf::Event> event);

	void loadFont(sf::Font& font, const std::string& path);
	void loadAudio(sf::SoundBuffer& buffer, const std::string& path);
	void loadTexture(sf::Texture& texture, const std::string& path);

	void loadResources();


	bool getIsDone();

private:
	// Bools and consts
	const float circleRadius = 20.f;
	bool whiteKingInCheck_, blackKingInCheck_;
	bool pieceSelected_, pieceMoved_;
	bool playAgain_, lockClick_, buttonPressed_;

	// Window and time
	sf::Clock clock_;
	sf::Time elapsed_;

	// Vectors and arrays
	std::vector<sf::CircleShape> moveCircles_;
	std::vector<Move> possibleMoves_;
	Board board_;
	Board potentialBoard_;
	sf::RectangleShape backgroundArray_[64];

	// King positions and states
	Position blackKingPosition_, whiteKingPosition_;
	Team playerTurn_;
	State gameState_;
	GameOutcome gameOutcome_;

	// Textures and shapes
	sf::Texture whitePawnTexture_, whiteRookTexture_, whiteBishopTexture_, whiteKnightTexture_, whiteQueenTexture_, whiteKingTexture_;
	sf::Texture blackPawnTexture_, blackRookTexture_, blackBishopTexture_, blackKnightTexture_, blackQueenTexture_, blackKingTexture_;
	sf::RectangleShape background_, gameOverBackground_;

	// Texts, buttons, and fonts
	sf::Font myriadBoldFont_, myriadRegularFont_, myriadSemiboldFont_;
	sf::Text titleText_, settingsTitleText_, gameOverTitleText_, winnerText_;
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

	sf::RenderWindow window_;
	bool isDone_;
};
