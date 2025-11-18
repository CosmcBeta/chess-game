#include "game.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/sliding_piece.hpp"
#include "pieces/knight.hpp"
#include "pieces/pawn.hpp"
#include "pieces/king.hpp"

#include <iostream>

Game::Game()
	:pieceSelected_(false), m_previousMove{MoveType::None, {0,0}},
	blackKingInCheck_(false), whiteKingInCheck_(false), pieceMoved_(false),
	buttonPressed_(false), lockClick_(false), playAgain_(false),
	startButton("Play", FontType::Regular, 60, {320, 320}),
	settingsButton("Settings", FontType::Regular, 60, {320, 400}),
	exitButton("Exit", FontType::Regular, 60, {320, 480}),
	settingsBackButton("Main Menu", FontType::Regular, 40, {320, 600}),
	playAgainButton("Play Again", FontType::Regular, 35, {200, 400}),
	mainMenuButton("Main Menu", FontType::Regular, 35, {420, 400}),
	settingsColorChoiceBrown("Brown", FontType::Regular, 35, {280, 290}),
	settingsColorChoiceBlue("Blue", FontType::Regular, 35, {500, 290}),
	settingsColorChoiceGreen("Green", FontType::Regular, 35, {400, 290}),
	settingsAudioChoiceYes("Yes", FontType::Regular, 35, {280, 420}),
	settingsAudioChoiceNo("No", FontType::Regular, 35, {420, 420}),
	pauseDrawButton("Draw Game", FontType::Regular, 35, {210, 360}),
	pauseWhiteForfeitButton("White Forfeit", FontType::Regular, 35, {440, 290}),
	pauseBlackForfeitButton("Black Forfeit", FontType::Regular, 35, {210, 290}),
	pauseQuitButton("Quit Game", FontType::Regular, 35, {440, 420}),
	pauseMenuButton("Main Menu", FontType::Regular, 35, {440, 360}),
	returnToGame("Resume", FontType::Regular, 35, {210, 420}),
	titleText_(myriadBoldFont_), settingsTitleText_(myriadBoldFont_), gameOverTitleText_(myriadBoldFont_), winnerText_(myriadSemiboldFont_),
	settingsAudioText(myriadRegularFont_), settingsColorText(myriadRegularFont_), pauseTitle(myriadBoldFont_),
	pieceMoveSound(pieceMoveBuffer), captureSound(captureBuffer), buttonClickSound(buttonClickBuffer),
	gameStartSound(gameStartBuffer), gameEndSound(gameEndBuffer), castleSound(castleBuffer),
	isDone_(false)
{
	restartClock();
	srand(static_cast<unsigned int>(time(NULL)));

	window_.create(sf::VideoMode({640, 640}), "Chess", sf::Style::Default);
	window_.setFramerateLimit(60);


	loadResources();
	createTexts();
	updateTheme();

	changeGamestate(State::Menu);
}

Game::~Game()
{
	window_.close();
}

bool Game::getIsDone()
{
	return isDone_;
}

sf::Time Game::getElapsed() { return elapsed_; }
void Game::restartClock() { elapsed_ = clock_.restart(); }

// Changes gamestates
void Game::changeGamestate(State newState)
{
	gameState_ = newState;

	if (gameState_ == State::CreateGame)
	{
		createPieces();
		playerTurn_ = Team::White;
		gameStartSound.play();
		changeGamestate(State::PlayingGame);
	}
	else if (gameState_ == State::GameOver)
	{
		gameEndSound.play();
		switch (gameOutcome_)
		{
		case GameOutcome::WhiteWins:
			winnerText_.setString("White Wins");
			break;
		case GameOutcome::BlackWins:
			winnerText_.setString("Black Wins");
			break;
		case GameOutcome::Stalemate:
			winnerText_.setString("Stalemate");
			break;
		default:
			break;
		}
	}
}

