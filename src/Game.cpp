#include "Game.hpp"

Game::Game()
	:m_window("Chess", sf::Vector2u(640, 640)),
	pieceSelected(false), m_previousMove(MoveType::NONE, {0,0}),
	redHighlight(243, 60, 66, 255),
	yellowHighlight(246, 246, 129, 255),
	lightBrown(236, 236, 208, 255),
	darkBrown(181, 136, 95, 255),
	grayCircle(140, 140, 140, 160),
	textHighlight(143, 107, 74, 255),
	blackKingInCheck(false), whiteKingInCheck(false),
	pieceMoved(false), playAgain(false),
	buttonPressed(false), lockClick(false),
	startButton(sf::String("Play"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 320.f)),
	settingsButton(sf::String("Settings"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 400.f)),
	exitButton(sf::String("Exit"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 480.f)),
	settingsBackButton(sf::String("Main Menu"), FontType::REGULAR, 40u, sf::Vector2f(320.f, 600.f)),
	playAgainButton(sf::String("Play Again"), FontType::REGULAR, 35u, sf::Vector2f(200.f, 400.f)),
	mainMenuButton(sf::String("Main Menu"), FontType::REGULAR, 35u, sf::Vector2f(420.f, 400.f)),
	titleText(myriadBold), settingsTitleText(myriadBold), gameOverTitleText(myriadBold), winnerText(myriadSemibold)
{
	restartClock();
	srand(static_cast<unsigned int>(time(NULL)));

	if (!myriadBold.openFromFile("assets/myriad_pro_bold.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadRegular.openFromFile("assets/myriad_pro_regular.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadSemibold.openFromFile("assets/myriad_pro_semibold.ttf"))
		std::cerr << "Failed to open font\n";

	titleText = sf::Text(myriadBold, "Chess", 185);
	settingsTitleText = sf::Text(myriadBold, "Settings", 135);
	gameOverTitleText = sf::Text(myriadBold, "Game Over", 80);
	winnerText = sf::Text(myriadSemibold, "Stalemate", 45);

	createTexts();
	changeGamestate(State::MENU);
}

sf::Time Game::getElapsed() { return m_elapsed; }
void Game::restartClock() { m_elapsed = m_clock.restart(); }
Window* Game::getWindow() { return &m_window; }

// Changes gamestates
void Game::changeGamestate(State p_newState)
{
	gameState = p_newState;

	if (gameState == State::CREATE_GAME)
	{
		if (!playAgain)
		{
			createTextures();
				createBackground();
		}
		createPieces();
		playerTurn = Team::WHITE;
		changeGamestate(State::PLAYING_GAME);
	}
	else if (gameState == State::GAME_OVER)
	{
		switch (gameOutcome)
		{
		case GameOutcome::WHITE_WINS:
			winnerText.setString(sf::String("White Wins"));
			break;
		case GameOutcome::BLACK_WINS:
			winnerText.setString(sf::String("Black Wins"));
			break;
		case GameOutcome::STALEMATE:
			winnerText.setString(sf::String("Stalemate"));
			break;
		default:
			break;
		}
	}
}

// Handles input from user
void Game::handleInput()
{
	std::optional event = m_window.getEvent();
	sf::Vector2i mousePos = m_window.getMousePos();
	bool leftButtonClicked = false;
	if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>())
    	leftButtonClicked = (mouse->button == sf::Mouse::Button::Left && !lockClick);
	
	switch (gameState)
	{
		case State::MENU:
			menuState(mousePos, leftButtonClicked);
			break;
		case State::SETTINGS:
			settingsState(mousePos, leftButtonClicked);
			break;
		case State::PLAYING_GAME:
			if (!playingGameState(mousePos, event, leftButtonClicked)) return;
			break;
		case State::GAME_OVER:
			gameOverState(mousePos, leftButtonClicked);
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
			changeGamestate(State::CREATE_GAME);
			buttonPressed = true;
		}
		if (settingsButton.getMouseInText() && !buttonPressed)
		{
			changeGamestate(State::SETTINGS);
			buttonPressed = true;
		}
		if (exitButton.getMouseInText() && !buttonPressed)
		{
			m_window.setIsDone(true);
			buttonPressed = true;
		}
	}
}

