#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

class Pawn : public Piece
{
public:
	Pawn(Team team, Position position);

	virtual void calculateMoves(Board board, Move previousMove);
};