// Handles input from user
void Game::handleInput()
{
	std::optional event = window_.pollEvent();
	sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
	bool leftButtonClicked = false;
	if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>())
	{
    	leftButtonClicked = (mouse->button == sf::Mouse::Button::Left && !lockClick_);
	}

	switch (gameState_)
	{
		case State::Menu:
			menuState(mousePos, leftButtonClicked);
			break;
		case State::Settings:
			settingsState(mousePos, leftButtonClicked);
			break;
		case State::PlayingGame:
			if (!playingGameState(mousePos, event, leftButtonClicked)) return;
			break;
		case State::GameOver:
			gameOverState(mousePos, leftButtonClicked);
			break;
		case State::Pause:
			pauseState(mousePos, leftButtonClicked, event);
			break;
		default:
			break;
	}

	if (auto mouse = event->getIf<sf::Event::MouseButtonReleased>())
	{
		if (mouse->button == sf::Mouse::Button::Left)
		{
			lockClick_ = false;
			buttonPressed_ = false;
		}
	}
}

void Game::menuState(sf::Vector2i mousePosition, bool leftButtonClicked)
{
	startButton.update(mousePosition);
	settingsButton.update(mousePosition);
	exitButton.update(mousePosition);

	if (leftButtonClicked)
	{
		if (startButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::CreateGame);
			buttonPressed_ = true;
		}
		if (settingsButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::Settings);
			buttonPressed_ = true;
		}
		if (exitButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			isDone_ = true;
			buttonPressed_ = true;
		}
	}
}

void Game::settingsState(sf::Vector2i mousePosition, bool leftButtonClicked)
{
	settingsBackButton.update(mousePosition);
	settingsAudioChoiceNo.update(mousePosition);
	settingsAudioChoiceYes.update(mousePosition);
	settingsColorChoiceBrown.update(mousePosition);
	settingsColorChoiceBlue.update(mousePosition);
	settingsColorChoiceGreen.update(mousePosition);

	if (leftButtonClicked)
	{
		if (settingsBackButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (settingsAudioChoiceNo.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			setAudio(false);
			buttonPressed_ = true;
		}
		if (settingsAudioChoiceYes.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			setAudio(true);
			buttonPressed_ = true;
		}
		if (settingsColorChoiceBrown.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Brown);
			updateTheme();
			buttonPressed_ = true;
		}
		if (settingsColorChoiceBlue.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Blue);
			updateTheme();
			buttonPressed_ = true;
		}
		if (settingsColorChoiceGreen.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Green);
			updateTheme();
			buttonPressed_ = true;
		}
	}
}

void Game::updateTheme()
{
	// Board & background
	createBackground();
	background_.setFillColor(theme.alternate);
	gameOverBackground_.setFillColor(theme.alternate);

	// Texts
	titleText_.setFillColor(theme.darkMain);
	settingsTitleText_.setFillColor(theme.darkMain);
	gameOverTitleText_.setFillColor(theme.darkMain);
	winnerText_.setFillColor(theme.darkMain);
	settingsColorText.setFillColor(theme.darkMain);
	settingsAudioText.setFillColor(theme.darkMain);
	pauseBackground.setFillColor(theme.alternate);
	pauseTitle.setFillColor(theme.darkMain);

	// Buttons
	startButton.setColor(theme.darkMain, theme.lightMain);
	settingsButton.setColor(theme.darkMain, theme.lightMain);
	exitButton.setColor(theme.darkMain, theme.lightMain);
	settingsBackButton.setColor(theme.darkMain, theme.lightMain);
	playAgainButton.setColor(theme.darkMain, theme.lightMain);
	mainMenuButton.setColor(theme.darkMain, theme.lightMain);
	settingsColorChoiceBrown.setColor(theme.darkMain, theme.lightMain);
	settingsColorChoiceBlue.setColor(theme.darkMain, theme.lightMain);
	settingsColorChoiceGreen.setColor(theme.darkMain, theme.lightMain);
	settingsAudioChoiceYes.setColor(theme.darkMain, theme.lightMain);
	settingsAudioChoiceNo.setColor(theme.darkMain, theme.lightMain);
	pauseDrawButton.setColor(theme.darkMain, theme.lightMain);
	pauseWhiteForfeitButton.setColor(theme.darkMain, theme.lightMain);
	pauseBlackForfeitButton.setColor(theme.darkMain, theme.lightMain);
	pauseMenuButton.setColor(theme.darkMain, theme.lightMain);
	pauseQuitButton.setColor(theme.darkMain, theme.lightMain);
	returnToGame.setColor(theme.darkMain, theme.lightMain);
}

