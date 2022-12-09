#include "Button.hpp"

Button::Button()
	:mouseInText(false)
{}

Button::~Button()
{}

Button::Button(sf::String p_string, FontType p_font, unsigned int p_characterSize, sf::Vector2f p_position)
	:m_string(p_string), m_position(p_position), mouseInText(false),
	m_normalColor(181, 136, 95, 255), m_hoveredColor(143, 107, 74, 255)
{
	myriadBold.loadFromFile("assets/myriad_pro_bold.ttf");
	myriadRegular.loadFromFile("assets/myriad_pro_regular.ttf");
	myriadSemibold.loadFromFile("assets/myriad_pro_semibold.ttf");

	m_text.setString(m_string);
	switch (p_font)
	{
	case FontType::REGULAR:
		m_text.setFont(myriadRegular);
		break;
	case FontType::BOLD:
		m_text.setFont(myriadBold);
		break;
	case FontType::SEMIBOLD:
		m_text.setFont(myriadSemibold);
		break;
	default:
		break;
	}
	m_text.setCharacterSize(p_characterSize);
	m_text.setFillColor(m_normalColor);
	m_rect = m_text.getLocalBounds();
	m_text.setOrigin(m_rect.left + m_rect.width / 2.f, m_rect.top + m_rect.height / 2.f); // Sets origin to the exact center of the text
	m_text.setPosition(m_position);

}

void Button::update(sf::Vector2i p_mousePosition)
{
	mouseInText = (p_mousePosition.x > m_text.getPosition().x - m_rect.width / 2.f &&
		p_mousePosition.x < m_text.getPosition().x + m_rect.width / 2.f &&
		p_mousePosition.y > m_text.getPosition().y - m_rect.height / 2 &&
		p_mousePosition.y < m_text.getPosition().y + m_rect.height / 2);

	if (mouseInText)
		m_text.setFillColor(m_hoveredColor);
	else
		m_text.setFillColor(m_normalColor);
}

bool Button::getMouseInText() { return mouseInText; }

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_text, states);
}