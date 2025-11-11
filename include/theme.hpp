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
	sf::Color darkBrown_, lightBrown_, altBrown_;
	sf::Color darkGreen_, lightGreen_, altGreen_;
    sf::Color darkBlue_, lightBlue_, altBlue_;
};
