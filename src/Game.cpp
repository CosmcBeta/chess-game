#include "Game.hpp"

Game::Game()
	:m_window("Chess", sf::Vector2u(640, 640)),
	m_resourceManager(ResourceManager::getInstance()),
	playerTurn(Team::WHITE),
	gameState(State::WHITE_TURN),
	pieceSelected(false),
	redHighlight(243, 60, 66, 255),
	yellowHighlight(246, 246, 129, 255),
	lightBack(236, 236, 208, 255),
	darkBack(181, 136, 95, 255),
	grayCircle(140, 140, 140, 160),
	bkInCheck(false), wkInCheck(false)
{
	restartClock();
	srand(time(NULL));

	m_resourceManager.loadResources();
	
	createBackground();
	createPieces();
}

Game::~Game()
{}

sf::Time Game::getElapsed() { return m_elapsed; }
void Game::restartClock() { m_elapsed = m_clock.restart(); }
Window* Game::getWindow() { return &m_window; }

// Handles input from user
void Game::handleInput()
{
	static bool lock_click;
	static sf::Vector2i selectedPiecePos(0, 0); // Array scale
	sf::Event event = m_window.getEvent();
	sf::Vector2i mousePosArray(0, 0), mousePos(0, 0);
	{
		sf::Vector2i tempPos = m_window.getMousePos();
		sf::Vector2i mousePosArray(tempPos.x / 80, tempPos.y / 80);
		sf::Vector2i mousePos(mousePosArray.x * 80, mousePosArray.y * 80);
	}
	
	// add wait event so that it doesn't continuously check for events

	if (event.type == sf::Event::MouseButtonPressed && pieceSelected == false) // If no piece is selected
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

				// Remove moves if in check
				if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::KING)
					removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam());
				
				displayMoves();
				pieceSelected = true;
				selectedPiecePos = mousePosArray;
			}
			lock_click = true;
		}
	}

	if (event.type == sf::Event::MouseButtonPressed && pieceSelected == true) // If there is a piece selected
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

				// Remove moves if in check
				if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::KING)
					removeInvalidMoves(m_field[mousePosArray.x][mousePosArray.y]->getTeam());
				
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
						m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];
						m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

						if (m_field[mousePosArray.x][mousePosArray.y]->getPieceType() == PieceType::KING && m_field[mousePosArray.x][mousePosArray.y]->getFirstMove())
						{ // Movement of rook for castling
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
						if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() == PieceType::KING && isInCheck(mousePosArray, Team::BLACK)) // Checks if the new position will keep the king in check
						{
							std::cout << "Not a valid move\n";
							continue;
						}// not king and new spot gets king out of check
						else if (m_field[selectedPiecePos.x][selectedPiecePos.y]->getPieceType() != PieceType::KING && isInCheckNotKing())// create a fake board where piece is here and check for check  //getKing(Team::BLACK))
						else
						{
							m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];
							m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

							endTurn(mousePosArray);
							bkInCheck = false;
							break;
						}
					}
				}
			}
			else if (wkInCheck)
			{
				for (sf::Vector2f move : possibleMoves) // Moves if correct move
				{
					if (static_cast<sf::Vector2f>(mousePos) == move)
					{
						if (isInCheck(mousePosArray, Team::WHITE))
						{
							std::cout << "Not a valid move\n";
							continue;
						}
						else
						{
							m_field[mousePosArray.x][mousePosArray.y] = m_field[selectedPiecePos.x][selectedPiecePos.y];
							m_field[selectedPiecePos.x][selectedPiecePos.y] = nullptr;

							endTurn(mousePosArray);
							wkInCheck = false;
							break;
						}
					}
				}
			}
			
			
			lock_click = true;
		}
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			lock_click = false;
		}
	}

}

void Game::removeInvalidMoves(Team p_kingTeam)
{
	for (int i = 0; i < possibleMoves.size(); i++)
	{
		sf::Vector2i temp(possibleMoves.at(i).x / 80, possibleMoves.at(i).y / 80);
		if (isInCheck(temp, p_kingTeam))
		{
			possibleMoves.erase(possibleMoves.begin() + i);
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

// Checks if the piece at the given position is in check
bool Game::isInCheck(sf::Vector2i p_kingPos, Team p_kingTeam)
{
	std::vector<sf::Vector2i> allMoves;
	for (auto& row : m_field)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
				continue;
			
			elem->calcMoves(m_field);
			std::vector<sf::Vector2f> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp(move.x / 80, move.y / 80);
				
				if (elem->getTeam() != p_kingTeam)
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

bool Game::isInCheckNotKing(sf::Vector2i p_piecePos, Team p_pieceTeam)
{
	Piece* tempField[8][8];

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			tempField[i][j] = m_field[i][j];
		}
	}

	tempField[p_piecePos.x][p_piecePos.y] = new Piece(Team::WHITE, PieceType::PAWN, static_cast<sf::Vector2f>(p_piecePos), whitePawnTex);
	
	sf::Vector2i kingPos = getKing(p_pieceTeam);
	std::vector<sf::Vector2i> allMoves;
	
	for (auto& row : tempField)
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
				continue;
			
			elem->calcMoves(m_field);
			std::vector<sf::Vector2f> elemMoves = elem->getMoves();

			for (auto& move : elemMoves)
			{
				sf::Vector2i temp(move.x / 80, move.y / 80);
				
				if (elem->getTeam() != p_pieceTeam)
					allMoves.push_back(temp);
			}
		}
	}
	for (auto& move : allMoves)
	{
		if (kingPos == move)
			return true;
	}
	
	return false;
}

// Called at the end of a playerss turn
void Game::endTurn(sf::Vector2i p_mousepos)
{
	if (playerTurn == Team::WHITE)
	{
		playerTurn = Team::BLACK;
		gameState = State::BLACK_TURN;
	}
	else
	{
		playerTurn = Team::WHITE;
		gameState = State::WHITE_TURN;
	}

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

	m_window.endDraw(); // Display
}

// Creates the background tile array
void Game::createBackground()  
{
	sf::RectangleShape lightRect(sf::Vector2f(80, 80));
	lightRect.setFillColor(lightBack);

	sf::RectangleShape darkRect(sf::Vector2f(80, 80));
	darkRect.setFillColor(darkBack);

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
