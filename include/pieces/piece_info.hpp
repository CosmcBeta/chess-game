#pragma once

#include <cstddef>
#include <memory>
#include <array>

constexpr int FIRST = 0;
constexpr int LAST = 7;

enum class File
{
    A = 0, B, C, D, E, F, G, H
};

enum class Rank
{
    One = 0, Two, Three, Four, Five, Six, Seven, Eight
};

struct Position
{
    File file; // first position in 2d array (x) columns
    Rank rank;

    bool operator==(const Position& other) const
    {
        return file == other.file && rank == other.rank;
    }
};

template <typename T>
constexpr T operator+(T e, int offset)
{
    return static_cast<T>(static_cast<int>(e) + offset);
}

template <typename T>
constexpr T operator-(T e, int offset)
{
    return static_cast<T>(static_cast<int>(e) - offset);
}

template <typename T>
constexpr T operator+=(T& e, int offset)
{
    e = e + offset;
    return e;
}

constexpr bool isValid(File f)
{
    return f >= File::A && f <= File::H;
}

constexpr bool isValid(Rank r)
{
    return r >= Rank::One && r <= Rank::Eight;
}

constexpr bool isValid(const Position& position)
{
    return isValid(position.file) && isValid(position.rank);
}

constexpr size_t toIndex(File f)
{
    return static_cast<size_t>(f);
}

constexpr size_t toIndex(Rank r)
{
    return static_cast<size_t>(r);
}

class Piece;

class Board
{
public:
    std::unique_ptr<Piece>& operator[](Position position)
    {
        return chessBoard_[toIndex(position.file)][toIndex(position.rank)];
    }

    const std::unique_ptr<Piece>& operator[](Position position) const
    {
        return chessBoard_[toIndex(position.file)][toIndex(position.rank)];
    }

private:
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> chessBoard_;
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
