#include "pieces/sliding_piece.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <array>
#include <span>
#include <unordered_map>
#include <utility>

namespace
{
    using Direction = std::pair<int, int>;

    // file, rank
    constexpr std::array<Direction, 8> DIRECTIONS = {
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
        static constexpr std::array<Direction, 1> none{{Direction{0, 0}}};
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

void SlidingPiece::calculateMoves(Board board, Move previousMove)
{
    possibleMoves_.clear();

    int numberOfDirectionsLeft = directions_.size();
    std::unordered_map<Direction, Position> positions {};
    for (const Direction& dir : directions_)
    {
        positions[dir] = position_;
    }

    do
    {
        for (auto& [dir, pos] : positions)
        {
            if (pos.file == -1 || pos.rank == -1)
            {
                continue;
            }

            pos.file += dir.first;
            pos.rank += dir.second;

            if (pos.file < FIRST || pos.file > LAST || pos.rank < FIRST || pos.rank > LAST)
            {
                positions.at(dir) = {-1, -1};
                numberOfDirectionsLeft -= 1;
                continue;
            }

            if (board[pos.file][pos.rank] == nullptr)
            {
                possibleMoves_.push_back({MoveType::Normal, pos});
                positions.at(dir) = pos;
            }
            else if (board[pos.file][pos.rank]->getTeam() != team_)
            {
                possibleMoves_.push_back({MoveType::Capture, pos});
                positions.at(dir) = {-1, -1};
                numberOfDirectionsLeft -= 1;
            }
            else
            {
                positions.at(dir) = {-1, -1};
                numberOfDirectionsLeft -= 1;
            }
        }
    } while (numberOfDirectionsLeft != 0);
}
