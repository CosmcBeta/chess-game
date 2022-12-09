#include "Game.hpp"

Game::Game()
	:m_window("Chess", sf::Vector2u(640, 640)),
	m_resourceManager(ResourceManager::getInstance()),
	playerTurn(Team::WHITE),
	pieceSelected(false),
	redHighlight(243, 60, 66, 255),
	yellowHighlight(246, 246, 129, 255),
	lightBrown(236, 236, 208, 255),
	darkBrown(181, 136, 95, 255),
	grayCircle(140, 140, 140, 160),
	textHighlight(143, 107, 74, 255),
	bkInCheck(false), wkInCheck(false),
	pieceMoved(false),
	background(sf::Vector2f(800, 800)), 
	startButton(sf::String("Play"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 320.f)),
	settingsButton(sf::String("Settings"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 400.f)),
	exitButton(sf::String("Exit"), FontType::REGULAR, 60u, sf::Vector2f(320.f, 480.f))
{
	restartClock();
	srand(time(NULL));

	changeGamestate(State::MENU);

	m_resourceManager.loadResources();
}

Game::~Game()
{}

sf::Time Game::getElapsed() { return m_elapsed; }
void Game::restartClock() { m_elapsed = m_clock.restart(); }
Window* Game::getWindow() { return &m_window; }

void Game::changeGamestate(State p_newState)
{
	gameState = p_newState;
	
	switch (gameState)
	{
	case State::MENU:
		createMenu();
		break;
	case State::CREATE_GAME:
		createBackground();
		createPieces();
		break;
	case State::PLAYING_GAME:

		break;
	case State::GAME_OVER:
		createGameOverScreen();
		break;
	default:
		break;
	}
}

