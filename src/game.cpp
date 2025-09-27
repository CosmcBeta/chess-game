#include "game.hpp"

#include <iostream>

Game::Game()
	:pieceSelected(false), m_previousMove{MoveType::None, {0,0}},
	blackKingInCheck(false), whiteKingInCheck(false), pieceMoved(false),
	buttonPressed(false), lockClick(false), playAgain(false),
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
	titleText(myriadBold), settingsTitleText(myriadBold), gameOverTitleText(myriadBold), winnerText(myriadSemibold),
	settingsAudioText(myriadRegular), settingsColorText(myriadRegular), pauseTitle(myriadBold),
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

sf::Time Game::getElapsed() { return m_elapsed; }
void Game::restartClock() { m_elapsed = m_clock.restart(); }

// Changes gamestates
void Game::changeGamestate(State p_newState)
{
	gameState = p_newState;

	if (gameState == State::CreateGame)
	{
		createPieces();
		playerTurn = Team::White;
		gameStartSound.play();
		changeGamestate(State::PlayingGame);
	}
	else if (gameState == State::GameOver)
	{
		gameEndSound.play();
		switch (gameOutcome)
		{
		case GameOutcome::WhiteWins:
			winnerText.setString("White Wins");
			break;
		case GameOutcome::BlackWins:
			winnerText.setString("Black Wins");
			break;
		case GameOutcome::Stalemate:
			winnerText.setString("Stalemate");
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
    	leftButtonClicked = (mouse->button == sf::Mouse::Button::Left && !lockClick);
	}
	
	switch (gameState)
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
			lockClick = false;
			buttonPressed = false;
		}
	}
}

void Game::menuState(sf::Vector2i mousePos, bool leftButtonClicked)
{
	startButton.update(mousePos);
	settingsButton.update(mousePos);
	exitButton.update(mousePos);

	if (leftButtonClicked)
	{
		if (startButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::CreateGame);
			buttonPressed = true;
		}
		if (settingsButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::Settings);
			buttonPressed = true;
		}
		if (exitButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			isDone_ = true;
			buttonPressed = true;
		}
	}
}

void Game::settingsState(sf::Vector2i mousePos, bool leftButtonClicked)
{
	settingsBackButton.update(mousePos);
	settingsAudioChoiceNo.update(mousePos);
	settingsAudioChoiceYes.update(mousePos);
	settingsColorChoiceBrown.update(mousePos);
	settingsColorChoiceBlue.update(mousePos);
	settingsColorChoiceGreen.update(mousePos);

	if (leftButtonClicked)
	{
		if (settingsBackButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed = true;
		}
		if (settingsAudioChoiceNo.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			setAudio(false);
			buttonPressed = true;
		}
		if (settingsAudioChoiceYes.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			setAudio(true);
			buttonPressed = true;
		}
		if (settingsColorChoiceBrown.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Brown);
			updateTheme();
			buttonPressed = true;
		}
		if (settingsColorChoiceBlue.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Blue);
			updateTheme();
			buttonPressed = true;
		}
		if (settingsColorChoiceGreen.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			theme.setTheme(ThemeSet::Green);
			updateTheme();
			buttonPressed = true;
		}
	}
}

