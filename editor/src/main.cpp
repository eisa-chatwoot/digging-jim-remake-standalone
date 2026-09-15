#include "Editor/Editor.h"
#include "Utils/Paths.h"
#include <clocale>
#ifdef _WIN32
#include <windows.h>
#else
#include <filesystem>
#endif

int main()
{
#ifdef _WIN32
    std::setlocale(LC_ALL, ".UTF-8");
    SetCurrentDirectoryW(Paths::executableDirectory().c_str());
#else
    std::error_code ec;
    std::filesystem::current_path(Paths::executableDirectory(), ec);
#endif

    Editor editor;
    editor.run();
}