void Game::settingsState(sf::Vector2i mousePos, bool leftButtonClicked)
{
	settingsBackButton.update(mousePos);

	if (leftButtonClicked)
	{
		if (settingsBackButton.getMouseInText() && !buttonPressed)
		{
			changeGamestate(State::MENU);
			buttonPressed = true;
		}
	}
}

bool Game::playingGameState(sf::Vector2i actualMousePos, std::optional<sf::Event> event, bool leftButtonClicked)
{
	static sf::Vector2i selectedPiecePos(0, 0); // Array scale
	sf::Vector2i mousePosArray = sf::Vector2i(actualMousePos.x / SQUARE_SIZE, actualMousePos.y / SQUARE_SIZE); // 8 by 8
	sf::Vector2i mousePos = sf::Vector2i(mousePosArray.x * SQUARE_SIZE, mousePosArray.y * SQUARE_SIZE); // 640 by 640, where to put the piece basically
	
	// Will remove/add later
	/*
	// Options
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			changeGamestate(State::MENU);
		}
	}
	*/	
	// Piece is not selected
	if (!pieceSelected && leftButtonClicked)
	{
		// Clicks a spot where your teams piece isn't
		if (m_field[mousePosArray.x][mousePosArray.y] == nullptr || m_field[mousePosArray.x][mousePosArray.y]->getTeam() != playerTurn)
			return false;
		
		// Gets moves for piece
		m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field, m_previousMove);
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
			m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field, m_previousMove);
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
			if (static_cast<sf::Vector2f>(mousePos) == move.pos) // Checks if move is a possible move
			{
				if (willBeInCheck(selectedPiecePos, mousePosArray, m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
					continue;

				m_moveHistory.push_back(move);
				m_previousMove = move;

				// Changes pawn into queen if it reaches the end
				if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
					m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::WHITE && mousePosArray.y == 0)
				{
					m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::WHITE, sf::Vector2f(static_cast<float>(mousePosArray.x), static_cast<float>(mousePosArray.y)), whiteQueenTex);
				}
				else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
					m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::BLACK && mousePosArray.y == 7)
				{
					m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::BLACK, sf::Vector2f(static_cast<float>(mousePosArray.x), static_cast<float>(mousePosArray.y)), blackQueenTex);
				}
				else
					m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];

				m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

				// Movement of rook for castling
				if (move.moveType == MoveType::CASTLE)
				{
					if (move.pos == sf::Vector2f(2 * SQUARE_SIZE, 0))
					{
						m_field[3][0] = m_field[0][0];
						m_field[0][0] = nullptr;
					}
					if (move.pos == sf::Vector2f(6 * SQUARE_SIZE, 0))
					{
						m_field[5][0] = m_field[7][0];
						m_field[7][0] = nullptr;
					}
					if (move.pos == sf::Vector2f(2 * SQUARE_SIZE, 7 * SQUARE_SIZE))
					{
						m_field[3][7] = m_field[0][7];
						m_field[0][7] = nullptr;
					}
					if (move.pos == sf::Vector2f(6 * SQUARE_SIZE, 7 * SQUARE_SIZE))
					{
						m_field[5][7] = m_field[7][7];
						m_field[7][7] = nullptr;
					}
				}

				// Removes the piece that en passant takes from the baord
				if (move.moveType == MoveType::EN_PASSANT)
				{
					if (move.pos.y / SQUARE_SIZE == 2)
						m_field[move.pos.x / SQUARE_SIZE][3] = nullptr;
					if (move.pos.y / SQUARE_SIZE == 5)
						m_field[move.pos.x / SQUARE_SIZE][4] = nullptr;
				}

				endTurn(mousePosArray);

				if (blackKingInCheck)
					blackKingInCheck = false;
				else if (whiteKingInCheck)
					whiteKingInCheck = false;
				
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
			changeGamestate(State::MENU);
			buttonPressed = true;
		}
		if (playAgainButton.getMouseInText() && !buttonPressed)
		{
			changeGamestate(State::CREATE_GAME);
			buttonPressed = true;
		}
	}
}

