#include "Sound/Manager.h"
#include "Utils/Paths.h"
#include <algorithm>
#include <iostream>

Sound::Manager::Manager() {}

void Sound::Manager::loadAllSounds() {
    // Load sound effects
    loadSound(Sound::Effect::Amoeba, Paths::assetPath("sounds/Effect/amoeba.wav").string());
    loadSound(Sound::Effect::Break, Paths::assetPath("sounds/Effect/break.wav").string());
    loadSound(Sound::Effect::CaveGullExplosion, Paths::assetPath("sounds/Effect/cave_gull_explosion.wav").string());
    loadSound(Sound::Effect::Collect, Paths::assetPath("sounds/Effect/collect.wav").string());
    loadSound(Sound::Effect::DiamondDrop, Paths::assetPath("sounds/Effect/diamond_drop.wav").string());
    loadSound(Sound::Effect::DiamondLand, Paths::assetPath("sounds/Effect/diamond_land.wav").string());
    loadSound(Sound::Effect::Dig, Paths::assetPath("sounds/Effect/dig.wav").string());
    loadSound(Sound::Effect::Drop, Paths::assetPath("sounds/Effect/drop.wav").string());
    loadSound(Sound::Effect::Explosion, Paths::assetPath("sounds/Effect/explosion.wav").string());
    loadSound(Sound::Effect::Land, Paths::assetPath("sounds/Effect/land.wav").string());
    loadSound(Sound::Effect::MagicWall, Paths::assetPath("sounds/Effect/magic_wall.wav").string());
    loadSound(Sound::Effect::Open, Paths::assetPath("sounds/Effect/open.wav").string());
    loadSound(Sound::Effect::Plasma, Paths::assetPath("sounds/Effect/plasma.wav").string());
    loadSound(Sound::Effect::Tube, Paths::assetPath("sounds/Effect/tube.wav").string());
    loadSound(Sound::Effect::Unlock, Paths::assetPath("sounds/Effect/unlock.wav").string());
    loadSound(Sound::Effect::Yahoo, Paths::assetPath("sounds/Effect/yahoo.wav").string());
    loadSound(Sound::Effect::Yippee, Paths::assetPath("sounds/Effect/yippee.wav").string());

    // Load music
    loadMusic(Sound::Music::MainMenu, Paths::assetPath("sounds/Music/main_menu.wav").string());
}

void Sound::Manager::loadSound(const Sound::Effect& effect, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load sound: " + filename + "\n");
    }

    m_buffers[effect] = buffer;

    auto sound = std::make_unique<sf::Sound>(buffer);
    sound->setBuffer(m_buffers[effect]);

    m_sounds[effect] = std::move(sound);
}

void Sound::Manager::loadMusic(const Sound::Music& music, const std::string& filename) {
    auto track = std::make_unique<sf::Music>();
    if (!track->openFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load music: " + filename + "\n");
    }

    m_music[music] = std::move(track);
}

void Sound::Manager::play(const Sound::Effect& effect) {
    auto bufIt = m_buffers.find(effect);
    if (bufIt == m_buffers.end()) return;

    auto sound = std::make_unique<sf::Sound>(bufIt->second);
    sound->setLooping(false);
    sound->setVolume(m_volume/2);
    sound->play();

    m_activeSounds.push_back(std::move(sound));
}

void Sound::Manager::loop(const Sound::Effect& effect) {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        it->second->setLooping(true);
        it->second->setVolume(m_volume/2);
        it->second->play();
    }
}

void Sound::Manager::stop(const Sound::Effect& effect) {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        it->second->stop();
    }
}

bool Sound::Manager::isPlaying(const Sound::Effect& effect) const {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        if (it->second->getStatus() == sf::Sound::Status::Playing)
            return true;
    }
    return false;
}

void Sound::Manager::playMusic(const Sound::Music& music) {
    auto it = m_music.find(music);
    if (it == m_music.end()) {
        return;
    }

    // Stop currently playing track, if any
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->stop();
    }

    m_currentMusic = it->second.get();
    m_currentMusic->play();
}

void Sound::Manager::stopMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->stop();
    }
    m_currentMusic = nullptr;
}

void Sound::Manager::setVolume(const float& volume) {
    m_volume = volume;
}

void Sound::Manager::update() {
    m_activeSounds.erase(
        std::remove_if(
            m_activeSounds.begin(),
            m_activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& s) {
        return s->getStatus() == sf::Sound::Status::Stopped;
    }),
        m_activeSounds.end()
    );
}
