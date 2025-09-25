#pragma once

#include <SFML/Graphics.hpp>

enum class ThemeSet
{
	Brown,
	Green,
    Blue
};

class Theme
{
public:
	Theme();

    void setTheme(ThemeSet p_theme);

	sf::Color darkMain;
	sf::Color lightMain;
	sf::Color alternate;
    sf::Color moveCircle;

private:
	sf::Color darkBrown, lightBrown, altBrown;
	sf::Color darkGreen, lightGreen, altGreen;
    sf::Color darkBlue, lightBlue, altBlue;
};
