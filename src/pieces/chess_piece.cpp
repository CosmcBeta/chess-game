#include "pieces/chess_piece.hpp"
#include "pieces/piece_info.hpp"

Piece::Piece(Team team, PieceType type, Position position):
team_(team), pieceType_(type), firstMove_(true), canEnPassant_(false), position_(position)
{}

Team Piece::getTeam() const
{
	return team_;
}

PieceType Piece::getPieceType() const
{
	return pieceType_;
}

Position Piece::getPosition() const
{
	return position_;
}

void Piece::setFirstMove(bool firstMove)
{
	firstMove_ = firstMove;
}

bool Piece::getFirstMove() const
{
	return firstMove_;
}

void Piece::setEnPassant(bool enPassant)
{
	canEnPassant_ = enPassant;
}

bool Piece::getEnPassant() const
{
	return canEnPassant_;
}

void Piece::setPosition(Position position)
{
	position_ = position;
}
