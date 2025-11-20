#include "pieces/sliding_piece.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <array>
#include <map>
#include <span>
#include <utility>
#include <vector>
#include <memory>

namespace
{
    using Direction = std::pair<int, int>;

    // file, rank
    constexpr std::array<Direction, 8> DIRECTIONS {
        {{-1, 0},
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1}}
    };

    constexpr std::span<const Direction> rookDirections() noexcept
    {
        return {DIRECTIONS.begin(), DIRECTIONS.begin() + 4};
    }

    constexpr std::span<const Direction> bishopDirections() noexcept
    {
        return {DIRECTIONS.begin() + 4, DIRECTIONS.end()};
    }

    constexpr std::span<const Direction> queenDirections() noexcept
    {
        return {DIRECTIONS.begin(), DIRECTIONS.end()};
    }

    constexpr std::span<const Direction> emptyDirections() noexcept
    {
        static constexpr std::array<Direction, 1> none {{Direction{0, 0}}};
        return {none};
    }

    constexpr std::span<const Direction> getDirections(PieceType type)
    {
        switch (type)
        {
            case PieceType::Rook: return rookDirections();
            case PieceType::Bishop: return bishopDirections();
            case PieceType::Queen: return queenDirections();
            default: return emptyDirections();
        }
    }
}

SlidingPiece::SlidingPiece(Team team, Position position, PieceType type)
:Piece(team, type, position), directions_(getDirections(type))
{}

std::vector<Move> SlidingPiece::calculateMoves(const Board& board, Move previousMove) const
{
    std::vector<Move> possibleMoves {};

    std::map<Direction, Position> positions {};
    for (const Direction& dir : directions_)
    {
        positions[dir] = position_;
    }

    for (auto& [dir, pos] : positions)
    {
        while (true)
        {
            pos.file += dir.first;
            pos.rank += dir.second;

            if (!isValid(pos))
            {
                break;
            }

            if (!board[pos])
            {
                possibleMoves.push_back({MoveType::Normal, pos});
            }
            else if (board[pos]->getTeam() != team_)
            {
                possibleMoves.push_back({MoveType::Capture, pos});
                break;
            }
            else
            {
                break;
            }
        }
    }

    return possibleMoves;
}

std::unique_ptr<Piece> SlidingPiece::clone() const
{
    return std::make_unique<SlidingPiece>(*this);
}
