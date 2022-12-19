#pragma once

#include <SFML/Graphics.hpp>

enum class ButtonState
{
	NORMAL,
	HOVERED
};

enum class FontType
{
	REGULAR,
	BOLD,
	SEMIBOLD
};

class Button : public sf::Drawable
{
public:
	Button();
	Button(sf::String p_string, FontType p_font, unsigned int p_characterSize, sf::Vector2f p_position);

	~Button();

	void update(sf::Vector2i p_mousePosition);
	bool getMouseInText();

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::String m_string;
	sf::Vector2f m_position;
	sf::Text m_text;
	sf::FloatRect m_rect;
	bool mouseInText;

	sf::Color m_normalColor;
	sf::Color m_hoveredColor;

	sf::Font myriadBold, myriadRegular, myriadSemibold;
};