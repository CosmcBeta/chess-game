#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <vector>

class Pawn : public Piece
{
public:
	Pawn(Team team, Position position);

	virtual std::vector<Move> calculateMoves(const Board& board, Move previousMove) const;
};
