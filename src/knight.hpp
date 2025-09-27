#pragma once 

#include "chess-piece.hpp"

#include <SFML/Graphics.hpp>


class Knight : public Piece
{
public:
	Knight(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
};