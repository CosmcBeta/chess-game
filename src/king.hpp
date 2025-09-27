#pragma once 

#include "chess-piece.hpp"

#include <SFML/Graphics.hpp>

class King : public Piece
{
public:
	King(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
};