void Game::setAudio(bool audioOn)
{
	if (audioOn)
	{
		sf::Listener::setGlobalVolume(100);
	}
	else
	{
		sf::Listener::setGlobalVolume(0);
	}
}

// pauseDrawButton, pauseWhiteForfeitButton, pauseBlackForfeitButton, pauseMenuButton, pauseQuitButton
void Game::pauseState(sf::Vector2i mousePosition, bool leftButtonClicked, std::optional<sf::Event> event)
{
	pauseDrawButton.update(mousePosition);
	pauseWhiteForfeitButton.update(mousePosition);
	pauseBlackForfeitButton.update(mousePosition);
	pauseMenuButton.update(mousePosition);
	pauseQuitButton.update(mousePosition);
	returnToGame.update(mousePosition);

	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::PlayingGame);
		}
	}

	if (leftButtonClicked)
	{
		if (pauseDrawButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			gameOutcome_ = GameOutcome::Stalemate;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseWhiteForfeitButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			gameOutcome_ = GameOutcome::BlackWins;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseBlackForfeitButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			gameOutcome_ = GameOutcome::WhiteWins;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseMenuButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (pauseQuitButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			isDone_ = true;
			buttonPressed_ = true;
		}
		if (returnToGame.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::PlayingGame);
			buttonPressed_ = true;
		}
	}
}


