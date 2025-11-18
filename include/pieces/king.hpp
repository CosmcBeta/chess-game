#pragma once

#include "chess_piece.hpp"
#include "piece_info.hpp"

#include <initializer_list>

class King : public Piece
{
public:
	King(Team team, Position position);

	virtual void calculateMoves(const Board& board, Move previousMove);

	bool checkCastle(int rookFile, int pieceRank, const Board& board, std::initializer_list<int> emptySpaces);
};
