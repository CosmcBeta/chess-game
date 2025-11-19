#include "board.hpp"
#include "pieces/king.hpp"
#include "pieces/knight.hpp"
#include "pieces/pawn.hpp"
#include "pieces/piece_info.hpp"
#include "pieces/sliding_piece.hpp"

ChessBoard::ChessBoard()
:previousMove_{MoveType::None, {0,0}},
blackKingInCheck_(false), whiteKingInCheck_(false)
{
    createPieces();
}
ChessBoard::~ChessBoard()
{

}

void ChessBoard::createPieces()
{
	// Add white pieces
	board_[4][7] = std::make_unique<King>(Team::White, Position{4, 7});
	board_[3][7] = std::make_unique<SlidingPiece>(Team::White, Position{3, 7}, PieceType::Queen);
	board_[2][7] = std::make_unique<SlidingPiece>(Team::White, Position{2, 7}, PieceType::Bishop);
	board_[5][7] = std::make_unique<SlidingPiece>(Team::White, Position{5, 7}, PieceType::Bishop);
	board_[1][7] = std::make_unique<Knight>(Team::White, Position{1, 7});
	board_[6][7] = std::make_unique<Knight>(Team::White, Position{6, 7});
	board_[0][7] = std::make_unique<SlidingPiece>(Team::White, Position{0, 7}, PieceType::Rook);
	board_[7][7] = std::make_unique<SlidingPiece>(Team::White, Position{7, 7}, PieceType::Rook);
	for (int i = 0; i < 8; i++)
	{
		board_[i][6] = std::make_unique<Pawn>(Team::White, Position{i, 6});
	}

	// Add black pieces
	board_[4][0] = std::make_unique<King>(Team::Black, Position{4, 0});
	board_[3][0] = std::make_unique<SlidingPiece>(Team::Black, Position{3, 0}, PieceType::Queen);
	board_[2][0] = std::make_unique<SlidingPiece>(Team::Black, Position{2, 0}, PieceType::Bishop);
	board_[5][0] = std::make_unique<SlidingPiece>(Team::Black, Position{5, 0}, PieceType::Bishop);
	board_[1][0] = std::make_unique<Knight>(Team::Black, Position{1, 0});
	board_[6][0] = std::make_unique<Knight>(Team::Black, Position{6, 0});
	board_[0][0] = std::make_unique<SlidingPiece>(Team::Black, Position{0, 0}, PieceType::Rook);
	board_[7][0] = std::make_unique<SlidingPiece>(Team::Black, Position{7, 0}, PieceType::Rook);
	for (int i = 0; i < 8; i++)
	{
		board_[i][1] = std::make_unique<Pawn>(Team::Black, Position{i, 1});
	}

	// Add black spaces
	for (int i = 2; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			board_[j][i] = nullptr;
		}
	}
}

bool ChessBoard::isInCheck(Position kingPosition, Team kingTeam)
{
    std::vector<Position> allMoves;
	for (auto& row : board_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() == kingTeam)
			{
				continue;
			}

			piece->calculateMoves(board_, previousMove_);
			std::vector<Move> pieceMoves = piece->getMoves();

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	for (auto& move : allMoves)
	{
		if (kingPosition == move)
		{
			return true;
		}
	}

	return false;
}

Position ChessBoard::getKingPosition(Team kingTeam, bool currentBoard) const
{
    const Board& board = currentBoard ? board_ : potentialBoard_;

	for (const auto& rank : board)
	{
		for (const auto& piece : rank)
		{
			if (piece && piece->getPieceType() == PieceType::King && piece->getTeam() == kingTeam)
			{
				return piece->getPosition();
			}
		}
	}
	return {0, 0}; // should fail if king not found
}

int ChessBoard::getNumberOfPieces() const
{
    int count {0};
	for (const auto& rank : board_)
	{
		for (const auto& piece : rank)
		{
			if (piece)
			{
				count++;
			}
		}
	}
	return count;
}

bool ChessBoard::willBeInCheck(Position oldPosition, Position newPosition, Team team)
{
    createPotentialBoard(oldPosition, newPosition, team);

	std::vector<Position> allMoves;
	for (auto& row : potentialBoard_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() == team)
			{
				continue;
			}

			piece->calculateMoves(potentialBoard_, previousMove_);
			std::vector<Move> pieceMoves = piece->getMoves();

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}

	Position kingPos = getKing(team, false);

	for (auto& move : allMoves)
	{
		if (kingPos == move)
		{
			return true;
		}
	}

	return false;
}

void ChessBoard::createPotentialBoard(Position oldPosition, Position newPosition, Team pieceTeam)
{
    // Resets the board
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			potentialBoard_[file][rank] = std::move(board_[file][rank]);
		}
	}

	// Moves the piece to where it would be
	potentialBoard_[newPosition.file][newPosition.rank] = std::move(potentialBoard_[oldPosition.file][oldPosition.rank]);
}

void ChessBoard::removeInvalidMoves(Team kingTeam, Position oldPosition)
{
    auto iter = possibleMoves_.begin();
	int i = 0;
	while (iter != possibleMoves_.end())
	{
		Position temp {possibleMoves_.at(i).position.file, possibleMoves_.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iter = possibleMoves_.erase(iter);
		}
		else
		{
			++iter;
			++i;
		}
	}
}

void ChessBoard::removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves)
{
    auto iter = moves.begin();
	int i = 0;
	while (iter != moves.end())
	{
		Position temp {moves.at(i).position.file, moves.at(i).position.rank};
		if (willBeInCheck(oldPosition, temp, kingTeam))
		{
			iter = moves.erase(iter);
		}
		else
		{
			++iter;
			++i;
		}
	}
}

int ChessBoard::getTotalMoveCount(Team team)
{
    std::vector<Position> allMoves;
	for (auto& row : board_)
	{
		for (auto& piece : row)
		{
			if (piece == nullptr || piece->getTeam() != team)
			{
				continue;
			}

			piece->calculateMoves(board_, previousMove_);
			std::vector<Move> pieceMoves = piece->getMoves();

			removeInvalidMoves(team, piece->getPosition(), pieceMoves);

			for (auto& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	return static_cast<int>(allMoves.size());
}
