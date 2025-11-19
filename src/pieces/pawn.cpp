#include "pieces/pawn.hpp"
#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

#include <vector>
#include <memory>

Pawn::Pawn(Team team, Position position)
	:Piece(team, PieceType::Pawn, position)
{}

std::vector<Move> Pawn::calculateMoves(const Board& board, Move previousMove) const
{
	std::vector<Move> possibleMoves {};

	if (position_.rank == FIRST || position_.rank == LAST)
	{
	    return possibleMoves;
	}

	int direction = (team_ == Team::White) ? -1 : 1;
	int enPassantRank = (team_ == Team::White) ? 3 : 4;


    if (board[position_.file][position_.rank + direction] == nullptr)
    {
        possibleMoves.push_back({MoveType::Normal, {position_.file, position_.rank + direction}});
    }

    if (firstMove_ && board[position_.file][position_.rank + direction * 2] == nullptr)
    {
        possibleMoves.push_back({MoveType::PawnDouble, {position_.file, position_.rank + direction * 2}});
    }

    for (int offset : {-1, 1})
    {
        int adjacentFile {position_.file + offset};
        if (FIRST <= adjacentFile && adjacentFile <= LAST && board[adjacentFile][position_.rank + direction] != nullptr \
            && board[adjacentFile][position_.rank + direction]->getTeam() != team_)
        {
            possibleMoves.push_back({MoveType::Capture, {adjacentFile, position_.rank + direction}});
        }
    }

    if (previousMove.moveType == MoveType::PawnDouble && position_.rank == enPassantRank)
    {
        for (int offset : {-1, 1})
        {
            int adjacentFile {position_.file + offset};
            if (previousMove.position.file == adjacentFile && previousMove.position.rank == position_.rank)
            {
                possibleMoves.push_back({MoveType::EnPassant, {adjacentFile, position_.rank + direction}});
            }
        }
    }

    return possibleMoves;
}

std::unique_ptr<Piece> Pawn::clone() const
{
    return std::make_unique<Pawn>(*this);
}
