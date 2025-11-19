#pragma once

#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <span>
#include <utility>
#include <vector>

class SlidingPiece : public Piece
{
public:
    SlidingPiece(Team team, Position position, PieceType type);

    virtual std::vector<Move> calculateMoves(const Board& board, Move previousMove) const;

private:
    std::span<const std::pair<int, int>> directions_;
};
