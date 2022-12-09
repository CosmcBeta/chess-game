#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>

class ResourceManager
{
public:
    static ResourceManager& getInstance();

    void loadResources();

    bool addTexture(const std::string& id, const std::string& texturePath);
    std::shared_ptr<sf::Texture> getTexture(const std::string& id);

    bool addFont(const std::string& id, const std::string& fontPath);
    std::shared_ptr<sf::Font> getFont(const std::string& id);

private:
    ResourceManager();
    ~ResourceManager();

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textureMap;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fontMap;
};