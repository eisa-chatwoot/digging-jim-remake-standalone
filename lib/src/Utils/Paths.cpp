#include "Utils/Paths.h"

#include <array>
#include <cstdlib>
#include <string>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <limits.h>
#include <mach-o/dyld.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace {

std::filesystem::path normalisePath(const std::filesystem::path& path)
{
    std::error_code error;
    const auto canonical = std::filesystem::weakly_canonical(path, error);
    return error ? path : canonical;
}

bool pathExists(const std::filesystem::path& path)
{
    std::error_code error;
    return std::filesystem::exists(path, error);
}

bool ensureDirectory(const std::filesystem::path& path)
{
    std::error_code error;
    std::filesystem::create_directories(path, error);
    return !error;
}

} // namespace

std::filesystem::path Paths::executablePath()
{
#ifdef _WIN32
    std::array<wchar_t, 32768> buffer{};
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length > 0 && length < buffer.size())
        return normalisePath(std::filesystem::path(std::wstring(buffer.data(), length)));
#elif defined(__APPLE__)
    std::array<char, PATH_MAX> buffer{};
    uint32_t size = static_cast<uint32_t>(buffer.size());
    if (_NSGetExecutablePath(buffer.data(), &size) == 0)
        return normalisePath(std::filesystem::path(buffer.data()));
#else
    std::array<char, PATH_MAX> buffer{};
    const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length > 0)
    {
        buffer[static_cast<size_t>(length)] = '\0';
        return normalisePath(std::filesystem::path(buffer.data()));
    }
#endif

    std::error_code error;
    const auto workingDirectory = std::filesystem::current_path(error);
    return error ? std::filesystem::path{} : workingDirectory / "DiggingJim";
}

std::filesystem::path Paths::executableDirectory()
{
    const auto path = executablePath();
    return path.has_filename() ? path.parent_path() : path;
}

std::filesystem::path Paths::assetsDirectory()
{
#ifdef __APPLE__
    const auto bundledAssets = executableDirectory().parent_path() / "Resources" / "assets";
    if (pathExists(bundledAssets))
        return bundledAssets;
#endif

    const auto adjacentAssets = executableDirectory() / "assets";
    if (pathExists(adjacentAssets))
        return adjacentAssets;

    std::error_code error;
    return std::filesystem::current_path(error) / "assets";
}

std::filesystem::path Paths::assetPath(const std::string_view relativePath)
{
    return assetsDirectory() / std::filesystem::path(std::string(relativePath));
}

std::filesystem::path Paths::userDataDirectory()
{
#ifdef _WIN32
    if (const char* appData = std::getenv("APPDATA"); appData && *appData)
        return std::filesystem::path(appData) / "Digging Jim";
#elif defined(__APPLE__)
    if (const char* home = std::getenv("HOME"); home && *home)
        return std::filesystem::path(home) / "Library" / "Application Support" / "Digging Jim";
#else
    if (const char* xdgDataHome = std::getenv("XDG_DATA_HOME"); xdgDataHome && *xdgDataHome)
        return std::filesystem::path(xdgDataHome) / "digging-jim";
    if (const char* home = std::getenv("HOME"); home && *home)
        return std::filesystem::path(home) / ".local" / "share" / "digging-jim";
#endif

    return executableDirectory() / "data";
}

std::filesystem::path Paths::settingsFile()
{
    return userDataDirectory() / "settings.txt";
}

std::filesystem::path Paths::progressFile()
{
    return userDataDirectory() / "progress.txt";
}

std::filesystem::path Paths::userCavesDirectory()
{
    return userDataDirectory() / "caves";
}

bool Paths::ensureUserDataDirectory()
{
    return ensureDirectory(userDataDirectory());
}

bool Paths::ensureUserCavesDirectory()
{
    return ensureDirectory(userCavesDirectory());
}
