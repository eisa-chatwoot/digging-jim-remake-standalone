#include "Image/Manager.h"
#include "Utils/Paths.h"
#include <iostream>

Image::Manager::Manager() {};

void Image::Manager::loadAllImages() {
    // Load textures
    loadTexture(Image::Texture::CaveHUD, Paths::assetPath("textures/Cave/hud.png").string());
    loadTexture(Image::Texture::CaveLoadingTiles, Paths::assetPath("textures/Cave/loading_tiles.png").string());
    loadTexture(Image::Texture::CaveNumbers, Paths::assetPath("textures/Cave/numbers.png").string());
    loadTexture(Image::Texture::CaveTiles, Paths::assetPath("textures/Cave/tiles.png").string());
    loadTexture(Image::Texture::GameCompleted, Paths::assetPath("textures/Game/completed.png").string());
    loadTexture(Image::Texture::GameFont, Paths::assetPath("textures/Game/font.png").string());
    loadTexture(Image::Texture::GameOver, Paths::assetPath("textures/Game/over.png").string());
    loadTexture(Image::Texture::MainMenuBelow, Paths::assetPath("textures/MainMenu/below.png").string());
    loadTexture(Image::Texture::MainMenuDial, Paths::assetPath("textures/MainMenu/dial.png").string());
    loadTexture(Image::Texture::MainMenuLoadCaves, Paths::assetPath("textures/MainMenu/load_caves.png").string());
    loadTexture(Image::Texture::MainMenuNumbers, Paths::assetPath("textures/MainMenu/numbers.png").string());
    loadTexture(Image::Texture::MainMenuOnOff, Paths::assetPath("textures/MainMenu/on_off.png").string());
    loadTexture(Image::Texture::MainMenuOptions, Paths::assetPath("textures/MainMenu/options.png").string());
    loadTexture(Image::Texture::MainMenuSelectArrow, Paths::assetPath("textures/MainMenu/select_arrow.png").string());
    loadTexture(Image::Texture::MainMenuTop, Paths::assetPath("textures/MainMenu/top.png").string());
    loadTexture(Image::Texture::EditorFill, Paths::assetPath("textures/Editor/fill.png").string());
    loadTexture(Image::Texture::EditorLineFill, Paths::assetPath("textures/Editor/linefill.png").string());
    loadTexture(Image::Texture::EditorEllipseFill, Paths::assetPath("textures/Editor/ellipsefill.png").string());
    loadTexture(Image::Texture::EditorCoords, Paths::assetPath("textures/Editor/coords.png").string());
    loadTexture(Image::Texture::EditorTestButton, Paths::assetPath("textures/Editor/test_button.png").string());
    loadTexture(Image::Texture::EditorMiniTiles, Paths::assetPath("textures/Editor/mini_tiles.png").string());
}

void Image::Manager::loadIcon(const Image::Icon& icon, const std::string& filename) {
    sf::Image image;
    if (!image.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load icon: " + filename + "\n");
    }
    m_icons[icon] = std::move(image);
}

void Image::Manager::loadTexture(const Image::Texture& texture, const std::string& filename) {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load texture: " + filename + "\n");
    }
    m_textures[texture] = std::move(tex);
}

const sf::Image& Image::Manager::getIcon(const Image::Icon& icon) const {
    return m_icons.at(icon);
}

const sf::Texture& Image::Manager::getTexture(const Image::Texture& texture) const {
    return m_textures.at(texture);
}
