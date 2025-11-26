#pragma once

#include "piece_info.hpp"

#include <memory>
#include <vector>

class Piece
{
public:
	Piece(Team team, PieceType type, Position position);
	virtual ~Piece() = default;

	virtual std::vector<Move> calculateMoves(const Board& board, Move previousMove) const = 0;
	virtual std::unique_ptr<Piece> clone() const = 0;

	Team getTeam() const;
	PieceType getPieceType() const;
	Position getPosition() const;
	bool getFirstMove() const;
	bool getEnPassant() const;

	void setPosition(Position position);
	void setFirstMove(bool firstMove);
	void setEnPassant(bool enPassant);

protected:
	Position position_;
	Team team_;
	PieceType pieceType_;
	bool firstMove_;
	bool canEnPassant_;
};