bool Game::playingGameState(sf::Vector2i actualMousePosition, std::optional<sf::Event> event, bool leftButtonClicked)
{
	static Position selectedPiecePos {0, 0}; // Array scale
	Position mousePosArray = {actualMousePosition.x / SQUARE_SIZE, actualMousePosition.y / SQUARE_SIZE}; // 8 by 8
	sf::Vector2i mousePos = sf::Vector2i(mousePosArray.file * SQUARE_SIZE, mousePosArray.rank * SQUARE_SIZE); // 640 by 640, where to put the piece basically

	// Options
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::Pause);
		}
	}

	// Piece is not selected
	if (!pieceSelected_ && leftButtonClicked)
	{
		// Clicks a spot where your teams piece isn't
		if (board_[mousePosArray.file][mousePosArray.rank] == nullptr || board_[mousePosArray.file][mousePosArray.rank]->getTeam() != playerTurn_)
		{
			return false;
		}

		// Gets moves for piece
		board_[mousePosArray.file][mousePosArray.rank]->calculateMoves(board_, m_previousMove);
		possibleMoves_ = board_[mousePosArray.file][mousePosArray.rank]->getMoves();

		// Remove moves if piece's king is in check
		removeInvalidMoves(board_[mousePosArray.file][mousePosArray.rank]->getTeam(), mousePosArray);

		displayMoves();
		pieceSelected_ = true;
		selectedPiecePos = mousePosArray;
		lockClick_ = true;
	}


	if (pieceSelected_ && leftButtonClicked)
	{
		if (board_[mousePosArray.file][mousePosArray.rank] != nullptr && // Checks if the new tile selected is the same team as the piece that is trying to move
		board_[mousePosArray.file][mousePosArray.rank]->getTeam() == board_[selectedPiecePos.file][selectedPiecePos.rank]->getTeam())
		{
			// Clear moves
			moveCircles_.clear();
			possibleMoves_.clear();

			// Gets moves for piece
			board_[mousePosArray.file][mousePosArray.rank]->calculateMoves(board_, m_previousMove);
			possibleMoves_ = board_[mousePosArray.file][mousePosArray.rank]->getMoves();

			// Remove moves if piece's king is in check
			removeInvalidMoves(board_[mousePosArray.file][mousePosArray.rank]->getTeam(), mousePosArray);

			displayMoves();
			pieceSelected_ = true;
			lockClick_ = true;
			selectedPiecePos = mousePosArray;
			return false;
		}

		for (Move move : possibleMoves_)
		{
			if (mousePosArray == move.position) // Checks if move is a possible move
			{
				if (willBeInCheck(selectedPiecePos, mousePosArray, board_[selectedPiecePos.file][selectedPiecePos.rank]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
				{
					continue;
				}

				// Sets the current move type to capture if the place the piece is moving to has a piece already there
				if (board_[mousePosArray.file][mousePosArray.rank] != nullptr)
				{
					move.moveType = MoveType::Capture;
				}

				// adds moves to the history
				m_previousMove = move;

				// Plays sounds for the moves
				if (move.moveType == MoveType::Castle)
				{
					castleSound.play();
				}
				else if (move.moveType == MoveType::Capture || move.moveType == MoveType::EnPassant)
				{
					captureSound.play();
				}
				else
				{
					pieceMoveSound.play();
				}

				// Changes pawn into queen if it reaches the end
				if (board_[selectedPiecePos.file][selectedPiecePos.rank]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePos.file][selectedPiecePos.rank]->getTeam() == Team::White && mousePosArray.rank == 0)
				{
					board_[mousePosArray.file][mousePosArray.rank] = new SlidingPiece(Team::White, mousePosArray, PieceType::Queen);
				}
				else if (board_[selectedPiecePos.file][selectedPiecePos.rank]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePos.file][selectedPiecePos.rank]->getTeam() == Team::Black && mousePosArray.rank == 7)
				{
					board_[mousePosArray.file][mousePosArray.rank] = new SlidingPiece(Team::Black, mousePosArray, PieceType::Queen);
				}
				else
				{
					board_[mousePosArray.file][mousePosArray.rank] = board_[selectedPiecePos.file][selectedPiecePos.rank];
				}

				board_[selectedPiecePos.file][selectedPiecePos.rank] = nullptr;

				// Movement of rook for castling
				if (move.moveType == MoveType::Castle)
				{
					if (move.position == Position{2, 0})
					{
						board_[3][0] = board_[0][0];
						board_[0][0] = nullptr;
					}
					if (move.position == Position{6, 0})
					{
						board_[5][0] = board_[7][0];
						board_[7][0] = nullptr;
					}
					if (move.position == Position{2, 7})
					{
						board_[3][7] = board_[0][7];
						board_[0][7] = nullptr;
					}
					if (move.position == Position{6, 7})
					{
						board_[5][7] = board_[7][7];
						board_[7][7] = nullptr;
					}
				}

				// Removes the piece that en passant takes from the baord
				if (move.moveType == MoveType::EnPassant)
				{
					if (move.position.rank == 2)
					{
						board_[move.position.file][3] = nullptr;
					}
					if (move.position.rank == 5)
					{
						board_[move.position.file][4] = nullptr;
					}
				}

				endTurn(mousePosArray);

				if (blackKingInCheck_)
				{
					blackKingInCheck_ = false;
				}
				else if (whiteKingInCheck_)
				{
					whiteKingInCheck_ = false;
				}

				break;
			}
		}
		pieceMoved_ = true;
		lockClick_ = true;
	}
	return true;
}

