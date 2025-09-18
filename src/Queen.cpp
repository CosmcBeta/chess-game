#include "Queen.hpp"

Queen::Queen(Team t, sf::Vector2f p_pos, sf::Texture& p_texture)
	:Piece(t, PieceType::QUEEN, p_pos, p_texture)
{}

void Queen::calcMoves(Board p_field, Move p_previousMove)
{
	possibleMoves.clear();

	sf::Vector2i deltaPos(0, 0);

	// Diagonal

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

	// Lines

	deltaPos.x = deltaPos.y = 0;

	while (deltaPos.x + m_position.x <= 7)
	{
		if (deltaPos.x != 0)
		{
			if (p_field[deltaPos.x + m_position.x][m_position.y] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (deltaPos.x * 80), m_sprite.getPosition().y)});
			}
			else if (p_field[deltaPos.x + m_position.x][m_position.y] != nullptr && p_field[deltaPos.x + m_position.x][m_position.y]->getTeam() != m_team)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (deltaPos.x * 80), m_sprite.getPosition().y)});
				break;
			}
			else
				break;
		}
		deltaPos.x++;
	}
	deltaPos.x = 0;
	while (deltaPos.x + m_position.x >= 0)
	{
		if (deltaPos.x != 0)
		{
			if (p_field[deltaPos.x + m_position.x][m_position.y] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (deltaPos.x * 80), m_sprite.getPosition().y)});
			}
			else if (p_field[deltaPos.x + m_position.x][m_position.y] != nullptr && p_field[deltaPos.x + m_position.x][m_position.y]->getTeam() != m_team)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (deltaPos.x * 80), m_sprite.getPosition().y)});
				break;
			}
			else
				break;
		}
		deltaPos.x--;
	}
	while (deltaPos.y + m_position.y <= 7)
	{
		if (deltaPos.y != 0)
		{
			if (p_field[m_position.x][deltaPos.y + m_position.y] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + (deltaPos.y * 80))});
			}
			else if (p_field[m_position.x][deltaPos.y + m_position.y] != nullptr && p_field[m_position.x][deltaPos.y + m_position.y]->getTeam() != m_team)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + (deltaPos.y * 80))});
				break;
			}
			else
				break;
		}
		deltaPos.y++;
	}
	deltaPos.y = 0;
	while (deltaPos.y + m_position.y >= 0)
	{
		if (deltaPos.y != 0)
		{
			if (p_field[m_position.x][deltaPos.y + m_position.y] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + (deltaPos.y * 80))});
			}
			else if (p_field[m_position.x][deltaPos.y + m_position.y] != nullptr && p_field[m_position.x][deltaPos.y + m_position.y]->getTeam() != m_team)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + (deltaPos.y * 80))});
				break;
			}
			else
				break;
		}
		deltaPos.y--;
	}
}

bool Queen::addToVector(sf::Vector2i p_pos, Board p_field) // Returns true if loop should end
{
	if (p_field[p_pos.x + m_position.x][p_pos.y + m_position.y] == nullptr)
	{
		possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (p_pos.x * 80), m_sprite.getPosition().y + (p_pos.y * 80))});
		return false;
	}
	else if (p_field[p_pos.x + m_position.x][p_pos.y + m_position.y] != nullptr &&
		p_field[p_pos.x + m_position.x][p_pos.y + m_position.y]->getTeam() != m_team)
	{
		possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + (p_pos.x * 80), m_sprite.getPosition().y + (p_pos.y * 80))});
		return true;
	}
	else
		return true;
}