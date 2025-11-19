#pragma once

#include "pieces/piece_info.hpp"

#include <vector>

enum class GameOutcome
{
	WhiteWins,
	BlackWins,
	Stalemate
};

class ChessBoard
{
public:
    ChessBoard();
    ~ChessBoard();

	int getNumberOfPieces() const;
	Position getKingPosition(Team kingTeam, bool currentBoard) const;

    void createPieces();
	bool isInCheck(Position kingPosition, Team kingTeam);

	bool willBeInCheck(Position oldPosition, Position newPosition, Team team);
	void createPotentialBoard(Position oldPosition, Position newPosition, Team pieceTeam);

	void displayMoves();
	void removeInvalidMoves(Team kingTeam, Position oldPosition);
	void removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves);
	int getTotalMoveCount(Team team);

private:
    bool whiteKingInCheck_, blackKingInCheck_;
    std::vector<Move> possibleMoves_;
    Board board_;
	Board potentialBoard_;
	Position blackKingPosition_, whiteKingPosition_;
	Team playerTurn_;
	GameOutcome gameOutcome_;

	Move previousMove_;
};
