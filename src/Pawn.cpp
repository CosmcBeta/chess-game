#include "Pawn.hpp"

Pawn::Pawn(Team t, sf::Vector2f p_pos, sf::Texture p_texture)
	:Piece(t, PieceType::PAWN, p_pos, p_texture)
{}

void Pawn::calcMoves(Piece* p_field[8][8])
{
	possibleMoves.clear();
	bool moveAdded = false; // Makes sure no dublicate moves are added to vector
	
	if (m_team == Team::BLACK) // going down
	{
		if (m_firstMove)
		{
			if (p_field[m_position.x][m_position.y + 1] == nullptr)
			{
				possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, (m_sprite.getPosition().y + 80)));
				moveAdded = true;
				if (p_field[m_position.x][m_position.y + 2] == nullptr)
				{
					possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, (m_sprite.getPosition().y + 160)));
				}
			}
		}
		
		if (m_position.y == 7)
			return;
		else if (p_field[m_position.x][m_position.y + 1] == nullptr && !moveAdded)
		{
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + 80));
		}

		if (m_position.x < 7 && p_field[m_position.x + 1][m_position.y + 1] != nullptr && p_field[m_position.x + 1][m_position.y + 1]->getTeam() != m_team)
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x + 80, (m_sprite.getPosition().y + 80)));
		if (m_position.x > 0 && p_field[m_position.x - 1][m_position.y + 1] != nullptr && p_field[m_position.x - 1][m_position.y + 1]->getTeam() != m_team)
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x - 80, (m_sprite.getPosition().y + 80)));

	}
	else // going up
	{
		if (m_firstMove)
		{
			if (p_field[m_position.x][m_position.y - 1] == nullptr)
			{
				possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, (m_sprite.getPosition().y - 80)));
				moveAdded = true;
				if (p_field[m_position.x][m_position.y - 2] == nullptr)
				{
					possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, (m_sprite.getPosition().y - 160)));
				}
			}
		}
		
		if (m_position.y == 0)
			return;
		else if (p_field[m_position.x][m_position.y - 1] == nullptr && !moveAdded)
		{
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y - 80));
		}
		if (m_position.x < 7 && p_field[m_position.x + 1][m_position.y - 1] != nullptr && p_field[m_position.x + 1][m_position.y - 1]->getTeam() != m_team)
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x + 80, (m_sprite.getPosition().y - 80)));
		if (m_position.x > 0 && p_field[m_position.x - 1][m_position.y - 1] != nullptr && p_field[m_position.x - 1][m_position.y - 1]->getTeam() != m_team)
			possibleMoves.push_back(sf::Vector2f(m_sprite.getPosition().x - 80, (m_sprite.getPosition().y - 80)));

	}
}