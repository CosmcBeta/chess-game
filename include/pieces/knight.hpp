#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <memory>
#include <vector>

class Knight : public Piece
{
public:
	Knight(Team team, Position position);

	std::vector<Move> calculateMoves(const Board& board, Move previousMove) const override;
	std::unique_ptr<Piece> clone() const override;
};
