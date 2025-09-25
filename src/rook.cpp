#include "rook.hpp"

Rook::Rook(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::Rook, position, texture)
{}

void Rook::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();
	sf::Vector2i deltaPos(0, 0);

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