// Handles input from user
void Game::handleInput()
{
	static bool lock_click;
	static sf::Vector2i selectedPiecePos(0, 0); // Array scale
	sf::Event event = m_window.getEvent();
	sf::Vector2i mousePosArray(0, 0), mousePos(0, 0);
	sf::Vector2i actualMousePos = m_window.getMousePos();
	mousePosArray = sf::Vector2i(actualMousePos.x / 80, actualMousePos.y / 80);
	mousePos = sf::Vector2i(mousePosArray.x * 80, mousePosArray.y * 80);

	// add wait event so that it doesn't continuously check for events - might do

	// Menu
	if (gameState == State::MENU)
	{
		startButton.update(actualMousePos);
		settingsButton.update(actualMousePos);
		exitButton.update(actualMousePos);

		// Clicking buttons
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left && lock_click != true)
			{
				if (startButton.getMouseInText())
					startGame();
				if (settingsButton.getMouseInText())
					openSettings();
				if (exitButton.getMouseInText())
					m_window.setIsDone(true);
			}
		}
	}
	
	// Playing the game
	if (gameState == State::PLAYING_GAME)
	{
		// Piece is not selected
		if (event.type == sf::Event::MouseButtonPressed && pieceSelected == false)
		{
			if (event.mouseButton.button == sf::Mouse::Left && lock_click != true)
			{
				if (m_field[mousePosArray.x][mousePosArray.y] == nullptr || m_field[mousePosArray.x][mousePosArray.y]->getTeam() != playerTurn)
					return;
				else
				{
					// Gets moves for piece
					m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field);
					possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();

					// Remove moves if piece's king is in check
					removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam(), mousePosArray);

					displayMoves();
					pieceSelected = true;
					selectedPiecePos = mousePosArray;
				}
				lock_click = true;
			}
		}

		// Piece is selected
		if (event.type == sf::Event::MouseButtonPressed && pieceSelected == true)
		{
			if (event.mouseButton.button == sf::Mouse::Left && lock_click != true)
			{
				if (m_field[mousePosArray.x][mousePosArray.y] != nullptr && // Checks if the new tile selected is the same team as the piece that is trying to move
					m_field[mousePosArray.x][mousePosArray.y]->getTeam() == m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam())
				{
					// Clear moves
					circles.clear();
					possibleMoves.clear();

					// Gets moves for piece
					m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field);
					possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();

					// Remove moves if piece's king is in check
					removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam(), mousePosArray);

					displayMoves();
					pieceSelected = true;
					lock_click = true;
					selectedPiecePos = mousePosArray;
					return;
				}

				if (!wkInCheck && !bkInCheck) // No king in check
				{
					for (sf::Vector2f move : possibleMoves)
					{
						if (static_cast<sf::Vector2f>(mousePos) == move) // Checks if move is a possible move
						{
							if (willBeInCheck(selectedPiecePos, mousePosArray, m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam()))// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
							{
								std::cout << "Not a valid move\n";
								continue;
							}

							// Changes pawn into queen if it reaches the end
							if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
								m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::WHITE && mousePosArray.y == 0)
							{
								m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::WHITE, sf::Vector2f(mousePosArray.x, mousePosArray.y), whiteQueenTex);
							}
							else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
								m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::BLACK && mousePosArray.y == 7)
							{
								m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::BLACK, sf::Vector2f(mousePosArray.x, mousePosArray.y), blackQueenTex);
							}
							else
								m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];
							
							m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;
							
							// Movement of rook for castling
							if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::KING && m_field[mousePosArray.x][mousePosArray.y]->getFirstMove())
							{ 
								if (move == sf::Vector2f(160.f, 0.f))
								{
									m_field[3][0] = m_field[0][0];
									m_field[0][0] = nullptr;
								}
								if (move == sf::Vector2f(480.f, 0.f))
								{
									m_field[5][0] = m_field[7][0];
									m_field[7][0] = nullptr;
								}
								if (move == sf::Vector2f(160.f, 560.f))
								{
									m_field[3][7] = m_field[0][7];
									m_field[0][7] = nullptr;
								}
								if (move == sf::Vector2f(480.f, 560.f))
								{
									m_field[5][7] = m_field[7][7];
									m_field[7][7] = nullptr;
								}
							}

							//// fix en passant

							//if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::PAWN &&
							//	m_field[mousePosArray.x][mousePosArray.y]->getTeam() == Team::BLACK &&
							//	mousePosArray.y == selectedPiecePos.y + 2)
							//{
							//	m_field[mousePosArray.x][mousePosArray.y]->toggleEnPassant();
							//}
							//if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::PAWN &&
							//	m_field[mousePosArray.x][mousePosArray.y]->getTeam() == Team::WHITE &&
							//	mousePosArray.y == selectedPiecePos.y - 2)
							//{
							//	m_field[mousePosArray.x][mousePosArray.y]->toggleEnPassant();
							//}


							endTurn(mousePosArray);
							break;
						}
					}
				}
				else if (bkInCheck) // Black king is in check
				{
					for (sf::Vector2f move : possibleMoves)
					{
						if (static_cast<sf::Vector2f>(mousePos) == move) // Checks if move is a possible move
						{
							if (willBeInCheck(selectedPiecePos, mousePosArray, Team::BLACK))
							{
								std::cout << "Not a valid move\n";
								continue;
							}
							else
							{
								// Changes pawn into queen if it reaches the end
								if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
									m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::WHITE && mousePosArray.y == 0)
								{
									m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::WHITE, sf::Vector2f(mousePosArray.x, mousePosArray.y), whiteQueenTex);
								}
								else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
									m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::BLACK && mousePosArray.y == 7)
								{
									m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::BLACK, sf::Vector2f(mousePosArray.x, mousePosArray.y), blackQueenTex);
								}
								else
									m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];

								m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

								endTurn(mousePosArray);
								bkInCheck = false;
								break;
							}
						}
					}
				}
				else if (wkInCheck) // White king is in check
				{
					for (sf::Vector2f move : possibleMoves) // Moves if correct move
					{
						if (static_cast<sf::Vector2f>(mousePos) == move)
						{
							if (willBeInCheck(selectedPiecePos, mousePosArray, Team::WHITE))
							{
								std::cout << "Not a valid move\n";
								continue;
							}
							else
							{
								// Changes pawn into queen if it reaches the end
								if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
									m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::WHITE && mousePosArray.y == 0)
								{
									m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::WHITE, sf::Vector2f(mousePosArray.x, mousePosArray.y), whiteQueenTex);
								}
								else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::PAWN &&
									m_field[selectedPiecePos.x][selectedPiecePos.y]->getTeam() == Team::BLACK && mousePosArray.y == 7)
								{
									m_field[mousePosArray.x][mousePosArray.y] = new Queen(Team::BLACK, sf::Vector2f(mousePosArray.x, mousePosArray.y), blackQueenTex);
								}
								else
									m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];

								m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

								endTurn(mousePosArray);
								wkInCheck = false;
								break;
							}
						}
					}
				}

				pieceMoved = true;
				lock_click = true;
			}
		}
	}
	
	// Game ended
	if (gameState == State::GAME_OVER)
	{

	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			lock_click = false;
		}
	}

}