void Game::updateTheme()
{
	// Board & background
	createBackground();
	background.setFillColor(theme.alternate);

	// Texts
	gameOverBackground.setFillColor(theme.alternate);
	titleText.setFillColor(theme.darkMain);
	settingsTitleText.setFillColor(theme.darkMain);
	gameOverTitleText.setFillColor(theme.darkMain);
	winnerText.setFillColor(theme.darkMain);
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

void Game::setAudio(bool p_audioOn)
{
	if (p_audioOn)
	{
		sf::Listener::setGlobalVolume(100);
	}
	else
	{
		sf::Listener::setGlobalVolume(0);
	}
}

// pauseDrawButton, pauseWhiteForfeitButton, pauseBlackForfeitButton, pauseMenuButton, pauseQuitButton
void Game::pauseState(sf::Vector2i mousePos, bool leftButtonClicked, std::optional<sf::Event> event)
{
	pauseDrawButton.update(mousePos);
	pauseWhiteForfeitButton.update(mousePos);
	pauseBlackForfeitButton.update(mousePos);
	pauseMenuButton.update(mousePos);
	pauseQuitButton.update(mousePos);
	returnToGame.update(mousePos);

	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::PlayingGame);
		}
	}

	if (leftButtonClicked)
	{
		if (pauseDrawButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			gameOutcome = GameOutcome::Stalemate;
			playAgain = true;
			changeGamestate(State::GameOver);
			buttonPressed = true;
		}
		if (pauseWhiteForfeitButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			gameOutcome = GameOutcome::BlackWins;
			playAgain = true;
			changeGamestate(State::GameOver);
			buttonPressed = true;
		}
		if (pauseBlackForfeitButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			gameOutcome = GameOutcome::WhiteWins;
			playAgain = true;
			changeGamestate(State::GameOver);
			buttonPressed = true;
		}
		if (pauseMenuButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed = true;
		}
		if (pauseQuitButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			isDone_ = true;
			buttonPressed = true;
		}
		if (returnToGame.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::PlayingGame);
			buttonPressed = true;
		}
	}
}


bool Game::playingGameState(sf::Vector2i actualMousePos, std::optional<sf::Event> event, bool leftButtonClicked)
{
	static sf::Vector2i selectedPiecePos(0, 0); // Array scale
	sf::Vector2i mousePosArray = sf::Vector2i(actualMousePos.x / SQUARE_SIZE, actualMousePos.y / SQUARE_SIZE); // 8 by 8
	sf::Vector2i mousePos = sf::Vector2i(mousePosArray.x * SQUARE_SIZE, mousePosArray.y * SQUARE_SIZE); // 640 by 640, where to put the piece basically
	
	// Options
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
		{
			changeGamestate(State::Pause);
		}
	}
		
	// Piece is not selected
	if (!pieceSelected && leftButtonClicked)
	{
		// Clicks a spot where your teams piece isn't
		if (m_field[mousePosArray.x][mousePosArray.y] == nullptr || m_field[mousePosArray.x][mousePosArray.y]->getTeam() != playerTurn)
		{
			return false;
		}
		
		// Gets moves for piece
		m_field[mousePosArray.x][mousePosArray.y]->calculateMoves(m_field, m_previousMove);
		possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();

		// Remove moves if piece's king is in check
		removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam(), mousePosArray);

		displayMoves();
		pieceSelected = true;
		selectedPiecePos = mousePosArray;
		lockClick = true;
	}


	if (pieceSelected && leftButtonClicked)
	{
		if (m_field[mousePosArray.x][mousePosArray.y] != nullptr && // Checks if the new tile selected is the same team as the piece that is trying to move
			m_field[mousePosArray.x][mousePosArray.y]->getTeam() == m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam())
		{
			// Clear moves
			moveCircles.clear();
			possibleMoves.clear();

			// Gets moves for piece
			m_field[mousePosArray.x][mousePosArray.y]->calculateMoves(m_field, m_previousMove);
			possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();

			// Remove moves if piece's king is in check
			removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam(), mousePosArray);

			displayMoves();
			pieceSelected = true;
			lockClick = true;
			selectedPiecePos = mousePosArray;
			return false;
		}

		for (Move move : possibleMoves)
		{
			if (static_cast<sf::Vector2f>(mousePos) == move.position) // Checks if move is a possible move
			{
				if (willBeInCheck(selectedPiecePos, mousePosArray, m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
				{
					continue;
				}	
				
				// Sets the current move type to capture if the place the piece is moving to has a piece already there
				if (m_field[mousePosArray.x][mousePosArray.y] != nullptr)
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
				if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::Pawn &&
					m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::White && mousePosArray.y == 0)
				{
					m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::White, sf::Vector2i(mousePosArray.x, mousePosArray.y), whiteQueenTex);
				}
				else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::Pawn &&
					m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::Black && mousePosArray.y == 7)
				{
					m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::Black, sf::Vector2i(mousePosArray.x, mousePosArray.y), blackQueenTex);
				}
				else
				{
					m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];
				}

				m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

				// Movement of rook for castling
				if (move.moveType == MoveType::Castle)
				{
					if (move.position == sf::Vector2f(2 * SQUARE_SIZE, 0))
					{
						m_field[3][0] = m_field[0][0];
						m_field[0][0] = nullptr;
					}
					if (move.position == sf::Vector2f(6 * SQUARE_SIZE, 0))
					{
						m_field[5][0] = m_field[7][0];
						m_field[7][0] = nullptr;
					}
					if (move.position == sf::Vector2f(2 * SQUARE_SIZE, 7 * SQUARE_SIZE))
					{
						m_field[3][7] = m_field[0][7];
						m_field[0][7] = nullptr;
					}
					if (move.position == sf::Vector2f(6 * SQUARE_SIZE, 7 * SQUARE_SIZE))
					{
						m_field[5][7] = m_field[7][7];
						m_field[7][7] = nullptr;
					}
				}

				// Removes the piece that en passant takes from the baord
				if (move.moveType == MoveType::EnPassant)
				{
					if (move.position.y / SQUARE_SIZE == 2)
					{
						m_field[move.position.x / SQUARE_SIZE][3] = nullptr;
					}
					if (move.position.y / SQUARE_SIZE == 5)
					{
						m_field[move.position.x / SQUARE_SIZE][4] = nullptr;
					}
				}

				endTurn(mousePosArray);

				if (blackKingInCheck)
				{
					blackKingInCheck = false;
				}
				else if (whiteKingInCheck)
				{
					whiteKingInCheck = false;
				}
				
				break;
			}
		}
		pieceMoved = true;
		lockClick = true;
	}
	return true;
}