void Game::gameOverState(sf::Vector2i mousePosition, bool leftButtonClicked)
{
	mainMenuButton.update(mousePosition);
	playAgainButton.update(mousePosition);

	if (leftButtonClicked)
	{
		if (mainMenuButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (playAgainButton.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound.play();
			changeGamestate(State::CreateGame);
			buttonPressed_ = true;
		}
	}
}

// Creates the fonts and texts
void Game::createTexts()
{
	sf::FloatRect textBounds;

	background_.setFillColor(theme.alternate);
	background_.setSize({800, 800});

	gameOverBackground_.setFillColor(theme.alternate);
	gameOverBackground_.setOutlineThickness(3);
	gameOverBackground_.setOutlineColor(sf::Color::Black);
	gameOverBackground_.setPosition({100, 200});
	gameOverBackground_.setSize({440, 240});

	pauseBackground.setFillColor(theme.alternate);
	pauseBackground.setOutlineThickness(3);
	pauseBackground.setOutlineColor(sf::Color::Black);
	pauseBackground.setPosition({70, 170});
	pauseBackground.setSize({500, 300});

	titleText_ = sf::Text(myriadBoldFont_, "Chess", 185);
	titleText_.setFillColor(theme.darkMain);
	textBounds = titleText_.getLocalBounds();
	titleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	titleText_.setPosition({320, 160});

	settingsTitleText_ = sf::Text(myriadBoldFont_, "Settings", 135);
	settingsTitleText_.setFillColor(theme.darkMain);
	textBounds = settingsTitleText_.getLocalBounds();
	settingsTitleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsTitleText_.setPosition({320, 120});

	gameOverTitleText_ = sf::Text(myriadBoldFont_, "Game Over", 80);
	gameOverTitleText_.setFillColor(theme.darkMain);
	textBounds = gameOverTitleText_.getLocalBounds();
	gameOverTitleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	gameOverTitleText_.setPosition({320, 235});

	winnerText_ = sf::Text(myriadSemiboldFont_, "Stalemate", 45);
	winnerText_.setFillColor(theme.darkMain);
	textBounds = winnerText_.getLocalBounds();
	winnerText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	winnerText_.setPosition({320, 290});

	settingsColorText = sf::Text(myriadRegularFont_, "Color:", 50);
	settingsColorText.setFillColor(theme.darkMain);
	textBounds = settingsColorText.getLocalBounds();
	settingsColorText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsColorText.setPosition({120, 290});

	settingsAudioText = sf::Text(myriadRegularFont_, "Audio:", 50);
	settingsAudioText.setFillColor(theme.darkMain);
	textBounds = settingsAudioText.getLocalBounds();
	settingsAudioText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsAudioText.setPosition({120, 420});

	pauseTitle = sf::Text(myriadBoldFont_, "Pause Menu", 80);
	pauseTitle.setFillColor(theme.darkMain);
	textBounds = pauseTitle.getLocalBounds();
	pauseTitle.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	pauseTitle.setPosition({320, 210});
}

// Removes any moves that puts king in check
void Game::removeInvalidMoves(Team kingTeam, Position oldPosition)
{
	auto iter = possibleMoves_.begin();
	int i = 0;
	while (iter != possibleMoves_.end())
	{
		Position temp {possibleMoves_.at(i).position.file, possibleMoves_.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iter = possibleMoves_.erase(iter);
		}
		else
		{
			++iter;
			++i;
		}
	}
}

// Removes any moves that put king in check given a specific set of possible moves
void Game::removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves)
{
	auto iter = moves.begin();
	int i = 0;
	while (iter != moves.end())
	{
		Position temp {moves.at(i).position.file, moves.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iter = moves.erase(iter);
		}
		else
		{
			++iter;
			++i;
		}
	}
}

// Displays the moves for one piece as circles on the screen
void Game::displayMoves()
{
	moveCircles_.clear();

	for (auto& i : possibleMoves_)
	{
		// Create circle for each point and adds them to an array
		sf::CircleShape tempCircle(circleRadius);
		tempCircle.setFillColor(theme.moveCircle);
		tempCircle.setPosition(sf::Vector2f(i.position.file + circleRadius, i.position.rank + circleRadius));
		moveCircles_.push_back(tempCircle);
	}
}