// Creates the fonts and texts
void Game::createTexts()
{
	sf::FloatRect textBounds;

	background.setFillColor(lightBrown);
	background.setSize(sf::Vector2f(800, 800));

	gameOverBackground.setFillColor(lightBrown);
	gameOverBackground.setOutlineThickness(3);
	gameOverBackground.setOutlineColor(sf::Color::Black);
	gameOverBackground.setPosition({100.f, 200.f});
	gameOverBackground.setSize(sf::Vector2f(440.f, 240.f));

	titleText.setFillColor(darkBrown);
	textBounds = titleText.getLocalBounds();
	titleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	titleText.setPosition(sf::Vector2f(320.f, 160.f));

	settingsTitleText.setFillColor(darkBrown);
	textBounds = settingsTitleText.getLocalBounds();
	settingsTitleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	settingsTitleText.setPosition(sf::Vector2f(320.f, 120.f));

	gameOverTitleText.setFillColor(darkBrown);
	textBounds = gameOverTitleText.getLocalBounds();
	gameOverTitleText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	gameOverTitleText.setPosition(sf::Vector2f(320.f, 235.f));

	winnerText.setFillColor(darkBrown);
	textBounds = winnerText.getLocalBounds();
	winnerText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
	winnerText.setPosition(sf::Vector2f(320.f, 290.f));
}

// Removes any moves that puts king in check
void Game::removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos)
{
	auto iter = possibleMoves.begin();
	int i = 0;
	while (iter != possibleMoves.end())
	{
		sf::Vector2i temp((int)possibleMoves.at(i).pos.x / SQUARE_SIZE, (int)possibleMoves.at(i).pos.y / SQUARE_SIZE);
		if (willBeInCheck(p_oldPos, temp, p_kingTeam))
			iter = possibleMoves.erase(iter);
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
		sf::Vector2i temp((int)p_moves.at(i).pos.x / SQUARE_SIZE, (int)p_moves.at(i).pos.y / SQUARE_SIZE);
		if (willBeInCheck(p_oldPos, temp, p_kingTeam))
			iter = p_moves.erase(iter);
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
		tempCircle.setFillColor(grayCircle);
		tempCircle.setPosition(sf::Vector2f(i.pos.x + circleRadius, i.pos.y + circleRadius));
		moveCircles.push_back(tempCircle);
	}
}

// Updates once every frame
void Game::update()
{
	m_window.update();

	for (int r = 0; r < 8; r++) // Updates the positions of the pieces
	{
		for (int c = 0; c < 8; c++)
		{
			if (m_field[r][c] == nullptr)
				continue;
			m_field[r][c]->setPos(sf::Vector2f((float)r, (float)c));
		}
	}

	whiteKingPos = getKing(Team::WHITE);
	blackKingPos = getKing(Team::BLACK);

	if (isInCheck(whiteKingPos, Team::WHITE))
	{
		if (!whiteKingInCheck)
			std::cout << "White king is in check\n";
		whiteKingInCheck = true;
	}
	if (isInCheck(blackKingPos, Team::BLACK))
	{
		if (!blackKingInCheck)
			std::cout << "Black king is in check\n";
		blackKingInCheck = true;
	}
	if (pieceMoved)
	{
		if (getTotalMoveCount(Team::WHITE) == 0)
		{
			if (isInCheck(whiteKingPos, Team::WHITE))
				gameOutcome = GameOutcome::BLACK_WINS;
			else
				gameOutcome = GameOutcome::STALEMATE;
			playAgain = true;
			changeGamestate(State::GAME_OVER);
		}
		if (getTotalMoveCount(Team::BLACK) == 0)
		{
			if (isInCheck(blackKingPos, Team::BLACK))
				gameOutcome = GameOutcome::WHITE_WINS;
			else
				gameOutcome = GameOutcome::STALEMATE;
			playAgain = true;
			changeGamestate(State::GAME_OVER);
		}
		if (numberOfPieces() == 2)
		{
			gameOutcome = GameOutcome::STALEMATE;
			playAgain = true;
			changeGamestate(State::GAME_OVER);
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
				count++;
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
				continue;

			elem->calcMoves(m_field, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			removeInvalidMoves(p_team, elem->getArrayPos(), elemMoves);

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.pos.x / SQUARE_SIZE, (int)move.pos.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}
	return static_cast<int>(allMoves.size());
}

// Gets the position of the king given its team
sf::Vector2i Game::getKing(Team p_kingTeam)
{
	for (auto& row : m_field)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
				continue;

			if (elem->getPieceType() == PieceType::KING && elem->getTeam() == p_kingTeam)
				return elem->getArrayPos();
		}
	}
	return sf::Vector2i(0, 0);
}