void Game::gameOverState(sf::Vector2i mousePos, bool leftButtonClicked)
{
	mainMenuButton.update(mousePos);
	playAgainButton.update(mousePos);

	if (leftButtonClicked)
	{
		if (mainMenuButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::Menu);
			buttonPressed = true;
		}
		if (playAgainButton.getMouseInText() && !buttonPressed)
		{
			buttonClickSound.play();
			changeGamestate(State::CreateGame);
			buttonPressed = true;
		}
	}
}

// Creates the fonts and texts
void Game::createTexts()
{
	sf::FloatRect textBounds;
	
	background.setFillColor(theme.alternate);
	background.setSize({800, 800});
	
	gameOverBackground.setFillColor(theme.alternate);
	gameOverBackground.setOutlineThickness(3);
	gameOverBackground.setOutlineColor(sf::Color::Black);
	gameOverBackground.setPosition({100, 200});
	gameOverBackground.setSize({440, 240});
	
	pauseBackground.setFillColor(theme.alternate);
	pauseBackground.setOutlineThickness(3);
	pauseBackground.setOutlineColor(sf::Color::Black);
	pauseBackground.setPosition({70, 170});
	pauseBackground.setSize({500, 300});
	
	titleText = sf::Text(myriadBold, "Chess", 185);
	titleText.setFillColor(theme.darkMain);
	textBounds = titleText.getLocalBounds();
	titleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	titleText.setPosition({320, 160});
	
	settingsTitleText = sf::Text(myriadBold, "Settings", 135);
	settingsTitleText.setFillColor(theme.darkMain);
	textBounds = settingsTitleText.getLocalBounds();
	settingsTitleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsTitleText.setPosition({320, 120});
	
	gameOverTitleText = sf::Text(myriadBold, "Game Over", 80);
	gameOverTitleText.setFillColor(theme.darkMain);
	textBounds = gameOverTitleText.getLocalBounds();
	gameOverTitleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	gameOverTitleText.setPosition({320, 235});
	
	winnerText = sf::Text(myriadSemibold, "Stalemate", 45);
	winnerText.setFillColor(theme.darkMain);
	textBounds = winnerText.getLocalBounds();
	winnerText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	winnerText.setPosition({320, 290});
	
	settingsColorText = sf::Text(myriadRegular, "Color:", 50);
	settingsColorText.setFillColor(theme.darkMain);
	textBounds = settingsColorText.getLocalBounds();
	settingsColorText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsColorText.setPosition({120, 290});
	
	settingsAudioText = sf::Text(myriadRegular, "Audio:", 50);
	settingsAudioText.setFillColor(theme.darkMain);
	textBounds = settingsAudioText.getLocalBounds();
	settingsAudioText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsAudioText.setPosition({120, 420});
	
	pauseTitle = sf::Text(myriadBold, "Pause Menu", 80);
	pauseTitle.setFillColor(theme.darkMain);
	textBounds = pauseTitle.getLocalBounds();
	pauseTitle.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	pauseTitle.setPosition({320, 210});
}

