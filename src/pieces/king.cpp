#include "pieces/king.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/chess_piece.hpp"

#include <initializer_list>
#include <algorithm>
#include <memory>
#include <vector>

namespace
{
    using Direction = std::pair<int, int>;

    constexpr std::array<Direction, 8> DIRECTIONS {
		{{-1, -1},
		{-1, 0},
		{-1, 1},
		{0, 1},
		{0, -1},
		{1, -1},
		{1, 0},
		{1, 1}}
	};
}

King::King(Team team, Position position)
	:Piece(team, PieceType::King, position)
{}

std::vector<Move> King::calculateMoves(const Board& board, Move previousMove) const
{
	std::vector<Move> possibleMoves {};

	for (const auto& move : DIRECTIONS)
	{
		Position tempPosition {position_.file + move.first, position_.rank + move.second};

		if (!isValid(tempPosition))
		{
			continue;
		}
		else if (!board[tempPosition])
		{
			possibleMoves.push_back({MoveType::Normal, tempPosition});
		}
		else if (board[tempPosition]->getTeam() != team_)
		{
			possibleMoves.push_back({MoveType::Capture, tempPosition});
		}
	}

	Rank rank = (team_ == Team::Black) ? Rank::One : Rank::Eight;
	if (firstMove_)
	{
	    if (checkCastle({File::A, rank}, board, {File::B, File::C, File::D}))
		{
		    possibleMoves.push_back({MoveType::Castle, {File::C, rank}});
		}

		if (checkCastle({File::H, rank}, board, {File::F, File::G}))
		{
		    possibleMoves.push_back({MoveType::Castle, {File::G, rank}});
		}
	}

	return possibleMoves;
}

bool King::checkCastle(Position position, const Board& board, std::initializer_list<File> emptySpaces) const
{
    const Piece* rook = board[position].get();
    if (!rook || rook->getPieceType() != PieceType::Rook || rook->getTeam() != team_ || !rook->getFirstMove())
    {
        return false;
    }

    return std::all_of(emptySpaces.begin(), emptySpaces.end(), [&](File file) { return board[position] == nullptr; });
}

std::unique_ptr<Piece> King::clone() const
{
    return std::make_unique<King>(*this);
}