// Updates once every frame
void Game::update()
{
	while (const std::optional event = window_.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			isDone_ = true;
	}

	for (int file = 0; file < 8; file++) // Updates the positions of the pieces
	{
		for (int rank = 0; rank < 8; rank++)
		{
			if (board_[file][rank] == nullptr)
			{
				continue;
			}
			board_[file][rank]->setPosition({file, rank});
		}
	}

	whiteKingPosition_ = getKing(Team::White, true);
	blackKingPosition_ = getKing(Team::Black, true);

	if (isInCheck(whiteKingPosition_, Team::White))
	{
		whiteKingInCheck_ = true;
	}
	if (isInCheck(blackKingPosition_, Team::Black))
	{
		blackKingInCheck_ = true;
	}
	if (pieceMoved_)
	{
		if (getTotalMoveCount(Team::White) == 0)
		{
			if (isInCheck(whiteKingPosition_, Team::White))
			{
				gameOutcome_ = GameOutcome::BlackWins;
			}
			else
			{
				gameOutcome_ = GameOutcome::Stalemate;
			}
			playAgain_ = true;
			changeGamestate(State::GameOver);
		}
		if (getTotalMoveCount(Team::Black) == 0)
		{
			if (isInCheck(blackKingPosition_, Team::Black))
			{
				gameOutcome_ = GameOutcome::WhiteWins;
			}
			else
			{
				gameOutcome_ = GameOutcome::Stalemate;
			}
			playAgain_ = true;
			changeGamestate(State::GameOver);
		}
		if (numberOfPieces() == 2)
		{
			gameOutcome_ = GameOutcome::Stalemate;
			playAgain_ = true;
			changeGamestate(State::GameOver);
		}
		pieceMoved_ = false;
	}
}

// Returns the total number of pieces on the board
int Game::numberOfPieces()
{
	int count = 0;
	for (auto& row : board_)
	{
		for (auto& elem : row)
		{
			if (elem != nullptr)
			{
				count++;
			}
		}
	}
	return count;
}

// Gets the total number of moves given the team
int Game::getTotalMoveCount(Team team)
{
	std::vector<Position> allMoves;
	for (auto& row : board_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() != team)
			{
				continue;
			}

			piece->calculateMoves(board_, m_previousMove);
			std::vector<Move> pieceMoves = piece->getMoves();

			removeInvalidMoves(team, piece->getPosition(), pieceMoves);

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	return static_cast<int>(allMoves.size());
}

// Gets the position of the king on either the current or potential board based on the bool
Position Game::getKing(Team kingTeam, bool currentBoard)
{
	Board& board = currentBoard ? board_ : potentialBoard_;

	for (auto& row : board)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr)
			{
				continue;
			}

			if (piece->getPieceType() == PieceType::King && piece->getTeam() == kingTeam)
			{
				return piece->getPosition();
			}
		}
	}
	return {0, 0}; // should fail if king not found
}

// Checks if the piece at the given position is in check
bool Game::isInCheck(Position kingPosition, Team kingTeam)
{
	std::vector<Position> allMoves;
	for (auto& row : board_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() == kingTeam)
			{
				continue;
			}

			piece->calculateMoves(board_, m_previousMove);
			std::vector<Move> pieceMoves = piece->getMoves();

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	for (auto& move : allMoves)
	{
		if (kingPosition == move)
		{
			return true;
		}
	}

	return false;
}

// Checks if the piece at the given position is in check on potential board
bool Game::willBeInCheck(Position oldPosition, Position newPosition, Team team)
{
	createPotentialBoard(oldPosition, newPosition, team);

	std::vector<Position> allMoves;
	for (auto& row : potentialBoard_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() == team)
			{
				continue;
			}

			piece->calculateMoves(potentialBoard_, m_previousMove);
			std::vector<Move> pieceMoves = piece->getMoves();

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}

	Position kingPos = getKing(team, false);

	for (auto& move : allMoves)
	{
		if (kingPos == move)
		{
			return true;
		}
	}

	return false;
}

// Creates the potential board
void Game::createPotentialBoard(Position oldPosition, Position newPosition, Team pieceTeam)
{
	// Resets the board
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			potentialBoard_[file][rank] = board_[file][rank];
		}
	}

	// Moves the piece to where it would be
	potentialBoard_[newPosition.file][newPosition.rank] = potentialBoard_[oldPosition.file][oldPosition.rank];
	potentialBoard_[oldPosition.file][oldPosition.rank] = nullptr;
}

// Called at the end of a players turn
void Game::endTurn(Position mousePosition)
{
	playerTurn_ = (playerTurn_ == Team::White) ? Team::Black: Team::White;

	board_[mousePosition.file][mousePosition.rank]->setFirstMove(false);

	moveCircles_.clear();
	possibleMoves_.clear();

	pieceSelected_ = false;
}