// Removes any moves that puts king in check
void Game::removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos)
{
	auto iter = possibleMoves.begin();
	int i = 0;
	while (iter != possibleMoves.end())
	{
		sf::Vector2i temp((int)possibleMoves.at(i).position.x / SQUARE_SIZE, (int)possibleMoves.at(i).position.y / SQUARE_SIZE);
		if (willBeInCheck(p_oldPos, temp, p_kingTeam))
		{
			iter = possibleMoves.erase(iter);
		}
		else
		{
			++iter;
			++i;
		}
	}
}

// Removes any moves that put king in check given a specific set of possible moves
void Game::removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos, std::vector<Move>& p_moves)
{
	auto iter = p_moves.begin();
	int i = 0;
	while (iter != p_moves.end())
	{
		sf::Vector2i temp((int)p_moves.at(i).position.x / SQUARE_SIZE, (int)p_moves.at(i).position.y / SQUARE_SIZE);
		if (willBeInCheck(p_oldPos, temp, p_kingTeam))
		{
			iter = p_moves.erase(iter);
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
	moveCircles.clear();

	for (auto& i : possibleMoves)
	{
		// Create circle for each point and adds them to an array
		sf::CircleShape tempCircle(circleRadius);
		tempCircle.setFillColor(theme.moveCircle);
		tempCircle.setPosition(sf::Vector2f(i.position.x + circleRadius, i.position.y + circleRadius));
		moveCircles.push_back(tempCircle);
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

	for (int r = 0; r < 8; r++) // Updates the positions of the pieces
	{
		for (int c = 0; c < 8; c++)
		{
			if (m_field[r][c] == nullptr)
			{
				continue;
			}
			m_field[r][c]->setPosition(sf::Vector2f((float)r, (float)c));
		}
	}

	whiteKingPos = getKing(Team::White, true);
	blackKingPos = getKing(Team::Black, true);

	if (isInCheck(whiteKingPos, Team::White))
	{
		whiteKingInCheck = true;
	}
	if (isInCheck(blackKingPos, Team::Black))
	{
		blackKingInCheck = true;
	}
	if (pieceMoved)
	{
		if (getTotalMoveCount(Team::White) == 0)
		{
			if (isInCheck(whiteKingPos, Team::White))
			{
				gameOutcome = GameOutcome::BlackWins;
			}
			else
			{
				gameOutcome = GameOutcome::Stalemate;
			}
			playAgain = true;
			changeGamestate(State::GameOver);
		}
		if (getTotalMoveCount(Team::Black) == 0)
		{
			if (isInCheck(blackKingPos, Team::Black))
			{
				gameOutcome = GameOutcome::WhiteWins;
			}
			else
			{
				gameOutcome = GameOutcome::Stalemate;
			}
			playAgain = true;
			changeGamestate(State::GameOver);
		}
		if (numberOfPieces() == 2)
		{
			gameOutcome = GameOutcome::Stalemate;
			playAgain = true;
			changeGamestate(State::GameOver);
		}
		pieceMoved = false;
	}
}

// Returns the total number of pieces on the board
int Game::numberOfPieces()
{
	int count = 0;
	for (auto& row : m_field)
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
int Game::getTotalMoveCount(Team p_team)
{
	std::vector<sf::Vector2i> allMoves;
	for (auto& row : m_field)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr || elem->getTeam() != p_team)
			{
				continue;
			}

			elem->calculateMoves(m_field, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			removeInvalidMoves(p_team, elem->getArrayPosition(), elemMoves);

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.position.x / SQUARE_SIZE, (int)move.position.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}
	return static_cast<int>(allMoves.size());
}

// Gets the position of the king on either the current or potential board based on the bool
sf::Vector2i Game::getKing(Team kingTeam, bool currentBoard)
{
	Board& board = currentBoard ? m_field : m_potentialField;

	for (auto& row : board)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
			{
				continue;
			}

			if (elem->getPieceType() == PieceType::King && elem->getTeam() == kingTeam)
			{
				return elem->getArrayPosition();
			}
		}
	}
	return sf::Vector2i(0, 0); // should fail if king not found
}

// Checks if the piece at the given position is in check
bool Game::isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam)
{
	std::vector<sf::Vector2i> allMoves;
	for (auto& row : m_field)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr || elem->getTeam() == p_kingTeam)
			{
				continue;
			}

			elem->calculateMoves(m_field, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.position.x / SQUARE_SIZE, (int)move.position.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}
	for (auto& move : allMoves)
	{
		if (p_kingPos == move)
		{
			return true;
		}
	}

	return false;
}

