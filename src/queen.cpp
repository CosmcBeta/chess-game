#include "queen.hpp"

Queen::Queen(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::Queen, position, texture)
{}

void Queen::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();

	sf::Vector2i deltaPos(0, 0);

	// Diagonal

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			while (deltaPos.x + position_.x >= 0 && deltaPos.y + position_.y >= 0 && deltaPos.x + position_.x <= 7 && deltaPos.y + position_.y <= 7)
			{
				if (deltaPos.x != 0 && deltaPos.y != 0 && addToVector(sf::Vector2i(deltaPos.x, deltaPos.y), board))
					break;
				if (i == 0)
					deltaPos.x--;
				else
					deltaPos.x++;
				if (j == 0)
					deltaPos.y--;
				else
					deltaPos.y++;
			}
			deltaPos.y = deltaPos.x = 0;
		}
	}

	// Lines

	deltaPos.x = deltaPos.y = 0;

	while (deltaPos.x + position_.x <= 7)
	{
		if (deltaPos.x != 0)
		{
			if (board[deltaPos.x + position_.x][position_.y] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (deltaPos.x * SQUARE_SIZE), sprite_.getPosition().y)});
			}
			else if (board[deltaPos.x + position_.x][position_.y] != nullptr && board[deltaPos.x + position_.x][position_.y]->getTeam() != team_)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (deltaPos.x * SQUARE_SIZE), sprite_.getPosition().y)});
				break;
			}
			else
				break;
		}
		deltaPos.x++;
	}
	deltaPos.x = 0;
	while (deltaPos.x + position_.x >= 0)
	{
		if (deltaPos.x != 0)
		{
			if (board[deltaPos.x + position_.x][position_.y] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (deltaPos.x * SQUARE_SIZE), sprite_.getPosition().y)});
			}
			else if (board[deltaPos.x + position_.x][position_.y] != nullptr && board[deltaPos.x + position_.x][position_.y]->getTeam() != team_)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (deltaPos.x * SQUARE_SIZE), sprite_.getPosition().y)});
				break;
			}
			else
				break;
		}
		deltaPos.x--;
	}
	while (deltaPos.y + position_.y <= 7)
	{
		if (deltaPos.y != 0)
		{
			if (board[position_.x][deltaPos.y + position_.y] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + (deltaPos.y * SQUARE_SIZE))});
			}
			else if (board[position_.x][deltaPos.y + position_.y] != nullptr && board[position_.x][deltaPos.y + position_.y]->getTeam() != team_)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + (deltaPos.y * SQUARE_SIZE))});
				break;
			}
			else
				break;
		}
		deltaPos.y++;
	}
	deltaPos.y = 0;
	while (deltaPos.y + position_.y >= 0)
	{
		if (deltaPos.y != 0)
		{
			if (board[position_.x][deltaPos.y + position_.y] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + (deltaPos.y * SQUARE_SIZE))});
			}
			else if (board[position_.x][deltaPos.y + position_.y] != nullptr && board[position_.x][deltaPos.y + position_.y]->getTeam() != team_)
			{
				possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x, sprite_.getPosition().y + (deltaPos.y * SQUARE_SIZE))});
				break;
			}
			else
				break;
		}
		deltaPos.y--;
	}
}

bool Queen::addToVector(sf::Vector2i p_pos, Board board) // Returns true if loop should end
{
	if (board[p_pos.x + position_.x][p_pos.y + position_.y] == nullptr)
	{
		possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (p_pos.x * SQUARE_SIZE), sprite_.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return false;
	}
	else if (board[p_pos.x + position_.x][p_pos.y + position_.y] != nullptr &&
		board[p_pos.x + position_.x][p_pos.y + position_.y]->getTeam() != team_)
	{
		possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (p_pos.x * SQUARE_SIZE), sprite_.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return true;
	}
	else
		return true;
}