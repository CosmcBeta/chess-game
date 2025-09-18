#include "Bishop.hpp"

Bishop::Bishop(Team t, sf::Vector2f p_pos, sf::Texture& p_texture)
	:Piece(t, PieceType::BISHOP, p_pos, p_texture)
{}

void Bishop::calcMoves(Board p_field, Move p_previousMove)
{
	possibleMoves.clear();

	sf::Vector2i deltaPos(0, 0);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			while (deltaPos.x + m_position.x >= 0 && deltaPos.y + m_position.y >= 0 && deltaPos.x + m_position.x <= 7 && deltaPos.y + m_position.y <= 7)
			{
				if (deltaPos.x != 0 && deltaPos.y != 0 && addToVector(sf::Vector2i(deltaPos.x, deltaPos.y), p_field))
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
	if (p_field[p_pos.x + m_position.x][p_pos.y + m_position.y] == nullptr)
	{
		possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (p_pos.x * SQUARE_SIZE), m_sprite.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return false;
	}
	else if (p_field[p_pos.x + m_position.x][p_pos.y + m_position.y] != nullptr &&
		p_field[p_pos.x + m_position.x][p_pos.y + m_position.y]->getTeam() != m_team)
	{
		possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (p_pos.x * SQUARE_SIZE), m_sprite.getPosition().y + (p_pos.y * SQUARE_SIZE))});
		return true;
	}
	else
		return true;
}