// Checks if the piece at the given position is in check on potential board
bool Game::willBeInCheck(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_team)
{
	createPotentialBoard(p_oldPos, p_newPos, p_team);

	std::vector<sf::Vector2i> allMoves;
	for (auto& row : m_potentialField)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr || elem->getTeam() == p_team)
			{
				continue;
			}

			elem->calculateMoves(m_potentialField, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.position.x / SQUARE_SIZE, (int)move.position.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}

	sf::Vector2i kingPos = getKing(p_team, false);

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
void Game::createPotentialBoard(sf::Vector2i p_oldPos, sf::Vector2i p_newPos, Team p_pieceTeam)
{
	// Resets the board
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_potentialField[i][j] = m_field[i][j];
		}
	}

	// Moves the piece to where it would be
	m_potentialField[p_newPos.x][p_newPos.y] = new Piece(p_pieceTeam, m_field[p_oldPos.x][p_oldPos.y]->getPieceType(), p_newPos, whitePawnTex);
	m_potentialField[p_oldPos.x][p_oldPos.y] = nullptr;
}

// Called at the end of a players turn
void Game::endTurn(sf::Vector2i p_mousepos)
{
	playerTurn = playerTurn == Team::White ? Team::Black: Team::White;

	m_field[p_mousepos.x][p_mousepos.y]->setFirstMove(false);

	moveCircles.clear();
	possibleMoves.clear();

	pieceSelected = false;
}

// Displays objects to the screen
void Game::render()
{
	window_.clear(sf::Color::Black); // Clear

	// Playing game display
	switch (gameState)
	{
		case State::PlayingGame:
			renderBoard();

			if (moveCircles.size() == 0)
			{
				break;
			}

			for (sf::CircleShape i : moveCircles)
			{
				window_.draw(i);
			}
			break;
		case State::Menu:
			window_.draw(background);
			window_.draw(titleText);
			window_.draw(startButton);
			window_.draw(settingsButton);
			window_.draw(exitButton);
			break;
		case State::Settings:
			window_.draw(background);
			window_.draw(settingsBackButton);
			window_.draw(settingsTitleText);
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
			window_.draw(gameOverBackground);
			window_.draw(gameOverTitleText);
			window_.draw(winnerText);
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
	for (sf::RectangleShape i : backgroundArray)
	{
		window_.draw(i);
	}

	for (auto& rows : m_field)
	{
		for (auto& elem : rows)
		{
			if (elem == nullptr)
			{
				continue;
			}
			sf::Sprite temp = elem->getSprite();
			window_.draw(temp);
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
				backgroundArray[j] = lightRect;
			}
			else
			{
				backgroundArray[j] = darkRect;
			}
			backgroundArray[j++].setPosition(sf::Vector2f(r * static_cast<float>(SQUARE_SIZE), c * static_cast<float>(SQUARE_SIZE)));
		}
	}
}

