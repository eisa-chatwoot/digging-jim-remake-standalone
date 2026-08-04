#include "Cave/Manager/Manager.h"
#include "Cave/Manager/File.h"
#include "Cave/Properties/Properties.h"
#include "Utils/Paths.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <array>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

Cave::Manager::Manager(Game* game) : m_game(game) {}

void Cave::Manager::load() {
    const auto bundledCavesDirectory = Paths::assetPath("data");
    const auto userCavesDirectory = Paths::userCavesDirectory();

    if (!std::filesystem::exists(bundledCavesDirectory) || !std::filesystem::is_directory(bundledCavesDirectory)) {
        throw std::runtime_error("Invalid directory: " + bundledCavesDirectory.string());
    }

    // Collect bundled cave files — originals.cav always goes first
    std::vector<std::string> bundledFiles;
    std::string originalsFile;
    for (const auto& entry : std::filesystem::directory_iterator(bundledCavesDirectory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cav") {
            std::string filename = entry.path().filename().string();
            if (filename == ORIGINAL_CAVE_FILE)
                originalsFile = filename;
            else
                bundledFiles.push_back(filename);
        }
    }
    if (!originalsFile.empty())
        bundledFiles.insert(bundledFiles.begin(), originalsFile);

    for (auto& filename : bundledFiles) {
        try {
            m_cavesData.push_back(Cave::File::loadFromFile(bundledCavesDirectory, filename));
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Failed to load cave file '" << filename << "': " << e.what() << "\n";
        }
    }

    if (m_cavesData.empty()) {
        throw std::runtime_error("No valid .cav files found in directory: " + bundledCavesDirectory.string());
    }

    const auto loadUserCaves = [this](const std::filesystem::path& directory) {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) return;

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cav") {
                std::string filename = entry.path().filename().string();
                try {
                    m_cavesData.push_back(Cave::File::loadFromFile(directory, filename));
                }
                catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to load user cave file '" << filename << "': " << e.what() << "\n";
                }
            }
        }
    };

    // Collect user cave files from the per-user data directory (optional — don't throw if missing).
    loadUserCaves(userCavesDirectory);

    // Keep caves from the pre-bundle layout working for existing users and development builds.
    const auto legacyCavesDirectory = Paths::executableDirectory() / "caves";
    if (legacyCavesDirectory != userCavesDirectory)
        loadUserCaves(legacyCavesDirectory);
}

std::vector<std::string> Cave::Manager::getCaveFiles() {
    std::vector<std::string> caveFiles;
    for (auto& caveData : m_cavesData) caveFiles.push_back(caveData.filename);
    return caveFiles;
}

bool Cave::Manager::startCave(const int& fileIndex, const int& caveNumber, Cave::Properties* properties, Cave::Map* map) {
    if (m_cavesData.empty() || map == nullptr || properties == nullptr) return false;

    // Clamp cave index to get cave file
    int index = std::clamp(fileIndex, 0, static_cast<int>(m_cavesData.size()  - 1));
    Cave::File file = m_cavesData[index];

    // Clamp cave number to get cave data
    // We subtract 1 from the game cave number, as that is indexed from 1, not 0 like file.caves
    int caveIndex = std::clamp(caveNumber - 1, 0, static_cast<int>(file.caves.size()) - 1);
    Cave::Data data = file.caves[caveIndex];

    // Set the cave properties
    *properties = data.properties;
    m_game->setCaveProperties(*properties);

    // Construct the map
    map->generateMap(properties, data.tileData);

    return true;
}

size_t Cave::Manager::numCaves(const int& fileIndex) {
    if (m_cavesData.empty()) return 0;

    int index = std::clamp(fileIndex, 0, static_cast<int>(m_cavesData.size() - 1));
    Cave::File file = m_cavesData[index];

    return file.caves.size();
}