void Game::startGame()
{
	changeGamestate(State::CREATE_GAME);
	changeGamestate(State::PLAYING_GAME);
}

void Game::createGameOverScreen()
{

}

void Game::openSettings()
{

}

void Game::cleanScreen()
{
	// Clean board
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_field[i][j] = nullptr;
		}
	}


}

void Game::createMenu()
{
	background.setFillColor(lightBrown);

	myriadBold.loadFromFile("assets/myriad_pro_bold.ttf");// = *(m_resourceManager.getInstance().getFont("boldMyriadFont")); //
	myriadRegular.loadFromFile("assets/myriad_pro_regular.ttf"); // = *(m_resourceManager.getInstance().getFont("regularMyriadFont"));
	myriadSemibold.loadFromFile("assets/myriad_pro_semibold.ttf"); // = *(m_resourceManager.getInstance().getFont("semiboldMyriadFont"));

	titleText.setString(sf::String("Chess"));
	titleText.setFont(myriadBold);
	titleText.setCharacterSize(185);
	titleText.setFillColor(darkBrown);
	titleRect = titleText.getLocalBounds();
	titleText.setOrigin(titleRect.left + titleRect.width / 2.f, titleRect.top + titleRect.height / 2.f);
	titleText.setPosition(sf::Vector2f(320.f, 160.f));
}


// Removes any moves that puts king in check
void Game::removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos)
{
	auto iter = possibleMoves.begin();
	int i = 0;
	while (iter != possibleMoves.end())
	{
		sf::Vector2i temp((int)possibleMoves.at(i).x / 80, (int)possibleMoves.at(i).y / 80);
		if (willBeInCheck(p_oldPos, temp, p_kingTeam))
			iter = possibleMoves.erase(iter);
		else
		{
			++iter;
			++i;
		}
	}
}

