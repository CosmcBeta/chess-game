#include "game.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/sliding_piece.hpp"
#include "pieces/knight.hpp"
#include "pieces/pawn.hpp"
#include "pieces/king.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

Game::Game()
	:pieceSelected_(false), previousMove_{MoveType::None, {File::A, Rank::One}},
	blackKingInCheck_(false), whiteKingInCheck_(false), pieceMoved_(false),
	buttonPressed_(false), lockClick_(false), playAgain_(false),
	startButton_("Play", FontType::Regular, 60, {320, 320}),
	settingsButton_("Settings", FontType::Regular, 60, {320, 400}),
	exitButton_("Exit", FontType::Regular, 60, {320, 480}),
	settingsBackButton_("Main Menu", FontType::Regular, 40, {320, 600}),
	playAgainButton_("Play Again", FontType::Regular, 35, {200, 400}),
	mainMenuButton_("Main Menu", FontType::Regular, 35, {420, 400}),
	settingsColorChoiceBrown_("Brown", FontType::Regular, 35, {280, 290}),
	settingsColorChoiceBlue_("Blue", FontType::Regular, 35, {500, 290}),
	settingsColorChoiceGreen_("Green", FontType::Regular, 35, {400, 290}),
	settingsAudioChoiceYes_("Yes", FontType::Regular, 35, {280, 420}),
	settingsAudioChoiceNo_("No", FontType::Regular, 35, {420, 420}),
	pauseDrawButton_("Draw Game", FontType::Regular, 35, {210, 360}),
	pauseWhiteForfeitButton_("White Forfeit", FontType::Regular, 35, {440, 290}),
	pauseBlackForfeitButton_("Black Forfeit", FontType::Regular, 35, {210, 290}),
	pauseQuitButton_("Quit Game", FontType::Regular, 35, {440, 420}),
	pauseMenuButton_("Main Menu", FontType::Regular, 35, {440, 360}),
	returnToGame_("Resume", FontType::Regular, 35, {210, 420}),
	titleText_(myriadBoldFont_), settingsTitleText_(myriadBoldFont_), gameOverTitleText_(myriadBoldFont_), winnerText_(myriadSemiboldFont_),
	settingsAudioText_(myriadRegularFont_), settingsColorText_(myriadRegularFont_), pauseTitle_(myriadBoldFont_),
	pieceMoveSound_(pieceMoveBuffer_), captureSound_(captureBuffer_), buttonClickSound_(buttonClickBuffer_),
	gameStartSound_(gameStartBuffer_), gameEndSound_(gameEndBuffer_), castleSound_(castleBuffer_),
	isDone_(false), selectedPiecePosition_(File::A, Rank::One)
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
		gameStartSound_.play();
		changeGamestate(State::PlayingGame);
	}
	else if (gameState_ == State::GameOver)
	{
		gameEndSound_.play();
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
	std::optional event {window_.pollEvent()};
	sf::Vector2i mousePos {sf::Mouse::getPosition(window_)};
	bool leftButtonClicked {false};
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
	startButton_.update(mousePosition);
	settingsButton_.update(mousePosition);
	exitButton_.update(mousePosition);

	if (leftButtonClicked)
	{
		if (startButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::CreateGame);
			buttonPressed_ = true;
		}
		if (settingsButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::Settings);
			buttonPressed_ = true;
		}
		if (exitButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			isDone_ = true;
			buttonPressed_ = true;
		}
	}
}

