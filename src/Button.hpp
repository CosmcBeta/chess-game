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

enum class Style
{
	TEXT,
	REGULAR,
	BOX
};

class Button : public sf::Drawable
{
public:
	Button();
	Button(sf::String p_string, FontType p_font, unsigned int p_characterSize, sf::Vector2f p_position, Style p_style);

	~Button();

	void update(sf::Vector2i p_mousePosition);
	bool getMouseInText();

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::String m_string;
	sf::Vector2f m_position;
	sf::Text m_text;
	sf::FloatRect m_rect;
	sf::RectangleShape m_box;
	bool mouseInText;
	Style m_style;

	sf::Color m_normalColor;
	sf::Color m_hoveredColor;

	sf::Font myriadBold, myriadRegular, myriadSemibold;
};

//titleText.setString(sf::String("Chess"));
//titleText.setFont(myriadBold);
//titleText.setCharacterSize(185);
//titleText.setFillColor(darkBack);
//titleRect = titleText.getLocalBounds();
//titleText.setOrigin(titleRect.left + titleRect.width / 2.f, titleRect.top + titleRect.height / 2.f);
//titleText.setPosition(sf::Vector2f(320.f, 160.f));