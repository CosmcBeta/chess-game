#pragma once 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

enum class Team
{
	WHITE,
	BLACK
};

enum class PieceType
{
	PAWN,
	BISHOP,
	KNIGHT,
	ROOK,
	QUEEN,
	KING
};

class Piece
{
public:
	Piece(Team t, PieceType type, sf::Vector2f p_pos, sf::Texture p_texture);

	Team getTeam();
	PieceType getPieceType();
	sf::Vector2f getPos();
	sf::Vector2i getArrayPos();
	sf::Sprite getSprite();

	void setPos(sf::Vector2f p_pos);
	virtual void calcMoves(Piece* p_field[8][8]);
	std::vector<sf::Vector2f> getMoves();
	void toggleFirstMove();
	bool getFirstMove();
	void toggleEnPassant();
	bool getEnPassant();

protected:
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2i m_position; // Position on an 8x8 scale;ex. if actual pos is 180, then this is 2; used for array
	Team m_team;
	PieceType m_pieceType;
	const float scale = 0.24024024f;
	std::vector<sf::Vector2f> possibleMoves;
	bool m_firstMove;
	bool m_canEnPassant;
};