void Game::settingsState(sf::Vector2i mousePosition, bool leftButtonClicked)
{
	settingsBackButton_.update(mousePosition);
	settingsAudioChoiceNo_.update(mousePosition);
	settingsAudioChoiceYes_.update(mousePosition);
	settingsColorChoiceBrown_.update(mousePosition);
	settingsColorChoiceBlue_.update(mousePosition);
	settingsColorChoiceGreen_.update(mousePosition);

	if (leftButtonClicked)
	{
		if (settingsBackButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (settingsAudioChoiceNo_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			setAudio(false);
			buttonPressed_ = true;
		}
		if (settingsAudioChoiceYes_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			setAudio(true);
			buttonPressed_ = true;
		}
		if (settingsColorChoiceBrown_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			theme_.setTheme(ThemeSet::Brown);
			updateTheme();
			buttonPressed_ = true;
		}
		if (settingsColorChoiceBlue_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			theme_.setTheme(ThemeSet::Blue);
			updateTheme();
			buttonPressed_ = true;
		}
		if (settingsColorChoiceGreen_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			theme_.setTheme(ThemeSet::Green);
			updateTheme();
			buttonPressed_ = true;
		}
	}
}

void Game::updateTheme()
{
	// Board & background
	createBackground();
	background_.setFillColor(theme_.alternate);
	gameOverBackground_.setFillColor(theme_.alternate);

	// Texts
	titleText_.setFillColor(theme_.darkMain);
	settingsTitleText_.setFillColor(theme_.darkMain);
	gameOverTitleText_.setFillColor(theme_.darkMain);
	winnerText_.setFillColor(theme_.darkMain);
	settingsColorText_.setFillColor(theme_.darkMain);
	settingsAudioText_.setFillColor(theme_.darkMain);
	pauseBackground_.setFillColor(theme_.alternate);
	pauseTitle_.setFillColor(theme_.darkMain);

	// Buttons
	startButton_.setColor(theme_.darkMain, theme_.lightMain);
	settingsButton_.setColor(theme_.darkMain, theme_.lightMain);
	exitButton_.setColor(theme_.darkMain, theme_.lightMain);
	settingsBackButton_.setColor(theme_.darkMain, theme_.lightMain);
	playAgainButton_.setColor(theme_.darkMain, theme_.lightMain);
	mainMenuButton_.setColor(theme_.darkMain, theme_.lightMain);
	settingsColorChoiceBrown_.setColor(theme_.darkMain, theme_.lightMain);
	settingsColorChoiceBlue_.setColor(theme_.darkMain, theme_.lightMain);
	settingsColorChoiceGreen_.setColor(theme_.darkMain, theme_.lightMain);
	settingsAudioChoiceYes_.setColor(theme_.darkMain, theme_.lightMain);
	settingsAudioChoiceNo_.setColor(theme_.darkMain, theme_.lightMain);
	pauseDrawButton_.setColor(theme_.darkMain, theme_.lightMain);
	pauseWhiteForfeitButton_.setColor(theme_.darkMain, theme_.lightMain);
	pauseBlackForfeitButton_.setColor(theme_.darkMain, theme_.lightMain);
	pauseMenuButton_.setColor(theme_.darkMain, theme_.lightMain);
	pauseQuitButton_.setColor(theme_.darkMain, theme_.lightMain);
	returnToGame_.setColor(theme_.darkMain, theme_.lightMain);
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
	pauseDrawButton_.update(mousePosition);
	pauseWhiteForfeitButton_.update(mousePosition);
	pauseBlackForfeitButton_.update(mousePosition);
	pauseMenuButton_.update(mousePosition);
	pauseQuitButton_.update(mousePosition);
	returnToGame_.update(mousePosition);

	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::PlayingGame);
		}
	}

	if (leftButtonClicked)
	{
		if (pauseDrawButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			gameOutcome_ = GameOutcome::Stalemate;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseWhiteForfeitButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			gameOutcome_ = GameOutcome::BlackWins;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseBlackForfeitButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			gameOutcome_ = GameOutcome::WhiteWins;
			playAgain_ = true;
			changeGamestate(State::GameOver);
			buttonPressed_ = true;
		}
		if (pauseMenuButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (pauseQuitButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			isDone_ = true;
			buttonPressed_ = true;
		}
		if (returnToGame_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::PlayingGame);
			buttonPressed_ = true;
		}
	}
}


