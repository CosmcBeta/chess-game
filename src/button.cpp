#include "button.hpp"

#include <iostream>

Button::Button()
	:mouseInText_(false), text_(myriadRegular_)
{
	if (!myriadRegular_.openFromFile("assets/fonts/myriad_pro_regular.ttf"))
		std::cerr << "Failed to open font\n";
	text_ = sf::Text(myriadRegular_);
}

Button::Button(const sf::String& string, FontType font, unsigned int characterSize, sf::Vector2f position)
	:mouseInText_(false), text_(myriadRegular_), darkColor_(0,0,0,255), lightColor_(0,0,0,255)
{
	if (!myriadBold_.openFromFile("assets/fonts/myriad_pro_bold.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadRegular_.openFromFile("assets/fonts/myriad_pro_regular.ttf"))
		std::cerr << "Failed to open font\n";
	if (!myriadSemibold_.openFromFile("assets/fonts/myriad_pro_semibold.ttf"))
		std::cerr << "Failed to open font\n";

	text_.setString(string);
	switch (font)
	{
	case FontType::Regular:
		text_.setFont(myriadRegular_);
		break;
	case FontType::Bold:
		text_.setFont(myriadBold_);
		break;
	case FontType::Semibold:
		text_.setFont(myriadSemibold_);
		break;
	default:
		break;
	}
	text_.setCharacterSize(characterSize);
	text_.setFillColor(darkColor_);
	rect_ = text_.getLocalBounds();
	text_.setOrigin({rect_.position.x + rect_.size.x / 2.f, rect_.position.y + rect_.size.y / 2.f}); // Sets origin to the exact center of the text
	text_.setPosition(position);
}

void Button::setColor(sf::Color darkColor, sf::Color lightColor)
{
	darkColor_ = darkColor;
	lightColor_ = lightColor;
}

void Button::update(sf::Vector2i mousePosition)
{
	mouseInText_ = (mousePosition.x > text_.getPosition().x - rect_.size.x / 2.f &&
		mousePosition.x < text_.getPosition().x + rect_.size.x / 2.f &&
		mousePosition.y > text_.getPosition().y - rect_.size.y / 2 &&
		mousePosition.y < text_.getPosition().y + rect_.size.y / 2);

	if (mouseInText_)
		text_.setFillColor(lightColor_);
	else
		text_.setFillColor(darkColor_);
}

bool Button::getMouseInText() { return mouseInText_; }

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(text_, states);
}