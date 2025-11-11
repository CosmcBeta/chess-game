#pragma once

struct Position
{
    int file;
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
