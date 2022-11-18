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

void Game::handleInput() // Handles input; mainly mouse
{
	static bool lock_click;
	static sf::Vector2i currentPiecePos(0, 0);
	sf::Event event = m_window.getEvent();
	sf::Vector2i tempPos = m_window.getMousePos();
	sf::Vector2i mousePosArray(tempPos.x / 80, tempPos.y / 80);
	sf::Vector2i mousePos(mousePosArray.x * 80, mousePosArray.y * 80);

	// add wait event so that it doesn't continuously check for events

	if (event.type == sf::Event::MouseButtonPressed && pieceSelected == false) // If no piece is selected
	{
		if (event.mouseButton.button == sf::Mouse::Left && lock_click != true)
		{
			if (m_field[mousePosArray.x][mousePosArray.y] == nullptr || m_field[mousePosArray.x][mousePosArray.y]->getTeam() != playerTurn)
				return;
			else
			{
				m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field);
				possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();
				displayMoves();
				pieceSelected = true;
				currentPiecePos = mousePosArray;
			}
			lock_click = true;
		}
	}

	if (event.type == sf::Event::MouseButtonPressed && pieceSelected == true) // If there is a piece selected
	{
		if (event.mouseButton.button == sf::Mouse::Left && lock_click != true)
		{
			if (m_field[mousePosArray.x][mousePosArray.y] != nullptr && // Checks if the new tile selected is the same team as the piece that is trying to move
				m_field[mousePosArray.x][mousePosArray.y]->getTeam() == m_field[currentPiecePos.x][currentPiecePos.y]->getTeam())
			{
				circles.clear();
				possibleMoves.clear();
				lock_click = true;
				m_field[mousePosArray.x][mousePosArray.y]->calcMoves(m_field);
				possibleMoves = m_field[mousePosArray.x][mousePosArray.y]->getMoves();
				displayMoves();
				pieceSelected = true;
				currentPiecePos = mousePosArray;
				return;
			}
			if (!wkInCheck && !bkInCheck)
			{
				std::cout << possibleMoves.size();
				for (sf::Vector2f move : possibleMoves) // Moves if correct move
				{
					if (static_cast<sf::Vector2f>(mousePos) == move)
					{
						m_field[mousePosArray.x][mousePosArray.y] = m_field[currentPiecePos.x][currentPiecePos.y];
						m_field[currentPiecePos.x][currentPiecePos.y] = nullptr;

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
			else if (bkInCheck)
			{
				std::cout << possibleMoves.size();
				for (sf::Vector2f move : possibleMoves) // Moves if correct move
				{
					if (static_cast<sf::Vector2f>(mousePos) == move)
					{
						if (isInCheck(mousePosArray)) // Checks if the new position will keep the king in check
						{
							std::cout << "Not a valid move\n";
							continue;
						}
						else
						{
							m_field[mousePosArray.x][mousePosArray.y] = m_field[currentPiecePos.x][currentPiecePos.y];
							m_field[currentPiecePos.x][currentPiecePos.y] = nullptr;

							endTurn(mousePosArray);
							bkInCheck = false;
							break;
						}
					}
				}
			}
			else if (wkInCheck)
			{
				std::cout << possibleMoves.size();
				for (sf::Vector2f move : possibleMoves) // Moves if correct move
				{
					if (static_cast<sf::Vector2f>(mousePos) == move)
					{
						if (isInCheck(mousePosArray))
						{
							std::cout << "Not a valid move\n";
							continue;
						}
						else
						{
							m_field[mousePosArray.x][mousePosArray.y] = m_field[currentPiecePos.x][currentPiecePos.y];
							m_field[currentPiecePos.x][currentPiecePos.y] = nullptr;

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

void Game::displayMoves() // Displays the moves for one piece
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

void Game::update() // Updates once every frame
{
	m_window.update();

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (m_field[i][j] == nullptr)
				continue;
			m_field[i][j]->setPos(sf::Vector2f((float)i, (float)j));
		}
	}

	sf::Vector2i wkPos(0, 0), bkPos(0, 0);

	for (auto& row : m_field) // Gets the position of the two kings
	{
		for (auto& elem : row)
		{
			if (elem == nullptr)
				continue;

			if (elem->getPieceType() == PieceType::KING && elem->getTeam() == Team::WHITE)
				wkPos = elem->getArrayPos();
			if (elem->getPieceType() == PieceType::KING && elem->getTeam() == Team::BLACK)
				bkPos = elem->getArrayPos();
		}
	}

	if (isInCheck(wkPos))
	{
		if (!wkInCheck)
			std::cout << "White king is in check\n";
		wkInCheck = true;
	}
	if (isInCheck(bkPos))
	{
		if (!bkInCheck)
			std::cout << "Black king is in check\n";
		bkInCheck = true;
	}

}

bool Game::isInCheck(sf::Vector2i p_kingPos)
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

void Game::switchTeam()
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
}

void Game::endTurn(sf::Vector2i p_mousepos)
{
	switchTeam();

	if (m_field[p_mousepos.x][p_mousepos.y]->getFirstMove()) // If first move change that since it moved
		m_field[p_mousepos.x][p_mousepos.y]->toggleFirstMove();

	circles.clear();
	possibleMoves.clear();

	pieceSelected = false;
}

void Game::render() // Displays stuff to screen
{
	m_window.beginDraw(); // Clear.

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

	m_window.endDraw(); // Display.
}

void Game::createBackground()  // Creates the background tile array
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

void Game::createPieces() // Creates all the pieces and adds them to 2d array; might try to delete them all and make objects local instead of global; could save memory
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