// Creates all the pieces and adds them to 2d array m_field
void Game::createPieces()
{
	// Add white pieces
	m_field[4][7] = new King(Team::White, {4, 7}, whiteKingTex);
	m_field[3][7] = new Queen(Team::White, {3, 7}, whiteQueenTex);
	m_field[2][7] = new Bishop(Team::White, {2, 7}, whiteBishopTex);
	m_field[5][7] = new Bishop(Team::White, {5, 7}, whiteBishopTex);
	m_field[1][7] = new Knight(Team::White, {1, 7}, whiteKnightTex);
	m_field[6][7] = new Knight(Team::White, {6, 7}, whiteKnightTex);
	m_field[0][7] = new Rook(Team::White, {0, 7}, whiteRookTex);
	m_field[7][7] = new Rook(Team::White, {7, 7}, whiteRookTex);
	for (int i = 0; i < 8; i++)
	{
		m_field[i][6] = new Pawn(Team::White, {i, 6}, whitePawnTex);
	}

	// Add black pieces
	m_field[4][0] = new King(Team::Black, {4, 0}, blackKingTex);
	m_field[3][0] = new Queen(Team::Black, {3, 0}, blackQueenTex);
	m_field[2][0] = new Bishop(Team::Black, {2, 0}, blackBishopTex);
	m_field[5][0] = new Bishop(Team::Black, {5, 0}, blackBishopTex);
	m_field[1][0] = new Knight(Team::Black, {1, 0}, blackKnightTex);
	m_field[6][0] = new Knight(Team::Black, {6, 0}, blackKnightTex);
	m_field[0][0] = new Rook(Team::Black, {0, 0}, blackRookTex);
	m_field[7][0] = new Rook(Team::Black, {7, 0}, blackRookTex);
	for (int i = 0; i < 8; i++)
	{
		m_field[i][1] = new Pawn(Team::Black, {i, 1}, blackPawnTex);
	}

	// Add black spaces
	for (int i = 2; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_field[j][i] = nullptr;
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
	loadTexture(whitePawnTex, "assets/images/white_pawn.png");
	loadTexture(whiteRookTex, "assets/images/white_rook.png");
	loadTexture(whiteBishopTex, "assets/images/white_bishop.png");
	loadTexture(whiteKnightTex, "assets/images/white_knight.png");
	loadTexture(whiteQueenTex, "assets/images/white_queen.png");
	loadTexture(whiteKingTex, "assets/images/white_king.png");
	loadTexture(blackPawnTex, "assets/images/black_pawn.png");
	loadTexture(blackRookTex, "assets/images/black_rook.png");
	loadTexture(blackBishopTex, "assets/images/black_bishop.png");
	loadTexture(blackKnightTex, "assets/images/black_knight.png");
	loadTexture(blackQueenTex, "assets/images/black_queen.png");
	loadTexture(blackKingTex, "assets/images/black_king.png");

	// Fonts
	loadFont(myriadBold, "assets/fonts/myriad_pro_bold.ttf");
	loadFont(myriadSemibold, "assets/fonts/myriad_pro_semibold.ttf");
	loadFont(myriadRegular, "assets/fonts/myriad_pro_regular.ttf");

	// Sounds
	loadAudio(pieceMoveBuffer, "assets/audio/move.mp3");
	loadAudio(captureBuffer, "assets/audio/capture.mp3");
	loadAudio(gameStartBuffer, "assets/audio/game-start.mp3");
	loadAudio(gameEndBuffer, "assets/audio/game-end.mp3");
	loadAudio(castleBuffer, "assets/audio/castle.mp3");
	loadAudio(buttonClickBuffer, "assets/audio/button-click.mp3");
}