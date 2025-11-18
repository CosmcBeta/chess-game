#pragma once

#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <span>
#include <utility>

class SlidingPiece : public Piece
{
public:
    SlidingPiece(Team team, Position position, PieceType type);

    virtual void calculateMoves(const Board& board, Move previousMove);

private:
    std::span<const std::pair<int, int>> directions_;
};
