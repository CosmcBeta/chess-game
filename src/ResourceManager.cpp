#include "ResourceManager.hpp"

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager()
{}

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

void ResourceManager::loadResources()
{
    addTexture("whitePawn", "assets/white_pawn.png");
    addTexture("whiteRook", "assets/white_rook.png");
    addTexture("whiteBishop", "assets/white_bishop.png");
    addTexture("whiteKnight", "assets/white_knight.png");
    addTexture("whiteQueen", "assets/white_queen.png");
    addTexture("whiteKing", "assets/white_king.png");
    addTexture("blackPawn", "assets/black_pawn.png");
    addTexture("blackRook", "assets/black_rook.png");
    addTexture("blackBishop", "assets/black_bishop.png");
    addTexture("blackKnight", "assets/black_knight.png");
    addTexture("blackQueen", "assets/black_queen.png");
    addTexture("blackKing", "assets/black_king.png");
}

bool ResourceManager::addTexture(const std::string& id, const std::string& texturePath)
{
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end())
    {
        std::cout << "Unable to add texture: " << id << " already exists\n";
        return false;
    }

    std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();

    if (!texture->loadFromFile(texturePath))
    {
        std::cout << "Unable to open texture: " << texturePath << "\n";
        return false;
    }

    m_textureMap.emplace(id, std::move(texture));
    return true;
}

std::shared_ptr<sf::Texture> ResourceManager::getTexture(const std::string& id)
{
    auto it = m_textureMap.find(id);
    if (it == m_textureMap.end())
    {
        std::cout << "Unable to load texture: " << id << " doesn't exist\n";
        return nullptr;
    }

    return std::move(it->second);
}

