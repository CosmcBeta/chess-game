#include "knight.hpp"

Knight::Knight(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::Knight, position, texture)
{}

void Knight::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();
	
	std::array<sf::Vector2i, 8> possibleSquares =
	{
		sf::Vector2i(-2, -1),
		sf::Vector2i(-2, 1),
		sf::Vector2i(2, -1),
		sf::Vector2i(2, 1),
		sf::Vector2i(-1, -2),
		sf::Vector2i(-1, 2),
		sf::Vector2i(1, -2),
		sf::Vector2i(1, 2)
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


}