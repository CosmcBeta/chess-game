#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

class Knight : public Piece
{
public:
	Knight(Team team, Position position);

	virtual void calculateMoves(const Board& board, Move previousMove);
};
