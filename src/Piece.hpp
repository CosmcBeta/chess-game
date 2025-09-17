#pragma once 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

class Piece;

using Board = std::array<std::array<Piece*, 8>, 8>;

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

enum class MoveType
{
	NORMAL,
	CASTLE,
	EN_PASSANT,
	PAWN_DOUBLE
};

class Move
{
public:
	Move(MoveType p_moveType, sf::Vector2f p_pos);

	MoveType moveType;
	sf::Vector2f pos;
};

class Piece
{
public:
	Piece(Team t, PieceType type, sf::Vector2f p_pos, sf::Texture& p_texture);

	Team getTeam();
	PieceType getPieceType();
	sf::Vector2f getPos();
	sf::Vector2i getArrayPos();
	sf::Sprite getSprite();

	void setPos(sf::Vector2f p_pos);
	virtual void calcMoves(Board p_field);
	std::vector<Move> getMoves();
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
	std::vector<Move> possibleMoves;
	bool m_firstMove;
	bool m_canEnPassant;
};

