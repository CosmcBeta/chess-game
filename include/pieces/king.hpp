#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <initializer_list>
#include <vector>

class King : public Piece
{
public:
	King(Team team, Position position);

	virtual std::vector<Move> calculateMoves(const Board& board, Move previousMove) const;

	bool checkCastle(int rookFile, int pieceRank, const Board& board, std::initializer_list<int> emptySpaces) const;
};
