#pragma once

constexpr int FIRST = 0;
constexpr int LAST = 7;

// const int SQUARE_SIZE = 80;
// static constexpr float SCALE = 0.24024024f;

struct Position
{
    int file; // first position in 2d array (x)
    int rank;
};

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
	Position position;
};
