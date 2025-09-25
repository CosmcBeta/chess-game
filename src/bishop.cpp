#include "bishop.hpp"

Bishop::Bishop(Team team, sf::Vector2i position, sf::Texture& texture)
	:Piece(team, PieceType::Bishop, position, texture)
{}

void Bishop::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();

	sf::Vector2i deltaPos(0, 0);

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
}

bool Bishop::addToVector(sf::Vector2i p_pos, Board p_field) // Returns true if loop should end
{
	if (p_field[p_pos.x + position_.x][p_pos.y + position_.y] == nullptr)
	{
		possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (p_pos.x * SQUARE_SIZE), sprite_.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return false;
	}
	else if (p_field[p_pos.x + position_.x][p_pos.y + position_.y] != nullptr &&
		p_field[p_pos.x + position_.x][p_pos.y + position_.y]->getTeam() != team_)
	{
		possibleMoves_.push_back({MoveType::Normal, sf::Vector2f(sprite_.getPosition().x + (p_pos.x * SQUARE_SIZE), sprite_.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return true;
	}
	else
		return true;
}