// Gets the position of the king on the potential board given its team 
sf::Vector2i Game::getPotentialKing(Team p_kingTeam)
{
	for (auto& row : m_potentialField)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
				continue;

			if (elem->getPieceType() == PieceType::KING && elem->getTeam() == p_kingTeam)
				return elem->getArrayPos();
		}
	}
	return sf::Vector2i(0, 0);
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
				continue;

			elem->calcMoves(m_field, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.pos.x / SQUARE_SIZE, (int)move.pos.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}
	for (auto& move : allMoves)
	{
		if (p_kingPos == move)
			return true;
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
				continue;

			elem->calcMoves(m_potentialField, m_previousMove);
			std::vector<Move> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.pos.x / SQUARE_SIZE, (int)move.pos.y / SQUARE_SIZE);
				allMoves.push_back(temp);
			}
		}
	}

	sf::Vector2i kingPos = getPotentialKing(p_team);

	for (auto& move : allMoves)
	{
		if (kingPos == move)
			return true;
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
	m_potentialField[p_newPos.x][p_newPos.y] = new Piece(p_pieceTeam, m_field[p_oldPos.x][p_oldPos.y]->getPieceType(), static_cast<sf::Vector2f>(p_newPos), whitePawnTex);
	m_potentialField[p_oldPos.x][p_oldPos.y] = nullptr;
}

// Called at the end of a players turn
void Game::endTurn(sf::Vector2i p_mousepos)
{
	if (playerTurn == Team::WHITE)
		playerTurn = Team::BLACK;
	else
		playerTurn = Team::WHITE;

	if (m_field[p_mousepos.x][p_mousepos.y]->getFirstMove())
		m_field[p_mousepos.x][p_mousepos.y]->toggleFirstMove();

	moveCircles.clear();
	possibleMoves.clear();

	pieceSelected = false;
}

// Displays objects to the screen
void Game::render()
{
	m_window.beginDraw(); // Clear

	// Playing game display
	switch (gameState)
	{
		case State::PLAYING_GAME:
			renderBoard();

			if (moveCircles.size() == 0)
				break;

			for (sf::CircleShape i : moveCircles)
			{
				m_window.draw(i);
			}
			break;
		case State::MENU:
			m_window.draw(background);
			m_window.draw(titleText);
			m_window.draw(startButton);
			m_window.draw(settingsButton);
			m_window.draw(exitButton);
			break;
		case State::SETTINGS:
			m_window.draw(background);
			m_window.draw(settingsBackButton);
			m_window.draw(settingsTitleText);
			break;
		case State::GAME_OVER:
			renderBoard();

			m_window.draw(gameOverBackground);
			m_window.draw(gameOverTitleText);
			m_window.draw(winnerText);
			m_window.draw(mainMenuButton);
			m_window.draw(playAgainButton);
			break;
		default:
			break;
	}

	m_window.endDraw(); // Display
}

void Game::renderBoard()
{
	for (sf::RectangleShape i : backgroundArray)
	{
		m_window.draw(i);
	}

	for (auto& rows : m_field)
	{
		for (auto& elem : rows)
		{
			if (elem == nullptr)
				continue;
			sf::Sprite temp = elem->getSprite();
			m_window.draw(temp);
		}
	}
}

// Creates the background tile array
void Game::createBackground()
{
	sf::RectangleShape lightRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	lightRect.setFillColor(lightBrown);

	sf::RectangleShape darkRect(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	darkRect.setFillColor(darkBrown);

	int j = 0;
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (r % 2 == 0 && j % 2 == 0 || r % 2 == 1 && j % 2 == 1)
				backgroundArray[j] = lightRect;
			else
				backgroundArray[j] = darkRect;
			backgroundArray[j++].setPosition(sf::Vector2f(r * static_cast<float>(SQUARE_SIZE), c * static_cast<float>(SQUARE_SIZE)));
		}
	}
}

