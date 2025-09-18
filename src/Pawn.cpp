#include "Pawn.hpp"

Pawn::Pawn(Team t, sf::Vector2f p_pos, sf::Texture& p_texture)
	:Piece(t, PieceType::PAWN, p_pos, p_texture)
{}

void Pawn::calcMoves(Board p_field, Move p_previousMove)
{
	possibleMoves.clear();
	bool moveAdded = false; // Makes sure no dublicate moves are added to vector
	
	if (m_team == Team::BLACK) // going down
	{
		if (m_firstMove)
		{
			if (p_field[m_position.x][m_position.y + 1] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + SQUARE_SIZE)});
				moveAdded = true;
				if (p_field[m_position.x][m_position.y + 2] == nullptr)
					possibleMoves.push_back({MoveType::PAWN_DOUBLE, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + (2 * SQUARE_SIZE))});
			}
		}
		
		if (m_position.y == 7)
			return;
		else if (p_field[m_position.x][m_position.y + 1] == nullptr && !moveAdded)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y + SQUARE_SIZE)});

		if (m_position.x < 7 && p_field[m_position.x + 1][m_position.y + 1] != nullptr && p_field[m_position.x + 1][m_position.y + 1]->getTeam() != m_team)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + SQUARE_SIZE, m_sprite.getPosition().y + SQUARE_SIZE)});
		if (m_position.x > 0 && p_field[m_position.x - 1][m_position.y + 1] != nullptr && p_field[m_position.x - 1][m_position.y + 1]->getTeam() != m_team)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x - SQUARE_SIZE, m_sprite.getPosition().y + SQUARE_SIZE)});

	}
	else // going up
	{
		if (m_firstMove)
		{
			if (p_field[m_position.x][m_position.y - 1] == nullptr)
			{
				possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y - SQUARE_SIZE)});
				moveAdded = true;
				if (p_field[m_position.x][m_position.y - 2] == nullptr)
					possibleMoves.push_back({MoveType::PAWN_DOUBLE, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y - (2 * SQUARE_SIZE))});
			}
		}
		
		if (m_position.y == 0)
			return;
		else if (p_field[m_position.x][m_position.y - 1] == nullptr && !moveAdded)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x, m_sprite.getPosition().y - SQUARE_SIZE)});

		if (m_position.x < 7 && p_field[m_position.x + 1][m_position.y - 1] != nullptr && p_field[m_position.x + 1][m_position.y - 1]->getTeam() != m_team)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x + SQUARE_SIZE, m_sprite.getPosition().y - SQUARE_SIZE)});
		if (m_position.x > 0 && p_field[m_position.x - 1][m_position.y - 1] != nullptr && p_field[m_position.x - 1][m_position.y - 1]->getTeam() != m_team)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(m_sprite.getPosition().x - SQUARE_SIZE, m_sprite.getPosition().y - SQUARE_SIZE)});

	}

	// En Passant WORKDS
	if (p_previousMove.moveType == MoveType::PAWN_DOUBLE)
	{
		if (m_team == Team::WHITE && m_position.y == 3) // in forth from top row and playing white aka going up
		{
			if (p_previousMove.pos.x / SQUARE_SIZE == m_position.x - 1 && p_previousMove.pos.y / SQUARE_SIZE == m_position.y)
				possibleMoves.push_back({MoveType::EN_PASSANT, sf::Vector2f(m_sprite.getPosition().x - SQUARE_SIZE, m_sprite.getPosition().y - SQUARE_SIZE)});

			if (p_previousMove.pos.x / SQUARE_SIZE == m_position.x + 1 && p_previousMove.pos.y / SQUARE_SIZE == m_position.y)
				possibleMoves.push_back({MoveType::EN_PASSANT, sf::Vector2f(m_sprite.getPosition().x + SQUARE_SIZE, m_sprite.getPosition().y - SQUARE_SIZE)});
		}

		if (m_team == Team::BLACK && m_position.y == 4) // in forth from top row and playing white aka going up
		{
			if (p_previousMove.pos.x / SQUARE_SIZE == m_position.x - 1 && p_previousMove.pos.y / SQUARE_SIZE == m_position.y)
				possibleMoves.push_back({MoveType::EN_PASSANT, sf::Vector2f(m_sprite.getPosition().x - SQUARE_SIZE, m_sprite.getPosition().y + SQUARE_SIZE)});

			if (p_previousMove.pos.x / SQUARE_SIZE == m_position.x + 1 && p_previousMove.pos.y / SQUARE_SIZE == m_position.y)
				possibleMoves.push_back({MoveType::EN_PASSANT, sf::Vector2f(m_sprite.getPosition().x + SQUARE_SIZE, m_sprite.getPosition().y + SQUARE_SIZE)});
		}
	}
}