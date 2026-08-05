#pragma once

#include <filesystem>
#include <string_view>

/// Paths used by the application at runtime.
///
/// Assets are read from the application bundle when one is present, while
/// settings and user-created caves are always kept outside the bundle.
namespace Paths {

/// Return the full path of the running executable when it can be determined.
[[nodiscard]] std::filesystem::path executablePath();

/// Return the directory containing the running executable.
[[nodiscard]] std::filesystem::path executableDirectory();

/// Return the root directory containing the bundled assets.
[[nodiscard]] std::filesystem::path assetsDirectory();

/// Resolve a path relative to the bundled assets directory.
[[nodiscard]] std::filesystem::path assetPath(std::string_view relativePath);

/// Return the platform-appropriate per-user application data directory.
[[nodiscard]] std::filesystem::path userDataDirectory();

/// Return the per-user settings file location.
[[nodiscard]] std::filesystem::path settingsFile();

/// Return the per-user completed-cave progress file location.
[[nodiscard]] std::filesystem::path progressFile();

/// Return the directory that contains user-created cave files.
[[nodiscard]] std::filesystem::path userCavesDirectory();

/// Create the per-user application data directory if necessary.
bool ensureUserDataDirectory();

/// Create the per-user caves directory if necessary.
bool ensureUserCavesDirectory();

} // namespace Paths