// Displays objects to the screen
void Game::render()
{
	window_.clear(sf::Color::Black); // Clear

	// Playing game display
	switch (gameState_)
	{
		case State::PlayingGame:
			renderBoard();

			if (moveCircles_.size() == 0)
			{
				break;
			}

			for (sf::CircleShape i : moveCircles_)
			{
				window_.draw(i);
			}
			break;
		case State::Menu:
			window_.draw(background_);
			window_.draw(titleText_);
			window_.draw(startButton);
			window_.draw(settingsButton);
			window_.draw(exitButton);
			break;
		case State::Settings:
			window_.draw(background_);
			window_.draw(settingsBackButton);
			window_.draw(settingsTitleText_);
			window_.draw(settingsAudioText);
			window_.draw(settingsColorText);
			window_.draw(settingsAudioChoiceNo);
			window_.draw(settingsAudioChoiceYes);
			window_.draw(settingsColorChoiceBrown);
			window_.draw(settingsColorChoiceGreen);
			window_.draw(settingsColorChoiceBlue);
			break;
		case State::GameOver:
			renderBoard();
			window_.draw(gameOverBackground_);
			window_.draw(gameOverTitleText_);
			window_.draw(winnerText_);
			window_.draw(mainMenuButton);
			window_.draw(playAgainButton);
			break;
		case State::Pause:
			renderBoard();
			window_.draw(pauseBackground);
			window_.draw(pauseTitle);
			window_.draw(pauseDrawButton);
			window_.draw(pauseWhiteForfeitButton);
			window_.draw(pauseBlackForfeitButton);
			window_.draw(pauseMenuButton);
			window_.draw(pauseQuitButton);
			window_.draw(returnToGame);
		default:
			break;
	}

	window_.display(); // Display
}

void Game::renderBoard()
{
	for (sf::RectangleShape i : backgroundArray_)
	{
		window_.draw(i);
	}

	for (auto& rows : board_)
	{
		for (auto& piece : rows)
		{
			if (piece == nullptr)
			{
				continue;
			}

			sf::Texture texture {};
			switch (piece->getPieceType())
			{
    		    case PieceType::Pawn:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    				break;
    			case PieceType::King:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    			    break;
    			case PieceType::Queen:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    			    break;
    			case PieceType::Bishop:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    			    break;
    			case PieceType::Rook:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    				break;
    			case PieceType::Knight:
                    texture = piece->getTeam() == Team::White ? whitePawnTexture_ : blackPawnTexture_;
    			    break;
    			defualt:
                    texture = whitePawnTexture_;
    			    break;
			}
			sf::Sprite sprite(texture);
			Position pos = piece->getPosition();
			sprite.setPosition({pos.file * 80.f, pos.rank * 80.f});
			sprite.setScale({SCALE, SCALE});
			window_.draw(sprite);
		}
	}
}

// Creates the background tile array
void Game::createBackground()
{
	sf::RectangleShape lightRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	lightRect.setFillColor(theme.alternate);

	sf::RectangleShape darkRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	darkRect.setFillColor(theme.darkMain);

	int j = 0;
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (r % 2 == 0 && j % 2 == 0 || r % 2 == 1 && j % 2 == 1)
			{
				backgroundArray_[j] = lightRect;
			}
			else
			{
				backgroundArray_[j] = darkRect;
			}
			backgroundArray_[j++].setPosition(sf::Vector2f(r * static_cast<float>(SQUARE_SIZE), c * static_cast<float>(SQUARE_SIZE)));
		}
	}
}