bool Game::playingGameState(sf::Vector2i actualMousePosition, std::optional<sf::Event> event, bool leftButtonClicked)
{
    int fileIndex {std::clamp(actualMousePosition.x / SQUARE_SIZE, static_cast<int>(toIndex(File::A)), static_cast<int>(toIndex(File::H)))};
    int rankIndex {std::clamp(actualMousePosition.y / SQUARE_SIZE, static_cast<int>(toIndex(Rank::One)), static_cast<int>(toIndex(Rank::Eight)))};
	Position mousePosition {static_cast<File>(fileIndex), static_cast<Rank>(rankIndex)};
	// static Position selectedPiecePosition {File::A, Rank::One};

	// auto& mousePosPiece {board_[mousePosition]};
	// auto& selectedPiece {board_[selectedPiecePosition]};

	// Options
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::Pause);
		}
	}

	// Piece is not selected
	if (!pieceSelected_ && leftButtonClicked && board_[mousePosition] && board_[mousePosition]->getTeam() == playerTurn_)
	{
		// Clicks a spot where your teams piece isn't
		// if (!board_[mousePosition] || board_[mousePosition]->getTeam() != playerTurn_)
		// {
		// 	return false;
		// }

		// Gets moves for piece
		possibleMoves_ = board_[mousePosition]->calculateMoves(board_, previousMove_);

		// Remove moves if piece's king is in check
		removeInvalidMoves(board_[mousePosition]->getTeam(), mousePosition);

		displayMoves();
		pieceSelected_ = true;
		lockClick_ = true;
		selectedPiecePosition_ = mousePosition;
	}


	if (pieceSelected_ && leftButtonClicked)
	{
	    // auto& selectedPiece {board_[selectedPiecePosition]};
	    // Checks if the new tile selected is the same team as the piece that is trying to move
		if (board_[mousePosition] && board_[mousePosition]->getTeam() == board_[selectedPiecePosition_]->getTeam())
		{
			// Clear moves
			moveCircles_.clear();
			possibleMoves_.clear();

			// Gets moves for piece
			possibleMoves_ = board_[mousePosition]->calculateMoves(board_, previousMove_);

			// Remove moves if piece's king is in check
			removeInvalidMoves(board_[mousePosition]->getTeam(), mousePosition);

			displayMoves();
			pieceSelected_ = true;
			lockClick_ = true;
			selectedPiecePosition_ = mousePosition;
			return false;
		}

		for (Move move : possibleMoves_)
		{
			if (mousePosition == move.position) // Checks if move is a possible move
			{
				if (willBeInCheck(selectedPiecePosition_, mousePosition, board_[selectedPiecePosition_]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
				{
					continue;
				}

				// Sets the current move type to capture if the place the piece is moving to has a piece already there
				if (board_[mousePosition])
				{
					move.moveType = MoveType::Capture;
				}

				// adds moves to the history
				previousMove_ = move;

				// Plays sounds for the moves
				if (move.moveType == MoveType::Castle)
				{
					castleSound_.play();
				}
				else if (move.moveType == MoveType::Capture || move.moveType == MoveType::EnPassant)
				{
					captureSound_.play();
				}
				else
				{
					pieceMoveSound_.play();
				}

				// Changes pawn into queen if it reaches the end
				if (board_[selectedPiecePosition_]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePosition_]->getTeam() == Team::White && mousePosition.rank == Rank::One)
				{
					board_[mousePosition] = std::make_unique<SlidingPiece>(Team::White, mousePosition, PieceType::Queen);
					board_[selectedPiecePosition_].reset();
				}
				else if (board_[selectedPiecePosition_]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePosition_]->getTeam() == Team::Black && mousePosition.rank == Rank::Eight)
				{
					board_[mousePosition] = std::make_unique<SlidingPiece>(Team::Black, mousePosition, PieceType::Queen);
					board_[selectedPiecePosition_].reset();
				}
				else
				{
					board_[mousePosition] = std::move(board_[selectedPiecePosition_]);
				}

				// Movement of rook for castling
				if (move.moveType == MoveType::Castle)
				{
					if (move.position == Position{File::C, Rank::One})
					{
						board_[{File::D, Rank::One}] = std::move(board_[{File::A, Rank::One}]);
					}
					if (move.position == Position{File::G, Rank::One})
					{
						board_[{File::F, Rank::One}] = std::move(board_[{File::H, Rank::One}]);
					}
					if (move.position == Position{File::C, Rank::Eight})
					{
						board_[{File::D, Rank::Eight}] = std::move(board_[{File::A, Rank::Eight}]);
					}
					if (move.position == Position{File::G, Rank::Eight})
					{
						board_[{File::F, Rank::Eight}] = std::move(board_[{File::H, Rank::Eight}]);
					}
				}

				// Removes the piece that en passant takes from the baord
				if (move.moveType == MoveType::EnPassant)
				{
					if (move.position.rank == Rank::Three)
					{
						board_[{move.position.file, Rank::Four}] = nullptr;
					}
					if (move.position.rank == Rank::Six)
					{
						board_[{move.position.file, Rank::Five}] = nullptr;
					}
				}

				endTurn(mousePosition);

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
	mainMenuButton_.update(mousePosition);
	playAgainButton_.update(mousePosition);

	if (leftButtonClicked)
	{
		if (mainMenuButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::Menu);
			buttonPressed_ = true;
		}
		if (playAgainButton_.getMouseInText() && !buttonPressed_)
		{
			buttonClickSound_.play();
			changeGamestate(State::CreateGame);
			buttonPressed_ = true;
		}
	}
}

// Creates the fonts and texts
void Game::createTexts()
{
	sf::FloatRect textBounds {};

	background_.setFillColor(theme_.alternate);
	background_.setSize({800, 800});

	gameOverBackground_.setFillColor(theme_.alternate);
	gameOverBackground_.setOutlineThickness(3);
	gameOverBackground_.setOutlineColor(sf::Color::Black);
	gameOverBackground_.setPosition({100, 200});
	gameOverBackground_.setSize({440, 240});

	pauseBackground_.setFillColor(theme_.alternate);
	pauseBackground_.setOutlineThickness(3);
	pauseBackground_.setOutlineColor(sf::Color::Black);
	pauseBackground_.setPosition({70, 170});
	pauseBackground_.setSize({500, 300});

	titleText_ = sf::Text(myriadBoldFont_, "Chess", 185);
	titleText_.setFillColor(theme_.darkMain);
	textBounds = titleText_.getLocalBounds();
	titleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	titleText_.setPosition({320, 160});

	settingsTitleText_ = sf::Text(myriadBoldFont_, "Settings", 135);
	settingsTitleText_.setFillColor(theme_.darkMain);
	textBounds = settingsTitleText_.getLocalBounds();
	settingsTitleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsTitleText_.setPosition({320, 120});

	gameOverTitleText_ = sf::Text(myriadBoldFont_, "Game Over", 80);
	gameOverTitleText_.setFillColor(theme_.darkMain);
	textBounds = gameOverTitleText_.getLocalBounds();
	gameOverTitleText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	gameOverTitleText_.setPosition({320, 235});

	winnerText_ = sf::Text(myriadSemiboldFont_, "Stalemate", 45);
	winnerText_.setFillColor(theme_.darkMain);
	textBounds = winnerText_.getLocalBounds();
	winnerText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	winnerText_.setPosition({320, 290});

	settingsColorText_ = sf::Text(myriadRegularFont_, "Color:", 50);
	settingsColorText_.setFillColor(theme_.darkMain);
	textBounds = settingsColorText_.getLocalBounds();
	settingsColorText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsColorText_.setPosition({120, 290});

	settingsAudioText_ = sf::Text(myriadRegularFont_, "Audio:", 50);
	settingsAudioText_.setFillColor(theme_.darkMain);
	textBounds = settingsAudioText_.getLocalBounds();
	settingsAudioText_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsAudioText_.setPosition({120, 420});

	pauseTitle_ = sf::Text(myriadBoldFont_, "Pause Menu", 80);
	pauseTitle_.setFillColor(theme_.darkMain);
	textBounds = pauseTitle_.getLocalBounds();
	pauseTitle_.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	pauseTitle_.setPosition({320, 210});
}

// Removes any moves that puts king in check
void Game::removeInvalidMoves(Team kingTeam, Position oldPosition)
{
	auto iterator {possibleMoves_.begin()};
	int i {0};
	while (iterator != possibleMoves_.end())
	{
		Position temp {possibleMoves_.at(i).position.file, possibleMoves_.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iterator = possibleMoves_.erase(iterator);
		}
		else
		{
			iterator++;
			i++;
		}
	}
}

// Removes any moves that put king in check given a specific set of possible moves
void Game::removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves)
{
	auto iterator {moves.begin()};
	int i {0};
	while (iterator != moves.end())
	{
		Position temp {moves.at(i).position.file, moves.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iterator = moves.erase(iterator);
		}
		else
		{
			iterator++;
			i++;
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
		sf::CircleShape tempCircle(circleRadius_);
		tempCircle.setFillColor(theme_.moveCircle);
		tempCircle.setPosition(sf::Vector2f(toIndex(i.position.file) * SQUARE_SIZE + circleRadius_, toIndex(i.position.rank) * SQUARE_SIZE + circleRadius_));
		moveCircles_.push_back(tempCircle);
	}
}

// Updates once every frame
void Game::update()
{
	while (const std::optional event = window_.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			isDone_ = true;
		}
	}

	for (File file = File::A; file <= File::H; file += 1) // Updates the positions of the pieces
	{
		for (Rank rank = Rank::One; rank <= Rank::Eight; rank += 1)
		{
		    Position position {file, rank};
			if (!board_[position])
			{
				continue;
			}
			board_[position]->setPosition(position);
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
	int count {0};
	for (const auto& file : board_)
	{
		for (const auto& piece : file)
		{
			if (piece)
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
	std::vector<Position> allMoves {};
	for (const auto& file : board_)
	{
		for (const auto& piece : file)
		{
			if (!piece || piece->getTeam() != team)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, previousMove_)};

			removeInvalidMoves(team, piece->getPosition(), pieceMoves);

			for (const Move& move : pieceMoves)
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

	for (const auto& file : board)
	{
		for (const auto& piece : file)
		{
			if (piece && piece->getPieceType() == PieceType::King && piece->getTeam() == kingTeam)
			{
				return piece->getPosition();
			}
		}
	}
	return {File::A, Rank::One}; // should fail if king not found
}

// Checks if the piece at the given position is in check
bool Game::isInCheck(Position kingPosition, Team kingTeam)
{
	for (const auto& file : board_)
	{
		for (const auto& piece : file)
		{
			if (!piece || piece->getTeam() == kingTeam)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, previousMove_)};

			for (const Move& move : pieceMoves)
			{
			    if (move.position == kingPosition)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// Checks if the piece at the given position is in check on potential board
bool Game::willBeInCheck(Position oldPosition, Position newPosition, Team team)
{
	createPotentialBoard(oldPosition, newPosition, team);
	Position kingPos = getKing(team, false);

	for (const auto& file : potentialBoard_)
	{
		for (const auto& piece : file)
		{
			if (!piece || piece->getTeam() == team)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(potentialBoard_, previousMove_)};

			for (const Move& move : pieceMoves)
			{
				if (kingPos == move.position)
				{
				    return true;
				}
			}
		}
	}

	return false;
}

// Creates the potential board
void Game::createPotentialBoard(Position oldPosition, Position newPosition, Team pieceTeam)
{
	// Resets the board
	for (File file = File::A; file <= File::H; file += 1)
	{
		for (Rank rank = Rank::One; rank <= Rank::Eight; rank += 1)
		{
		    Position position {file, rank};
			if (board_[position])
			{
			    potentialBoard_[position] = board_[position]->clone();
			}
			else
			{
                potentialBoard_[position] = nullptr;
			}
		}
	}

	// Moves the piece to where it would be
	potentialBoard_[newPosition] = std::move(potentialBoard_[oldPosition]);
}

// Called at the end of a players turn
void Game::endTurn(Position mousePosition)
{
	playerTurn_ = (playerTurn_ == Team::White) ? Team::Black: Team::White;

	board_[mousePosition]->setFirstMove(false);

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
			window_.draw(startButton_);
			window_.draw(settingsButton_);
			window_.draw(exitButton_);
			break;
		case State::Settings:
			window_.draw(background_);
			window_.draw(settingsBackButton_);
			window_.draw(settingsTitleText_);
			window_.draw(settingsAudioText_);
			window_.draw(settingsColorText_);
			window_.draw(settingsAudioChoiceNo_);
			window_.draw(settingsAudioChoiceYes_);
			window_.draw(settingsColorChoiceBrown_);
			window_.draw(settingsColorChoiceGreen_);
			window_.draw(settingsColorChoiceBlue_);
			break;
		case State::GameOver:
			renderBoard();
			window_.draw(gameOverBackground_);
			window_.draw(gameOverTitleText_);
			window_.draw(winnerText_);
			window_.draw(mainMenuButton_);
			window_.draw(playAgainButton_);
			break;
		case State::Pause:
			renderBoard();
			window_.draw(pauseBackground_);
			window_.draw(pauseTitle_);
			window_.draw(pauseDrawButton_);
			window_.draw(pauseWhiteForfeitButton_);
			window_.draw(pauseBlackForfeitButton_);
			window_.draw(pauseMenuButton_);
			window_.draw(pauseQuitButton_);
			window_.draw(returnToGame_);
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
			if (!piece)
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
                    texture = piece->getTeam() == Team::White ? whiteKingTexture_ : blackKingTexture_;
    			    break;
    			case PieceType::Queen:
                    texture = piece->getTeam() == Team::White ? whiteQueenTexture_ : blackQueenTexture_;
    			    break;
    			case PieceType::Bishop:
                    texture = piece->getTeam() == Team::White ? whiteBishopTexture_ : blackBishopTexture_;
    			    break;
    			case PieceType::Rook:
                    texture = piece->getTeam() == Team::White ? whiteRookTexture_ : blackRookTexture_;
    				break;
    			case PieceType::Knight:
                    texture = piece->getTeam() == Team::White ? whiteKnightTexture_ : blackKnightTexture_;
    			    break;
    			default:
                    texture = whitePawnTexture_;
    			    break;
			}
			sf::Sprite sprite(texture);
			Position pos = piece->getPosition();
			sprite.setPosition({toIndex(pos.file) * 80.f, toIndex(pos.rank) * 80.f});
			sprite.setScale({SCALE, SCALE});
			window_.draw(sprite);
		}
	}
}

// Creates the background tile array
void Game::createBackground()
{
	sf::RectangleShape lightRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	lightRect.setFillColor(theme_.alternate);

	sf::RectangleShape darkRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	darkRect.setFillColor(theme_.darkMain);

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
	board_[{File::E, Rank::Eight}] = std::make_unique<King>(Team::White, Position{File::E, Rank::Eight});
	board_[{File::D, Rank::Eight}] = std::make_unique<SlidingPiece>(Team::White, Position{File::D, Rank::Eight}, PieceType::Queen);
	board_[{File::C, Rank::Eight}] = std::make_unique<SlidingPiece>(Team::White, Position{File::C, Rank::Eight}, PieceType::Bishop);
	board_[{File::F, Rank::Eight}] = std::make_unique<SlidingPiece>(Team::White, Position{File::F, Rank::Eight}, PieceType::Bishop);
	board_[{File::B, Rank::Eight}] = std::make_unique<Knight>(Team::White, Position{File::B, Rank::Eight});
	board_[{File::G, Rank::Eight}] = std::make_unique<Knight>(Team::White, Position{File::G, Rank::Eight});
	board_[{File::A, Rank::Eight}] = std::make_unique<SlidingPiece>(Team::White, Position{File::A, Rank::Eight}, PieceType::Rook);
	board_[{File::H, Rank::Eight}] = std::make_unique<SlidingPiece>(Team::White, Position{File::H, Rank::Eight}, PieceType::Rook);
	for (File file = File::A; file <= File::H; file += 1)
	{
		board_[{file, Rank::Seven}] = std::make_unique<Pawn>(Team::White, Position{file, Rank::Seven});
	}

	// Add black pieces
	board_[{File::E, Rank::One}] = std::make_unique<King>(Team::Black, Position{File::E, Rank::One});
	board_[{File::D, Rank::One}] = std::make_unique<SlidingPiece>(Team::Black, Position{File::D, Rank::One}, PieceType::Queen);
	board_[{File::C, Rank::One}] = std::make_unique<SlidingPiece>(Team::Black, Position{File::C, Rank::One}, PieceType::Bishop);
	board_[{File::F, Rank::One}] = std::make_unique<SlidingPiece>(Team::Black, Position{File::F, Rank::One}, PieceType::Bishop);
	board_[{File::B, Rank::One}] = std::make_unique<Knight>(Team::Black, Position{File::B, Rank::One});
	board_[{File::G, Rank::One}] = std::make_unique<Knight>(Team::Black, Position{File::G, Rank::One});
	board_[{File::A, Rank::One}] = std::make_unique<SlidingPiece>(Team::Black, Position{File::A, Rank::One}, PieceType::Rook);
	board_[{File::H, Rank::One}] = std::make_unique<SlidingPiece>(Team::Black, Position{File::H, Rank::One}, PieceType::Rook);
	for (File file = File::A; file <= File::H; file += 1)
	{
		board_[{file, Rank::Two}] = std::make_unique<Pawn>(Team::Black, Position{file, Rank::Two});
	}

	// Add black spaces
	for (File file = File::A; file <= File::H; file += 1)
	{
		for (Rank rank = Rank::Three; rank <= Rank::Six; rank += 1)
		{
			board_[{file, rank}] = nullptr;
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
	loadAudio(pieceMoveBuffer_, "../assets/audio/move.mp3");
	loadAudio(captureBuffer_, "../assets/audio/capture.mp3");
	loadAudio(gameStartBuffer_, "../assets/audio/game-start.mp3");
	loadAudio(gameEndBuffer_, "../assets/audio/game-end.mp3");
	loadAudio(castleBuffer_, "../assets/audio/castle.mp3");
	loadAudio(buttonClickBuffer_, "../assets/audio/button-click.mp3");
}
