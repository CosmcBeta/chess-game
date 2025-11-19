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

bool ChessBoard::isInCheck(Position kingPosition, Team kingTeam) const
{
    std::vector<Position> allMoves {};
	for (const auto& rank : board_)
	{
		for (const auto& piece : rank)
		{
			if (!piece || piece->getTeam() == kingTeam)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, previousMove_)};

			for (const Move& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	for (const Position& move : allMoves)
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
	return {0, 0}; // should fail if king not found (work on this)
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

	std::vector<Position> allMoves {};
	for (const auto& rank : potentialBoard_)
	{
		for (const auto& piece : rank)
		{
			if (!piece || piece->getTeam() == team)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(potentialBoard_, previousMove_)};

			for (const Move& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}

	Position kingPos {getKingPosition(team, false)};

	for (const Position& move : allMoves)
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
		    if (board_[file][rank])
		    {
				potentialBoard_[file][rank] = board_[file][rank]->clone();
			}
			else
			{
                potentialBoard_[file][rank] = nullptr;
			}
		}
	}

	// Moves the piece to where it would be
	potentialBoard_[newPosition.file][newPosition.rank] = std::move(potentialBoard_[oldPosition.file][oldPosition.rank]);
}

void ChessBoard::removeInvalidMoves(Team kingTeam, Position oldPosition)
{
    auto iterator {possibleMoves_.begin()};
	int i {0};
	while (iterator != possibleMoves_.end())
	{
		if (willBeInCheck(oldPosition, possibleMoves_.at(i).position, kingTeam))
		{
			iterator = possibleMoves_.erase(iterator);
		}
		else
		{
			iterator++;
			i++;
		}
	}
}

void ChessBoard::removeInvalidMoves(Team kingTeam, Position oldPosition, std::vector<Move>& moves)
{
    auto iterator {moves.begin()};
	int i {0};
	while (iterator != moves.end())
	{
		if (willBeInCheck(oldPosition, moves.at(i).position, kingTeam))
		{
			iterator = moves.erase(iterator);
		}
		else
		{
			iterator++;
			i++;
		}
	}
}

int ChessBoard::getTotalMoveCount(Team team)
{
    std::vector<Position> allMoves {};
	for (const auto& rank : board_)
	{
		for (const auto& piece : rank)
		{
			if (!piece || piece->getTeam() != team)
			{
				continue;
			}

			std::vector<Move> pieceMoves {piece->calculateMoves(board_, previousMove_)};

			removeInvalidMoves(team, piece->getPosition(), pieceMoves);

			for (const Move& move : pieceMoves)
			{
				allMoves.push_back(move.position);
			}
		}
	}
	return static_cast<int>(allMoves.size());
}
