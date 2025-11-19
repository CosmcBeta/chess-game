#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <vector>

class Knight : public Piece
{
public:
	Knight(Team team, Position position);

	virtual std::vector<Move> calculateMoves(const Board& board, Move previousMove) const;
};
