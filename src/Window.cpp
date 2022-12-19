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
	m_window.create({ m_windowSize.x, m_windowSize.y, 32 }, m_windowTitle, sf::Style::Default);
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
	while (m_window.pollEvent(m_event))
	{
		if (m_event.type == sf::Event::Closed) { m_isDone = true; }
	}
}

sf::Event& Window::getEvent()
{
	return m_event;
}

sf::Vector2i Window::getMousePos()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
	return mousePos;
}