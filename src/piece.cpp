#include "piece.hpp"

Piece::Piece(Team team, PieceType type, sf::Vector2i position, sf::Texture& texture)
	:team_(team), pieceType_(type), texture_(texture), firstMove_(true),
	canEnPassant_(false), sprite_(texture)//, //_position(position)
{
	sprite_.setPosition({position.x * 80.f, position.y * 80.f});
	position_.x = (int)position.x;
	position_.y = (int)position.y;
	sprite_.setScale(sf::Vector2f(SCALE, SCALE));
}

Team Piece::getTeam() { return team_; }
PieceType Piece::getPieceType() { return pieceType_; }
sf::Vector2f Piece::getPosition() { return sprite_.getPosition(); }
sf::Vector2i Piece::getArrayPosition() { return position_; }
sf::Sprite Piece::getSprite() { return sprite_; }
void Piece::calculateMoves(Board board, Move previousMove) {}
std::vector<Move> Piece::getMoves() { return possibleMoves_; }
void Piece::setFirstMove(bool firstMove) { firstMove_ = firstMove; }
bool Piece::getFirstMove() { return firstMove_; }
void Piece::setEnPassant(bool enPassant) { canEnPassant_ = enPassant; }
bool Piece::getEnPassant() { return canEnPassant_; }

void Piece::setPosition(sf::Vector2f position) 
{ 
	sprite_.setPosition({position.x * 80.f, position.y * 80.f}); 
	position_.x = (int)position.x;
	position_.y = (int)position.y;
}
