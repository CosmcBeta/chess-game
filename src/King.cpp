#include "King.hpp"

King::King(Team t, sf::Vector2f p_pos, sf::Texture& p_texture)
	:Piece(t, PieceType::KING, p_pos, p_texture)
{}

void King::calcMoves(Board p_field)
{
	possibleMoves.clear();

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
		sf::Vector2i tempPosition(m_position.x + move.x, m_position.y + move.y);

		if (tempPosition.x < 0 || tempPosition.x > 7 || tempPosition.y < 0 || tempPosition.y > 7)
			continue;

		if (p_field[tempPosition.x][tempPosition.y] == nullptr)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(tempPosition.x * 80.f, tempPosition.y * 80.f)});

		if (p_field[tempPosition.x][tempPosition.y] != nullptr && p_field[tempPosition.x][tempPosition.y]->getTeam() != m_team)
			possibleMoves.push_back({MoveType::NORMAL, sf::Vector2f(tempPosition.x * 80.f, tempPosition.y * 80.f)});
	}

	if (m_firstMove && m_team == Team::BLACK) // Checks if king has moved
	{
		if (p_field[0][0] != nullptr && p_field[0][0]->getPieceType() == PieceType::ROOK && p_field[0][0]->getTeam() == m_team && p_field[0][0]->getFirstMove()) // checks if piece in corner is the rook and hasn't moved
		{
			if (p_field[1][0] == nullptr && p_field[2][0] == nullptr && p_field[3][0] == nullptr)
			{
				possibleMoves.push_back({MoveType::CASTLE, sf::Vector2f(160.f, 0.f)});
			}
		}
		if (p_field[7][0] != nullptr && p_field[7][0]->getPieceType() == PieceType::ROOK && p_field[7][0]->getTeam() == m_team && p_field[7][0]->getFirstMove())
		{
			if (p_field[6][0] == nullptr && p_field[5][0] == nullptr)
			{
				possibleMoves.push_back({MoveType::CASTLE, sf::Vector2f(480.f, 0.f)});
			}
		}
	}

	if (m_firstMove && m_team == Team::WHITE) // Checks if king has moved
	{
		if (p_field[0][7] != nullptr && p_field[0][7]->getPieceType() == PieceType::ROOK && p_field[0][7]->getTeam() == m_team && p_field[0][7]->getFirstMove()) // checks if piece in corner is the rook and hasn't moved
		{
			if (p_field[1][7] == nullptr && p_field[2][7] == nullptr && p_field[3][7] == nullptr)
			{
				possibleMoves.push_back({MoveType::CASTLE, sf::Vector2f(160.f, 560.f)});
			}
		}
		if (p_field[7][7] != nullptr && p_field[7][7]->getPieceType() == PieceType::ROOK && p_field[7][7]->getTeam() == m_team && p_field[7][7]->getFirstMove())
		{
			if (p_field[6][7] == nullptr && p_field[5][7] == nullptr)
			{
				possibleMoves.push_back({MoveType::CASTLE, sf::Vector2f(480.f, 560.f)});
			}
		}
	}
}