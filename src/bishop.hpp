#pragma once 

#include "piece.hpp"

#include <SFML/Graphics.hpp>

class Bishop : public Piece
{
public:
	Bishop(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
	bool addToVector(sf::Vector2i p_pos, Board p_field);
};