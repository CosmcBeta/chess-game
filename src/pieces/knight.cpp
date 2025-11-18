#include "pieces/knight.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <array>

Knight::Knight(Team team, Position position)
	:Piece(team, PieceType::Knight, position)
{}

void Knight::calculateMoves(const Board& board, Move previousMove)
{
	possibleMoves_.clear();

	std::array<Position, 8> possibleSquares =
	{
        Position(-2, -1),
    	Position(-2, 1),
    	Position(2, -1),
    	Position(2, 1),
    	Position(-1, -2),
    	Position(-1, 2),
    	Position(1, -2),
    	Position(1, 2)
	};

	for (const auto& move : possibleSquares)
	{
		Position tempPosition {position_.file + move.file, position_.rank + move.rank};

		if (tempPosition.file < FIRST || tempPosition.file > LAST || tempPosition.rank < FIRST || tempPosition.rank > LAST)
		{
			continue;
		}
		else if (board[tempPosition.file][tempPosition.rank] == nullptr)
		{
			possibleMoves_.push_back({MoveType::Normal, {tempPosition.file, tempPosition.rank}});
		}
		else if (board[tempPosition.file][tempPosition.rank]->getTeam() != team_)
		{
			possibleMoves_.push_back({MoveType::Capture, {tempPosition.file, tempPosition.rank}});
		}
	}
}
