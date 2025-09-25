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
	Button(const sf::String& p_string, FontType p_font, unsigned int p_characterSize, sf::Vector2f p_position);

	void update(sf::Vector2i p_mousePosition);
	bool getMouseInText();
	void setColor(sf::Color p_darkColor, sf::Color p_lightColor);

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2f m_position;
	sf::Text m_text;
	sf::FloatRect m_rect;
	bool mouseInText;

	sf::Color darkColor, lightColor;

	sf::Font myriadBold, myriadRegular, myriadSemibold;
};