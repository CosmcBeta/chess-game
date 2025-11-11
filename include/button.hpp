#pragma once

#include <SFML/Graphics.hpp>

enum class ButtonState
{
	Normal,
	Hovered
};

enum class FontType
{
	Regular,
	Bold,
	Semibold
};

class Button : public sf::Drawable
{
public:
	Button();
	Button(const sf::String& string, FontType font, unsigned int characterSize, sf::Vector2f position);

	void update(sf::Vector2i mousePosition);
	bool getMouseInText();
	void setColor(sf::Color darkColor, sf::Color lightColor);

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Text text_;
	sf::FloatRect rect_;
	bool mouseInText_;

	sf::Color darkColor_, lightColor_;

	sf::Font myriadBold_, myriadRegular_, myriadSemibold_;
};