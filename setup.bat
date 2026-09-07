@echo off
REM UNO Game Setup Script for Windows

echo.
echo ================================================
echo UNO GAME - SETUP SCRIPT (WINDOWS)
echo ================================================
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake is not installed!
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)
echo OK - CMake found

REM Check if Visual Studio is available
where msbuild >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Visual Studio/MSBuild not found
    echo Please install Visual Studio Community Edition
    echo Download from: https://visualstudio.microsoft.com/downloads/
)

echo.
echo Creating build directory...
if not exist "build" mkdir build
cd build

echo.
echo Running CMake configuration...
cmake -G "Visual Studio 16 2019" ..

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ================================================
echo BUILD SUCCESSFUL!
echo ================================================
echo.
echo Executables created in Release folder:
echo   uno_server.exe   - Start multiplayer server
echo   uno_client.exe   - Connect to server as client
echo   uno_single.exe   - Single player vs AI
echo.
echo To run tests: ctest --verbose --config Release
echo.
pause