// Removes any moves that put king in check given the moves vector
void Game::removeInvalidMoves(Team p_kingTeam, sf::Vector2i p_oldPos, std::vector<sf::Vector2f>& p_moves)
{
	auto iter = p_moves.begin();
	int i = 0;
	while (iter != p_moves.end())
	{
		sf::Vector2i temp((int)p_moves.at(i).x / 80, (int)p_moves.at(i).y / 80);
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
	circles.clear();

	for (auto& i : possibleMoves)
	{
		// Create circle for each point and adds them to an array
		sf::CircleShape tempCircle(circleRadius);
		tempCircle.setFillColor(grayCircle);
		tempCircle.setPosition(sf::Vector2f(i.x + circleRadius, i.y + circleRadius));
		circles.push_back(tempCircle);
	}
}

// Updates once every frame
void Game::update()
{
	m_window.update();

	for (int i = 0; i < 8; i++) // Updates the positions of the pieces
	{
		for (int j = 0; j < 8; j++)
		{
			if (m_field[i][j] == nullptr)
				continue;
			m_field[i][j]->setPos(sf::Vector2f((float)i, (float)j));
		}
	}

	wkPos = getKing(Team::WHITE);
	bkPos = getKing(Team::BLACK);

	if (isInCheck(wkPos, Team::WHITE))
	{
		if (!wkInCheck)
			std::cout << "White king is in check\n";
		wkInCheck = true;
	}
	if (isInCheck(bkPos, Team::BLACK))
	{
		if (!bkInCheck)
			std::cout << "Black king is in check\n";
		bkInCheck = true;
	}
	if (pieceMoved)
	{
		if (getTotalMoveCount(Team::WHITE) == 0)
		{
			std::cout << "White Checkmate\n";
			changeGamestate(State::GAME_OVER);
		}
		if (getTotalMoveCount(Team::BLACK) == 0)
		{
			std::cout << "Black Checkmate\n";
			changeGamestate(State::GAME_OVER);
		}
		pieceMoved = false;
	}
	
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

			elem->calcMoves(m_field);
			std::vector<sf::Vector2f> elemMoves = elem->getMoves();

			removeInvalidMoves(p_team, elem->getArrayPos(), elemMoves);

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.x / 80, (int)move.y / 80);
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

			elem->calcMoves(m_field);
			std::vector<sf::Vector2f> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.x / 80, (int)move.y / 80);
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

			elem->calcMoves(m_potentialField);
			std::vector<sf::Vector2f> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp((int)move.x / 80, (int)move.y / 80);
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

	circles.clear();
	possibleMoves.clear();

	pieceSelected = false;
}

// Displays objects to the screen
void Game::render()
{
	m_window.beginDraw(); // Clear

	// Playing game display
	if (gameState == State::PLAYING_GAME)
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

		if (circles.size() != 0)
		{
			for (sf::CircleShape i : circles)
			{
				m_window.draw(i);
			}
		}
	}
	
	// Menu display
	if (gameState == State::MENU)
	{
		m_window.draw(background);
		m_window.draw(titleText);
		//m_window.draw(startText);
		m_window.draw(startButton);
		m_window.draw(settingsButton);
		m_window.draw(exitButton);
	}

	m_window.endDraw(); // Display
}

// Creates the background tile array
void Game::createBackground()
{
	sf::RectangleShape lightRect(sf::Vector2f(80, 80));
	lightRect.setFillColor(lightBrown);

	sf::RectangleShape darkRect(sf::Vector2f(80, 80));
	darkRect.setFillColor(darkBrown);

	int j = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (x % 2 == 0 && j % 2 == 0 || x % 2 == 1 && j % 2 == 1)
				backgroundArray[j] = lightRect;
			else
				backgroundArray[j] = darkRect;
			backgroundArray[j++].setPosition(sf::Vector2f(x * 80.f, y * 80.f));
		}
	}
}

