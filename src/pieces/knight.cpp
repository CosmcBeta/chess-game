#include "pieces/knight.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <array>
#include <memory>
#include <vector>

namespace
{
	using Direction = std::pair<int, int>;

	constexpr std::array<Direction, 8> TARGETS {
		{{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}}
	};
}

Knight::Knight(Team team, Position position): Piece(team, PieceType::Knight, position) {}

std::vector<Move> Knight::calculateMoves(const Board& board, Move previousMove) const
{
	std::vector<Move> possibleMoves {};

	for (const auto& move : TARGETS)
	{
		Position tempPosition {position_.file + move.first, position_.rank + move.second};

		if (!isValid(tempPosition))
		{
			continue;
		}
		else if (!board[tempPosition])
		{
			possibleMoves.push_back({
				MoveType::Normal, {tempPosition.file, tempPosition.rank}
			});
		}
		else if (board[tempPosition]->getTeam() != team_)
		{
			possibleMoves.push_back({
				MoveType::Capture, {tempPosition.file, tempPosition.rank}
			 });
		}
	}

	return possibleMoves;
}

std::unique_ptr<Piece> Knight::clone() const
{
	return std::make_unique<Knight>(*this);
}
