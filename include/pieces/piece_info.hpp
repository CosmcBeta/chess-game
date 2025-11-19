#pragma once

#include <memory>

constexpr int FIRST = 0;
constexpr int LAST = 7;

class Piece;
using Board = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;

struct Position
{
    int file; // first position in 2d array (x)
    int rank;

    bool operator==(const Position& other) const
    {
        return file == other.file && rank == other.rank;
    }
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
