#include "pieces/pawn.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

Pawn::Pawn(Team team, Position position)
	:Piece(team, PieceType::Pawn, position)
{}

void Pawn::calculateMoves(Board board, Move previousMove)
{
	possibleMoves_.clear();

	if (team_ == Team::Black) // going down
	{
		if (firstMove_)
		{
			if (board[position_.file][position_.rank + 1] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, {position_.file, position_.rank + 1}});
				if (board[position_.file][position_.rank + 2] == nullptr)
				{
					possibleMoves_.push_back({MoveType::PawnDouble, {position_.file, position_.rank + 2}});
				}
			}
		}

		if (position_.rank == 7)
		{
			return;
		}
		else if (board[position_.file][position_.rank + 1] == nullptr)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file, position_.rank + 1}});
		}

		if (position_.file < 7 && board[position_.file + 1][position_.rank + 1] != nullptr && board[position_.file + 1][position_.rank + 1]->getTeam() != team_)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file + 1, position_.rank + 1}});
		}
		if (position_.file > 0 && board[position_.file - 1][position_.rank + 1] != nullptr && board[position_.file - 1][position_.rank + 1]->getTeam() != team_)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file - 1, position_.rank + 1}});
		}

	}
	else // going up
	{
		if (firstMove_)
		{
			if (board[position_.file][position_.rank - 1] == nullptr)
			{
				possibleMoves_.push_back({MoveType::Normal, {position_.file, position_.rank - 1}});
				if (board[position_.file][position_.rank - 2] == nullptr)
				{
					possibleMoves_.push_back({MoveType::PawnDouble, {position_.file, position_.rank - 2}});
				}
			}
		}

		if (position_.rank == 0)
		{
			return;
		}
		else if (board[position_.file][position_.rank - 1] == nullptr)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file, position_.rank - 1}});
		}

		if (position_.file < 7 && board[position_.file + 1][position_.rank - 1] != nullptr && board[position_.file + 1][position_.rank - 1]->getTeam() != team_)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file + 1, position_.rank - 1}});
		}
		if (position_.file > 0 && board[position_.file - 1][position_.rank - 1] != nullptr && board[position_.file - 1][position_.rank - 1]->getTeam() != team_)
		{
			possibleMoves_.push_back({MoveType::Normal, {position_.file - 1, position_.rank - 1}});
		}
	}

	// En Passant
	if (previousMove.moveType == MoveType::PawnDouble)
	{
		if (team_ == Team::White && position_.rank == 3) // in forth from top row and playing white aka going up
		{
			if (previousMove.position.file == position_.file - 1 && previousMove.position.rank == position_.rank)
			{
				possibleMoves_.push_back({MoveType::EnPassant, {position_.file - 1, position_.rank - 1}});
			}

			if (previousMove.position.file == position_.file + 1 && previousMove.position.rank == position_.rank)
			{
				possibleMoves_.push_back({MoveType::EnPassant, {position_.file + 1, position_.rank - 1}});
			}
		}

		if (team_ == Team::Black && position_.rank == 4) // in forth from top row and playing white aka going up
		{
			if (previousMove.position.file == position_.file - 1 && previousMove.position.rank == position_.rank)
			{
				possibleMoves_.push_back({MoveType::EnPassant, {position_.file - 1, position_.rank + 1}});
			}

			if (previousMove.position.file == position_.file + 1 && previousMove.position.rank == position_.rank)
			{
				possibleMoves_.push_back({MoveType::EnPassant, {position_.file + 1, position_.rank + 1}});
			}
		}
	}
}
