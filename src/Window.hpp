#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Window
{
public:
	Window();
	Window(const std::string& title, const sf::Vector2u& size);
	~Window();

	void beginDraw();
	void endDraw();

	void update();
	sf::Vector2i getMousePos();

	sf::Event& getEvent();
	sf::RenderWindow* getRenderWindow();

	bool isDone();
	sf::Vector2u getWindowSize();
	void setIsDone(bool p_isDone);

	void draw(sf::Drawable& p_drawable);
	sf::Event m_event;
private:
	void setup(const std::string title, const sf::Vector2u& size);
	void create();
	void destroy();

	sf::RenderWindow m_window;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isDone;
	bool m_isFullscreen;
};