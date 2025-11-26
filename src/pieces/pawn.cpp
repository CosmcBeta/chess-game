#include "pieces/pawn.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <memory>
#include <vector>

Pawn::Pawn(Team team, Position position): Piece(team, PieceType::Pawn, position) {}

std::vector<Move> Pawn::calculateMoves(const Board& board, Move previousMove) const
{
	std::vector<Move> possibleMoves {};

	if (position_.rank == Rank::One || position_.rank == Rank::Eight)
	{
		return possibleMoves;
	}

	int direction = (team_ == Team::White) ? -1 : 1;
	Rank enPassantRank = (team_ == Team::White) ? Rank::Four : Rank::Five;

	Position forwardOne {position_.file, position_.rank + direction};
	Position forwardTwo {position_.file, position_.rank + (direction * 2)};

	if (!board[forwardOne])
	{
		possibleMoves.push_back({MoveType::Normal, forwardOne});
	}

	if (firstMove_ && !board[forwardTwo])
	{
		possibleMoves.push_back({MoveType::PawnDouble, forwardTwo});
	}

	for (int offset : {-1, 1})
	{
		File adjacentFile {position_.file + offset};
		if (!isValid(adjacentFile)) continue;

		Position diagonalPosition {adjacentFile, position_.rank + direction};
		if (board[diagonalPosition] && board[diagonalPosition]->getTeam() != team_)
		{
			possibleMoves.push_back({MoveType::Capture, diagonalPosition});
		}

		if (previousMove.moveType == MoveType::PawnDouble && position_.rank == enPassantRank &&
			previousMove.position == Position {adjacentFile, position_.rank})
		{
			possibleMoves.push_back({MoveType::EnPassant, diagonalPosition});
		}
	}

	return possibleMoves;
}

std::unique_ptr<Piece> Pawn::clone() const
{
	return std::make_unique<Pawn>(*this);
}
