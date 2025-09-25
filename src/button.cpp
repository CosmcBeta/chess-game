#include "button.hpp"

#include <iostream>

Button::Button()
	:mouseInText(false), m_text(myriadRegular)
{
	if (!myriadRegular.openFromFile("assets/fonts/myriad_pro_regular.ttf"))
		std::cerr << "Failed to open font\n";
	m_text = sf::Text(myriadRegular);
}

Button::Button(const sf::String& p_string, FontType p_font, unsigned int p_characterSize, sf::Vector2f p_position)
	:m_position(p_position), mouseInText(false), m_text(myriadRegular),
	darkColor(0,0,0,255), lightColor(0,0,0,255)
{
	if (!myriadBold.openFromFile("assets/fonts/myriad_pro_bold.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadRegular.openFromFile("assets/fonts/myriad_pro_regular.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadSemibold.openFromFile("assets/fonts/myriad_pro_semibold.ttf"))
		std::cerr << "Failed to open font\n";

	m_text.setString(p_string);
	switch (p_font)
	{
	case FontType::Regular:
		m_text.setFont(myriadRegular);
		break;
	case FontType::Bold:
		m_text.setFont(myriadBold);
		break;
	case FontType::Semibold:
		m_text.setFont(myriadSemibold);
		break;
	default:
		break;
	}
	m_text.setCharacterSize(p_characterSize);
	m_text.setFillColor(darkColor);
	m_rect = m_text.getLocalBounds();
	m_text.setOrigin({m_rect.position.x + m_rect.size.x / 2.f, m_rect.position.y + m_rect.size.y / 2.f}); // Sets origin to the exact center of the text
	m_text.setPosition(m_position);
}

void Button::setColor(sf::Color p_darkColor, sf::Color p_lightColor)
{
	darkColor = p_darkColor;
	lightColor = p_lightColor;
}

void Button::update(sf::Vector2i p_mousePosition)
{
	mouseInText = (p_mousePosition.x > m_text.getPosition().x - m_rect.size.x / 2.f &&
		p_mousePosition.x < m_text.getPosition().x + m_rect.size.x / 2.f &&
		p_mousePosition.y > m_text.getPosition().y - m_rect.size.y / 2 &&
		p_mousePosition.y < m_text.getPosition().y + m_rect.size.y / 2);

	if (mouseInText)
		m_text.setFillColor(lightColor);
	else
		m_text.setFillColor(darkColor);
}

bool Button::getMouseInText() { return mouseInText; }

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_text, states);
}