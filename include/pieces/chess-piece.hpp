#pragma once 

#include <SFML/Graphics.hpp>

class Piece;

using Board = std::array<std::array<Piece*, 8>, 8>;
const int SQUARE_SIZE = 80;

enum class Team
{
	White,
	Black
};

enum class PieceType
{
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
};

enum class MoveType
{
	Normal,
	Castle,
	EnPassant,
	PawnDouble,
	Capture,
	None
};

struct Move
{
	MoveType moveType;
	sf::Vector2f position;
};

class Piece
{
public:
	Piece(Team team, PieceType type, sf::Vector2i position, sf::Texture& texture);

	Team getTeam();
	PieceType getPieceType();
	sf::Vector2f getPosition();
	sf::Vector2i getArrayPosition();
	sf::Sprite getSprite();

	void setPosition(sf::Vector2f position);
	virtual void calculateMoves(Board board, Move previousMove);
	std::vector<Move> getMoves();
	void setFirstMove(bool firstMove);
	bool getFirstMove();
	void setEnPassant(bool enPassant);
	bool getEnPassant();
	
protected:
	static constexpr float SCALE = 0.24024024f;
	
	sf::Sprite sprite_;
	sf::Texture texture_;
	sf::Vector2i position_; // Position on an 8x8 scale;ex. if actual pos is 180, then this is 2; used for array
	Team team_;
	PieceType pieceType_;
	std::vector<Move> possibleMoves_;
	bool firstMove_;
	bool canEnPassant_;
};

