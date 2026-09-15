#include "Game/Game.h"
#include "Utils/Paths.h"
#include <clocale>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <filesystem>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
    std::setlocale(LC_ALL, ".UTF-8");
    SetCurrentDirectoryW(Paths::executableDirectory().c_str());
#else
    std::error_code ec;
    std::filesystem::current_path(Paths::executableDirectory(), ec);
#endif

    std::string caveFile      = "";
    int         startCave     = 1;
    bool        editorMode    = false;
    bool        fullscreen    = true;
    bool        developerMode = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if      (arg == "--editor-mode")          editorMode    = true;
        else if (arg == "--fullscreen")            fullscreen    = true;
        else if (arg == "--developer")             developerMode = true;
        else if (arg.rfind("--cave=",  0) == 0)   caveFile      = arg.substr(7);
        else if (arg.rfind("--start=", 0) == 0)   startCave     = std::stoi(arg.substr(8));
    }

    Game game;
    if (!game.run(caveFile, startCave, editorMode, fullscreen, developerMode))
    {
#ifdef _WIN32
        const std::string err = game.lastError();
        const std::wstring werr(err.begin(), err.end());
        MessageBoxW(nullptr, werr.c_str(), L"DiggingJim - Fatal Error", MB_OK | MB_ICONERROR);
#endif
        return 1;
    }
    return 0;
}