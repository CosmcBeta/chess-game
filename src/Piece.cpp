#include "Piece.hpp"

Piece::Piece(Team t, PieceType type, sf::Vector2f p_pos, sf::Texture p_texture)
	:m_pieceType(type), m_team(t), m_texture(p_texture), m_firstMove(true), m_canEnPassant(false)
{
	m_sprite.setTexture(m_texture);
	m_sprite.setPosition(p_pos.x * 80.f, p_pos.y * 80.f);
	m_position.x = (int)p_pos.x;
	m_position.y = (int)p_pos.y;
	m_sprite.setScale(sf::Vector2f(scale, scale));
}

Team Piece::getTeam() { return m_team; }
PieceType Piece::getPieceType() { return m_pieceType; }
sf::Vector2f Piece::getPos() { return m_sprite.getPosition(); }
sf::Vector2i Piece::getArrayPos() { return m_position; }
sf::Sprite Piece::getSprite() { return m_sprite; }
void Piece::calcMoves(Piece* p_field[8][8]) {}
std::vector<sf::Vector2f> Piece::getMoves() { return possibleMoves; }
void Piece::toggleFirstMove() { m_firstMove = !m_firstMove; }
bool Piece::getFirstMove() { return m_firstMove; }
void Piece::toggleEnPassant() { m_canEnPassant = !m_canEnPassant; }
bool Piece::getEnPassant() { return m_canEnPassant; }

void Piece::setPos(sf::Vector2f p_pos) 
{ 
	m_sprite.setPosition(p_pos.x * 80.f, p_pos.y * 80.f); 
	m_position.x = (int)p_pos.x;
	m_position.y = (int)p_pos.y;
}
