#include "game.hpp"
#include "board.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/sliding_piece.hpp"
#include "pieces/knight.hpp"
#include "pieces/pawn.hpp"
#include "pieces/king.hpp"

#include <iostream>
#include <memory>

Game::Game()
	:pieceSelected_(false), m_previousMove{MoveType::None, {File::A, Rank::One}},
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
	static Position selectedPiecePos {File::A, Rank::One}; // Array scale
	Position mousePosArray {static_cast<File>(actualMousePosition.x / SQUARE_SIZE), static_cast<Rank>(actualMousePosition.y / SQUARE_SIZE)}; // 8 by 8
	sf::Vector2i mousePos {static_cast<int>(toIndex(mousePosArray.file) * SQUARE_SIZE), static_cast<int>(toIndex(mousePosArray.rank) * SQUARE_SIZE)}; // 640 by 640, where to put the piece basically

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
		if (!board_[mousePosArray] || board_[mousePosArray]->getTeam() != playerTurn_)
		{
			return false;
		}

		// Gets moves for piece
		possibleMoves_ = board_[mousePosArray]->calculateMoves(board_, m_previousMove);

		// Remove moves if piece's king is in check
		removeInvalidMoves(board_[mousePosArray]->getTeam(), mousePosArray);

		displayMoves();
		pieceSelected_ = true;
		selectedPiecePos = mousePosArray;
		lockClick_ = true;
	}


	if (pieceSelected_ && leftButtonClicked)
	{
	    // Checks if the new tile selected is the same team as the piece that is trying to move
		if (board_[mousePosArray] && board_[mousePosArray]->getTeam() == board_[selectedPiecePos]->getTeam())
		{
			// Clear moves
			moveCircles_.clear();
			possibleMoves_.clear();

			// Gets moves for piece
			possibleMoves_ = board_[mousePosArray]->calculateMoves(board_, m_previousMove);
			// possibleMoves_ = board_[mousePosArray.file][mousePosArray.rank]->getMoves();

			// Remove moves if piece's king is in check
			removeInvalidMoves(board_[mousePosArray]->getTeam(), mousePosArray);

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
				if (willBeInCheck(selectedPiecePos, mousePosArray, board_[selectedPiecePos]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
				{
					continue;
				}

				// Sets the current move type to capture if the place the piece is moving to has a piece already there
				if (board_[mousePosArray])
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
				if (board_[selectedPiecePos]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePos]->getTeam() == Team::White && mousePosArray.rank == Rank::One)
				{
					board_[mousePosArray] = std::make_unique<SlidingPiece>(Team::White, mousePosArray, PieceType::Queen);
					board_[selectedPiecePos].reset();
				}
				else if (board_[selectedPiecePos]->getPieceType() == PieceType::Pawn &&
					board_[selectedPiecePos]->getTeam() == Team::Black && mousePosArray.rank == Rank::Eight)
				{
					board_[mousePosArray] = std::make_unique<SlidingPiece>(Team::Black, mousePosArray, PieceType::Queen);
					board_[selectedPiecePos].reset();
				}
				else
				{
					board_[mousePosArray] = std::move(board_[selectedPiecePos]);
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
	sf::FloatRect textBounds {};

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
		sf::CircleShape tempCircle(circleRadius);
		tempCircle.setFillColor(theme.moveCircle);
		tempCircle.setPosition(sf::Vector2f(toIndex(i.position.file) * SQUARE_SIZE + circleRadius, toIndex(i.position.rank) * SQUARE_SIZE + circleRadius));
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

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, m_previousMove)};

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

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, m_previousMove)};

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

			std::vector<Move> pieceMoves {piece->calculateMoves(potentialBoard_, m_previousMove)};

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
	loadAudio(pieceMoveBuffer, "../assets/audio/move.mp3");
	loadAudio(captureBuffer, "../assets/audio/capture.mp3");
	loadAudio(gameStartBuffer, "../assets/audio/game-start.mp3");
	loadAudio(gameEndBuffer, "../assets/audio/game-end.mp3");
	loadAudio(castleBuffer, "../assets/audio/castle.mp3");
	loadAudio(buttonClickBuffer, "../assets/audio/button-click.mp3");
}