// Creates all the pieces and adds them to 2d array m_field
void Game::createPieces()
{
	whitePawnTex = *(m_resourceManager.getTexture("whitePawn"));
	whiteRookTex = *(m_resourceManager.getTexture("whiteRook"));
	whiteBishopTex = *(m_resourceManager.getTexture("whiteBishop"));
	whiteKnightTex = *(m_resourceManager.getTexture("whiteKnight"));
	whiteQueenTex = *(m_resourceManager.getTexture("whiteQueen"));
	whiteKingTex = *(m_resourceManager.getTexture("whiteKing"));

	blackPawnTex = *(m_resourceManager.getTexture("blackPawn"));
	blackRookTex = *(m_resourceManager.getTexture("blackRook"));
	blackBishopTex = *(m_resourceManager.getTexture("blackBishop"));
	blackKnightTex = *(m_resourceManager.getTexture("blackKnight"));
	blackQueenTex = *(m_resourceManager.getTexture("blackQueen"));
	blackKingTex = *(m_resourceManager.getTexture("blackKing"));

	m_field[4][7] = new King(Team::WHITE, sf::Vector2f(4, 7), whiteKingTex);
	m_field[3][7] = new Queen(Team::WHITE, sf::Vector2f(3, 7), whiteQueenTex);
	m_field[2][7] = new Bishop(Team::WHITE, sf::Vector2f(2, 7), whiteBishopTex);
	m_field[5][7] = new Bishop(Team::WHITE, sf::Vector2f(5, 7), whiteBishopTex);
	m_field[1][7] = new Knight(Team::WHITE, sf::Vector2f(1, 7), whiteKnightTex);
	m_field[6][7] = new Knight(Team::WHITE, sf::Vector2f(6, 7), whiteKnightTex);
	m_field[0][7] = new Rook(Team::WHITE, sf::Vector2f(0, 7), whiteRookTex);
	m_field[7][7] = new Rook(Team::WHITE, sf::Vector2f(7, 7), whiteRookTex);
	m_field[0][6] = new Pawn(Team::WHITE, sf::Vector2f(0, 6), whitePawnTex);
	m_field[1][6] = new Pawn(Team::WHITE, sf::Vector2f(1, 6), whitePawnTex);
	m_field[2][6] = new Pawn(Team::WHITE, sf::Vector2f(2, 6), whitePawnTex);
	m_field[3][6] = new Pawn(Team::WHITE, sf::Vector2f(3, 6), whitePawnTex);
	m_field[4][6] = new Pawn(Team::WHITE, sf::Vector2f(4, 6), whitePawnTex);
	m_field[5][6] = new Pawn(Team::WHITE, sf::Vector2f(5, 6), whitePawnTex);
	m_field[6][6] = new Pawn(Team::WHITE, sf::Vector2f(6, 6), whitePawnTex);
	m_field[7][6] = new Pawn(Team::WHITE, sf::Vector2f(7, 6), whitePawnTex);

	m_field[4][0] = new King(Team::BLACK, sf::Vector2f(4, 0), blackKingTex);
	m_field[3][0] = new Queen(Team::BLACK, sf::Vector2f(3, 0), blackQueenTex);
	m_field[2][0] = new Bishop(Team::BLACK, sf::Vector2f(2, 0), blackBishopTex);
	m_field[5][0] = new Bishop(Team::BLACK, sf::Vector2f(5, 0), blackBishopTex);
	m_field[1][0] = new Knight(Team::BLACK, sf::Vector2f(1, 0), blackKnightTex);
	m_field[6][0] = new Knight(Team::BLACK, sf::Vector2f(6, 0), blackKnightTex);
	m_field[0][0] = new Rook(Team::BLACK, sf::Vector2f(0, 0), blackRookTex);
	m_field[7][0] = new Rook(Team::BLACK, sf::Vector2f(7, 0), blackRookTex);
	m_field[0][1] = new Pawn(Team::BLACK, sf::Vector2f(0, 1), blackPawnTex);
	m_field[1][1] = new Pawn(Team::BLACK, sf::Vector2f(1, 1), blackPawnTex);
	m_field[2][1] = new Pawn(Team::BLACK, sf::Vector2f(2, 1), blackPawnTex);
	m_field[3][1] = new Pawn(Team::BLACK, sf::Vector2f(3, 1), blackPawnTex);
	m_field[4][1] = new Pawn(Team::BLACK, sf::Vector2f(4, 1), blackPawnTex);
	m_field[5][1] = new Pawn(Team::BLACK, sf::Vector2f(5, 1), blackPawnTex);
	m_field[6][1] = new Pawn(Team::BLACK, sf::Vector2f(6, 1), blackPawnTex);
	m_field[7][1] = new Pawn(Team::BLACK, sf::Vector2f(7, 1), blackPawnTex);

	for (int i = 2; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_field[j][i] = nullptr;
		}
	}
}