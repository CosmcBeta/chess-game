#pragma once 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <array>

#include "piece.hpp"

class King : public Piece
{
public:
	King(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
};