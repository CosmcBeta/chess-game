#include "Window.hpp"

Window::Window() { setup("Window", sf::Vector2u(640, 480)); }
Window::Window(const std::string& title, const sf::Vector2u& size) { setup(title, size); }
Window::~Window() { destroy(); }

void Window::setup(const std::string title, const sf::Vector2u& size) 
{
	m_windowTitle = title;
	m_windowSize = size;
	m_isDone = false;
	m_window.setFramerateLimit(60);
	create();
}

void Window::create() 
{
	sf::VideoMode videoMode({m_windowSize.x, m_windowSize.y});
	m_window.create(sf::VideoMode({m_windowSize.x, m_windowSize.y}), m_windowTitle, sf::Style::Default);
}

void Window::destroy() 
{
	m_window.close();
}

void Window::beginDraw() { m_window.clear(sf::Color::Black); }
void Window::endDraw() { m_window.display(); }

bool Window::isDone() { return m_isDone; }
sf::RenderWindow* Window::getRenderWindow() { return &m_window; }

void Window::setIsDone(bool p_isDone)
{
	m_isDone = p_isDone;
}

void Window::draw(sf::Drawable& drawable) 
{
	m_window.draw(drawable);
}

sf::Vector2u Window::getWindowSize() { return m_windowSize; }

void Window::update() 
{
	while (const std::optional event = m_window.pollEvent())
	{
		//const sf::Event& event = *optEvent;
		if (event->is<sf::Event::Closed>())
		{
			m_isDone = true;
		}
	}
	// while (m_window.pollEvent(m_event))
	// {
	// 	if (m_event.type == sf::Event::Closed) { m_isDone = true; }
	// }
}
// while (auto event = window.pollEvent()) {
//     if (event->type == sf::Event::Closed)
//         window.close();
// }


std::optional<sf::Event> Window::getEvent()
{
	return m_window.pollEvent();
}

sf::Vector2i Window::getMousePos()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
	return mousePos;
}