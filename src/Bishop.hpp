#pragma once 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

#include "Piece.hpp"

class Bishop : public Piece
{
public:
	Bishop(Team t, sf::Vector2f p_pos, sf::Texture p_texture);

	virtual void calcMoves(Piece* p_field[8][8]);
	bool addToVector(sf::Vector2i p_pos, Piece* p_field[8][8]);
};