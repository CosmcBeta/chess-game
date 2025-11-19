#include "pieces/king.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/chess_piece.hpp"

#include <initializer_list>
#include <algorithm>
#include <vector>

King::King(Team team, Position position)
	:Piece(team, PieceType::King, position)
{}

std::vector<Move> King::calculateMoves(const Board& board, Move previousMove) const
{
	std::vector<Move> possibleMoves {};

	std::array<Position, 8> possibleSquares =
	{
		Position(-1, -1),
		Position(-1, 0),
		Position(-1, 1),
		Position(0, 1),
		Position(0, -1),
		Position(1, -1),
		Position(1, 0),
		Position(1, 1)
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
			possibleMoves.push_back({MoveType::Normal, {tempPosition.file, tempPosition.rank}});
		}
		else if (board[tempPosition.file][tempPosition.rank]->getTeam() != team_)
		{
			possibleMoves.push_back({MoveType::Capture, {tempPosition.file, tempPosition.rank}});
		}
	}

	int rank = (team_ == Team::Black) ? FIRST : LAST;
	if (firstMove_)
	{
	    if (checkCastle(FIRST, rank, board, {1, 2, 3}))
		{
		    possibleMoves.push_back({MoveType::Castle, {2, rank}});
		}

		if (checkCastle(LAST, rank, board, {5, 6}))
		{
		    possibleMoves.push_back({MoveType::Castle, {6, rank}});
		}
	}

	return possibleMoves;
}

bool King::checkCastle(int rookFile, int pieceRank, const Board& board, std::initializer_list<int> emptySpaces) const
{
    const Piece* rook = board[rookFile][pieceRank].get();
    if (!rook || rook->getPieceType() != PieceType::Rook || rook->getTeam() != team_ || !rook->getFirstMove())
    {
        return false;
    }

    return std::all_of(emptySpaces.begin(), emptySpaces.end(), [&](int file) { return board[file][pieceRank] == nullptr; });
}
