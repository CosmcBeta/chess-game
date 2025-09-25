#pragma once

#include <SFML/Graphics.hpp>

enum class ThemeSet
{
	BROWN,
	GREEN,
    BLUE
};

class Theme
{
public:
	Theme();

    void setTheme(ThemeSet p_theme);

	sf::Color darkMain;
	sf::Color lightMain;
	sf::Color alternate;

private:
	sf::Color darkBrown, lightBrown, altBrown;
	sf::Color darkGreen, lightGreen, altGreen;
    sf::Color darkBlue, lightBlue, altBlue;
};