// Creates all the pieces and adds them to 2d array board_
void Game::createPieces()
{
	// Add white pieces
	board_[4][7] = new King(Team::White, {4, 7});
	board_[3][7] = new SlidingPiece(Team::White, {3, 7}, PieceType::Queen);
	board_[2][7] = new SlidingPiece(Team::White, {2, 7}, PieceType::Bishop);
	board_[5][7] = new SlidingPiece(Team::White, {5, 7}, PieceType::Bishop);
	board_[1][7] = new Knight(Team::White, {1, 7});
	board_[6][7] = new Knight(Team::White, {6, 7});
	board_[0][7] = new SlidingPiece(Team::White, {0, 7}, PieceType::Rook);
	board_[7][7] = new SlidingPiece(Team::White, {7, 7}, PieceType::Rook);
	for (int i = 0; i < 8; i++)
	{
		board_[i][6] = new Pawn(Team::White, {i, 6});
	}

	// Add black pieces
	board_[4][0] = new King(Team::Black, {4, 0});
	board_[3][0] = new SlidingPiece(Team::Black, {3, 0}, PieceType::Queen);
	board_[2][0] = new SlidingPiece(Team::Black, {2, 0}, PieceType::Bishop);
	board_[5][0] = new SlidingPiece(Team::Black, {5, 0}, PieceType::Bishop);
	board_[1][0] = new Knight(Team::Black, {1, 0});
	board_[6][0] = new Knight(Team::Black, {6, 0});
	board_[0][0] = new SlidingPiece(Team::Black, {0, 0}, PieceType::Rook);
	board_[7][0] = new SlidingPiece(Team::Black, {7, 0}, PieceType::Rook);
	for (int i = 0; i < 8; i++)
	{
		board_[i][1] = new Pawn(Team::Black, {i, 1});
	}

	// Add black spaces
	for (int i = 2; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			board_[j][i] = nullptr;
		}
	}
}

// Loads the font given with the path given
void Game::loadFont(sf::Font& font, const std::string& path)
{
	if (!font.openFromFile(path))
	{
		std::cerr << "Failed to open font: " << path << "\n";
	}
}

// Loads the audio given with the path given
void Game::loadAudio(sf::SoundBuffer& buffer, const std::string& path)
{
	if (!buffer.loadFromFile(path))
	{
		std::cerr << "Failed to open audio: " << path << "\n";
	}
}

// Loads the texture given with the path given
void Game::loadTexture(sf::Texture& texture, const std::string& path)
{
	if (!texture.loadFromFile(path))
	{
		std::cerr << "Failed to load texture: " << path << "\n";
	}
}

void Game::loadResources()
{
	// Textures
	loadTexture(whitePawnTexture_, "../assets/images/white_pawn.png");
	loadTexture(whiteRookTexture_, "../assets/images/white_rook.png");
	loadTexture(whiteBishopTexture_, "../assets/images/white_bishop.png");
	loadTexture(whiteKnightTexture_, "../assets/images/white_knight.png");
	loadTexture(whiteQueenTexture_, "../assets/images/white_queen.png");
	loadTexture(whiteKingTexture_, "../assets/images/white_king.png");
	loadTexture(blackPawnTexture_, "../assets/images/black_pawn.png");
	loadTexture(blackRookTexture_, "../assets/images/black_rook.png");
	loadTexture(blackBishopTexture_, "../assets/images/black_bishop.png");
	loadTexture(blackKnightTexture_, "../assets/images/black_knight.png");
	loadTexture(blackQueenTexture_, "../assets/images/black_queen.png");
	loadTexture(blackKingTexture_, "../assets/images/black_king.png");

	// Fonts
	loadFont(myriadBoldFont_, "../assets/fonts/myriad_pro_bold.ttf");
	loadFont(myriadSemiboldFont_, "../assets/fonts/myriad_pro_semibold.ttf");
	loadFont(myriadRegularFont_, "../assets/fonts/myriad_pro_regular.ttf");

	// Sounds
	loadAudio(pieceMoveBuffer, "../assets/audio/move.mp3");
	loadAudio(captureBuffer, "../assets/audio/capture.mp3");
	loadAudio(gameStartBuffer, "../assets/audio/game-start.mp3");
	loadAudio(gameEndBuffer, "../assets/audio/game-end.mp3");
	loadAudio(castleBuffer, "../assets/audio/castle.mp3");
	loadAudio(buttonClickBuffer, "../assets/audio/button-click.mp3");
}
