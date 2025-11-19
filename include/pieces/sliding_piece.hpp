#pragma once

#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <span>
#include <utility>
#include <vector>
#include <memory>

class SlidingPiece : public Piece
{
public:
    SlidingPiece(Team team, Position position, PieceType type);

    std::vector<Move> calculateMoves(const Board& board, Move previousMove) const override;
    std::unique_ptr<Piece> clone() const override;

private:
    std::span<const std::pair<int, int>> directions_;
};
