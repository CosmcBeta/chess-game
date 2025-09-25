#pragma once 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

#include "piece.hpp"

class Pawn : public Piece
{
public:
	Pawn(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
};