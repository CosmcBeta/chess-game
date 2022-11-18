#include "Knight.hpp"

Knight::Knight(Team t, sf::Vector2f p_pos, sf::Texture p_texture)
	:Piece(t, PieceType::KNIGHT, p_pos, p_texture)
{}

void Knight::calcMoves(Piece* p_field[8][8])
{
	possibleMoves.clear();
	
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
		sf::Vector2i tempPosition(m_position.x + move.x, m_position.y + move.y);
		
		if (tempPosition.x < 0 || tempPosition.x > 7 || tempPosition.y < 0 || tempPosition.y > 7)
			continue;

		if (p_field[tempPosition.x][tempPosition.y] == nullptr)
			possibleMoves.push_back(sf::Vector2f(tempPosition.x * 80, tempPosition.y * 80));

		if (p_field[tempPosition.x][tempPosition.y] != nullptr && p_field[tempPosition.x][tempPosition.y]->getTeam() != m_team)
			possibleMoves.push_back(sf::Vector2f(tempPosition.x * 80, tempPosition.y * 80));
	}


}