// Initializes the textures used for the pieces
void Game::createTextures()
{
	if (!whitePawnTex.loadFromFile("assets/white_pawn.png"))
		std::cerr << "Failed to load texture\n";
	if (!whiteRookTex.loadFromFile("assets/white_rook.png"))
		std::cerr << "Failed to load texture\n";
	if (!whiteBishopTex.loadFromFile("assets/white_bishop.png"))
		std::cerr << "Failed to load texture\n";
	if (!whiteKnightTex.loadFromFile("assets/white_knight.png"))
		std::cerr << "Failed to load texture\n";
	if (!whiteQueenTex.loadFromFile("assets/white_queen.png"))
		std::cerr << "Failed to load texture\n";
	if (!whiteKingTex.loadFromFile("assets/white_king.png"))
		std::cerr << "Failed to load texture\n";

	if (!blackPawnTex.loadFromFile("assets/black_pawn.png"))
		std::cerr << "Failed to load texture\n";
	if (!blackRookTex.loadFromFile("assets/black_rook.png"))
		std::cerr << "Failed to load texture\n";
	if (!blackBishopTex.loadFromFile("assets/black_bishop.png"))
		std::cerr << "Failed to load texture\n";
	if (!blackKnightTex.loadFromFile("assets/black_knight.png"))
		std::cerr << "Failed to load texture\n";
	if (!blackQueenTex.loadFromFile("assets/black_queen.png"))
		std::cerr << "Failed to load texture\n";
	if (!blackKingTex.loadFromFile("assets/black_king.png"))
		std::cerr << "Failed to load texture\n";
}

// Creates all the pieces and adds them to 2d array m_field
void Game::createPieces()
{
	// Add white pieces
	m_field[4][7] = new King(Team::WHITE, sf::Vector2f(4, 7), whiteKingTex);
	m_field[3][7] = new Queen(Team::WHITE, sf::Vector2f(3, 7), whiteQueenTex);
	m_field[2][7] = new Bishop(Team::WHITE, sf::Vector2f(2, 7), whiteBishopTex);
	m_field[5][7] = new Bishop(Team::WHITE, sf::Vector2f(5, 7), whiteBishopTex);
	m_field[1][7] = new Knight(Team::WHITE, sf::Vector2f(1, 7), whiteKnightTex);
	m_field[6][7] = new Knight(Team::WHITE, sf::Vector2f(6, 7), whiteKnightTex);
	m_field[0][7] = new Rook(Team::WHITE, sf::Vector2f(0, 7), whiteRookTex);
	m_field[7][7] = new Rook(Team::WHITE, sf::Vector2f(7, 7), whiteRookTex);
	for (int i = 0; i < 8; i++)
	{
		m_field[i][6] = new Pawn(Team::WHITE, sf::Vector2f(i, 6), whitePawnTex);
	}

	// Add black pieces
	m_field[4][0] = new King(Team::BLACK, sf::Vector2f(4, 0), blackKingTex);
	m_field[3][0] = new Queen(Team::BLACK, sf::Vector2f(3, 0), blackQueenTex);
	m_field[2][0] = new Bishop(Team::BLACK, sf::Vector2f(2, 0), blackBishopTex);
	m_field[5][0] = new Bishop(Team::BLACK, sf::Vector2f(5, 0), blackBishopTex);
	m_field[1][0] = new Knight(Team::BLACK, sf::Vector2f(1, 0), blackKnightTex);
	m_field[6][0] = new Knight(Team::BLACK, sf::Vector2f(6, 0), blackKnightTex);
	m_field[0][0] = new Rook(Team::BLACK, sf::Vector2f(0, 0), blackRookTex);
	m_field[7][0] = new Rook(Team::BLACK, sf::Vector2f(7, 0), blackRookTex);
	for (int i = 0; i < 8; i++)
	{
		m_field[i][1] = new Pawn(Team::BLACK, sf::Vector2f(i, 1), blackPawnTex);
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