#pragma once

#include "piece_info.hpp"

#include <vector>

class Piece;
using Board = std::array<std::array<Piece*, 8>, 8>;
// const int SQUARE_SIZE = 80;
// static constexpr float SCALE = 0.24024024f;

class Piece
{
public:
	Piece(Team team, PieceType type, Position position);

	virtual void calculateMoves(const Board& board, Move previousMove) = 0;

	Team getTeam() const;
	PieceType getPieceType() const;
	Position getPosition() const;
	std::vector<Move> getMoves() const;
	bool getFirstMove() const;
	bool getEnPassant() const;

	void setPosition(Position position);
	void setFirstMove(bool firstMove);
	void setEnPassant(bool enPassant);

protected:
	Position position_; // Position on an 8x8 scale
	Team team_;
	PieceType pieceType_;
	std::vector<Move> possibleMoves_;
	bool firstMove_;
	bool canEnPassant_;
};
