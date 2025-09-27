#pragma once 

#include "chess-piece.hpp"

#include <SFML/Graphics.hpp>


class Queen : public Piece
{
public:
	Queen(Team team, sf::Vector2i position, sf::Texture& texture);

	virtual void calculateMoves(Board board, Move previousMove);
	bool addToVector(sf::Vector2i p_pos, Board p_field);

};