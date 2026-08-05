@echo off
setlocal EnableExtensions

for %%I in ("%~dp0\..\..") do set "SOURCE_DIR=%%~fI"
set "BUILD_DIR=%SOURCE_DIR%\build-windows"

if not defined VCPKG_ROOT set "VCPKG_ROOT=%USERPROFILE%\vcpkg"

if exist "%ProgramW6432%\Git\cmd\git.exe" set "PATH=%ProgramW6432%\Git\cmd;%PATH%"
if exist "%ProgramFiles%\Git\cmd\git.exe" set "PATH=%ProgramFiles%\Git\cmd;%PATH%"
for /d %%I in ("%LOCALAPPDATA%\Microsoft\WinGet\Packages\Kitware.CMake_*") do (
    for /d %%J in ("%%~fI\cmake-*-windows-*") do (
        if exist "%%~fJ\bin\cmake.exe" set "PATH=%%~fJ\bin;%PATH%"
    )
)

where git >nul 2>nul
if errorlevel 1 (
    echo Git was not found on PATH.
    exit /b 1
)
where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake was not found on PATH.
    exit /b 1
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo Visual Studio Build Tools was not found.
    exit /b 1
)

set "HOST_ARCH=x64"
if /I "%PROCESSOR_ARCHITECTURE%"=="ARM64" set "HOST_ARCH=arm64"
if /I "%PROCESSOR_ARCHITEW6432%"=="ARM64" set "HOST_ARCH=arm64"

set "VS_REQUIREMENTS=Microsoft.VisualStudio.Component.VC.Tools.x86.x64"
if /I "%HOST_ARCH%"=="arm64" set "VS_REQUIREMENTS=%VS_REQUIREMENTS% Microsoft.VisualStudio.Component.VC.Tools.ARM64"

set "VS_INSTALLATION="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires %VS_REQUIREMENTS% -property installationPath`) do set "VS_INSTALLATION=%%I"
if not defined VS_INSTALLATION (
    echo The MSVC x64/x86 build tools were not found.
    exit /b 1
)

call "%VS_INSTALLATION%\Common7\Tools\VsDevCmd.bat" -host_arch=%HOST_ARCH% -arch=x64
if errorlevel 1 exit /b 1

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo Bootstrapping vcpkg in "%VCPKG_ROOT%"...
    git clone https://github.com/microsoft/vcpkg "%VCPKG_ROOT%"
    if errorlevel 1 exit /b 1
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
    if errorlevel 1 exit /b 1
)

cmake -U CMAKE_MT -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --target windows_release --config Release --parallel 4
if errorlevel 1 exit /b 1

echo.
echo Standalone ZIP and single-file EXE packages created in "%BUILD_DIR%\dist".
