#include "pawn.hpp"

Pawn::Pawn(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::Pawn, position, texture)
{}

void Pawn::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();
	bool moveAdded = false; // Makes sure no dublicate moves are added to vector
	
	if (team_ == Team::Black) // going down
	{
		if (firstMove_)
		{
			if (board[position_.x][position_.y + 1] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + SQUARE_SIZE)});
				moveAdded = true;
				if (board[position_.x][position_.y + 2] == nullptr)
					possibleMoves_.push_back({MoveType::PawnDouble, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + (2 * SQUARE_SIZE))});
			}
		}
		
		if (position_.y == 7)
			return;
		else if (board[position_.x][position_.y + 1] == nullptr && !moveAdded)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + SQUARE_SIZE)});

		if (position_.x < 7 && board[position_.x + 1][position_.y + 1] != nullptr && board[position_.x + 1][position_.y + 1]->getTeam() != team_)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + SQUARE_SIZE, sprite_.getPosition().y + SQUARE_SIZE)});
		if (position_.x > 0 && board[position_.x - 1][position_.y + 1] != nullptr && board[position_.x - 1][position_.y + 1]->getTeam() != team_)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x - SQUARE_SIZE, sprite_.getPosition().y + SQUARE_SIZE)});

	}
	else // going up
	{
		if (firstMove_)
		{
			if (board[position_.x][position_.y - 1] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y - SQUARE_SIZE)});
				moveAdded = true;
				if (board[position_.x][position_.y - 2] == nullptr)
					possibleMoves_.push_back({MoveType::PawnDouble, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y - (2 * SQUARE_SIZE))});
			}
		}
		
		if (position_.y == 0)
			return;
		else if (board[position_.x][position_.y - 1] == nullptr && !moveAdded)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y - SQUARE_SIZE)});

		if (position_.x < 7 && board[position_.x + 1][position_.y - 1] != nullptr && board[position_.x + 1][position_.y - 1]->getTeam() != team_)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + SQUARE_SIZE, sprite_.getPosition().y - SQUARE_SIZE)});
		if (position_.x > 0 && board[position_.x - 1][position_.y - 1] != nullptr && board[position_.x - 1][position_.y - 1]->getTeam() != team_)
			possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x - SQUARE_SIZE, sprite_.getPosition().y - SQUARE_SIZE)});

	}

	// En Passant WORKDS
	if (previousMove.moveType == MoveType::PawnDouble)
	{
		if (team_ == Team::White && position_.y == 3) // in forth from top row and playing white aka going up
		{
			if (previousMove.position.x / SQUARE_SIZE == position_.x - 1 && previousMove.position.y / SQUARE_SIZE == position_.y)
				possibleMoves_.push_back({MoveType::EnPassant, sf::Vector2f(sprite_.getPosition().x - SQUARE_SIZE, sprite_.getPosition().y - SQUARE_SIZE)});

			if (previousMove.position.x / SQUARE_SIZE == position_.x + 1 && previousMove.position.y / SQUARE_SIZE == position_.y)
				possibleMoves_.push_back({MoveType::EnPassant, sf::Vector2f(sprite_.getPosition().x + SQUARE_SIZE, sprite_.getPosition().y - SQUARE_SIZE)});
		}

		if (team_ == Team::Black && position_.y == 4) // in forth from top row and playing white aka going up
		{
			if (previousMove.position.x / SQUARE_SIZE == position_.x - 1 && previousMove.position.y / SQUARE_SIZE == position_.y)
				possibleMoves_.push_back({MoveType::EnPassant, sf::Vector2f(sprite_.getPosition().x - SQUARE_SIZE, sprite_.getPosition().y + SQUARE_SIZE)});

			if (previousMove.position.x / SQUARE_SIZE == position_.x + 1 && previousMove.position.y / SQUARE_SIZE == position_.y)
				possibleMoves_.push_back({MoveType::EnPassant, sf::Vector2f(sprite_.getPosition().x + SQUARE_SIZE, sprite_.getPosition().y + SQUARE_SIZE)});
		}
	}
}