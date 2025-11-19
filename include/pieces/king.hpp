#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <initializer_list>
#include <vector>
#include <memory>

class King : public Piece
{
public:
	King(Team team, Position position);

	std::vector<Move> calculateMoves(const Board& board, Move previousMove) const override;
	std::unique_ptr<Piece> clone() const override;

	bool checkCastle(int rookFile, int pieceRank, const Board& board, std::initializer_list<int> emptySpaces) const;
};
