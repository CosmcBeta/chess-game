#include "king.hpp"

King::King(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::King, position, texture)
{}

void King::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();

	std::array<sf::Vector2i, 8> possibleSquares =
	{
		sf::Vector2i(-1, -1),
		sf::Vector2i(-1, 0),
		sf::Vector2i(-1, 1),
		sf::Vector2i(0, 1),
		sf::Vector2i(0, -1),
		sf::Vector2i(1, -1),
		sf::Vector2i(1, 0),
		sf::Vector2i(1, 1)
	};

	for (auto& move : possibleSquares)
	{
		sf::Vector2i tempPosition(position_.x + move.x, position_.y + move.y);

		if (tempPosition.x < 0 || tempPosition.x > 7 || tempPosition.y < 0 || tempPosition.y > 7)
			continue;

		if (board[tempPosition.x][tempPosition.y] == nullptr)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(tempPosition.x * SQUARE_SIZE, tempPosition.y * SQUARE_SIZE)});

		if (board[tempPosition.x][tempPosition.y] != nullptr && board[tempPosition.x][tempPosition.y]->getTeam() != team_)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(tempPosition.x * SQUARE_SIZE, tempPosition.y * SQUARE_SIZE)});
	}

	if (firstMove_ && team_ == Team::Black) // Checks if king has moved
	{
		if (board[0][0] != nullptr && board[0][0]->getPieceType() == PieceType::Rook && board[0][0]->getTeam() == team_ && board[0][0]->getFirstMove()) // checks if piece in corner is the Rook and hasn't moved
		{
			if (board[1][0] == nullptr && board[2][0] == nullptr && board[3][0] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Castle, sf::Vector2f(2 * SQUARE_SIZE, 0)});
			}
		}
		if (board[7][0] != nullptr && board[7][0]->getPieceType() == PieceType::Rook && board[7][0]->getTeam() == team_ && board[7][0]->getFirstMove())
		{
			if (board[6][0] == nullptr && board[5][0] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Castle, sf::Vector2f(6 * SQUARE_SIZE, 0)});
			}
		}
	}

	if (firstMove_ && team_ == Team::White) // Checks if king has moved
	{
		if (board[0][7] != nullptr && board[0][7]->getPieceType() == PieceType::Rook && board[0][7]->getTeam() == team_ && board[0][7]->getFirstMove()) // checks if piece in corner is the Rook and hasn't moved
		{
			if (board[1][7] == nullptr && board[2][7] == nullptr && board[3][7] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Castle, sf::Vector2f(2 * SQUARE_SIZE, 7 * SQUARE_SIZE)});
			}
		}
		if (board[7][7] != nullptr && board[7][7]->getPieceType() == PieceType::Rook && board[7][7]->getTeam() == team_ && board[7][7]->getFirstMove())
		{
			if (board[6][7] == nullptr && board[5][7] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Castle, sf::Vector2f(6 * SQUARE_SIZE, 7 * SQUARE_SIZE)});
			}
		}
	}
}