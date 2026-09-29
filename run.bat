@echo off
setlocal

if not exist build mkdir build

where g++ >nul 2>nul
if %errorlevel%==0 (
    g++ -std=c++17 -Wall -Wextra -pedantic ^
        src\main.cpp ^
        src\Shell.cpp ^
        src\VirtualFileSystem.cpp ^
        -o build\emulator.exe

    if errorlevel 1 exit /b 1

    build\emulator.exe %*
    exit /b %errorlevel%
)

where cl >nul 2>nul
if %errorlevel%==0 (
    cl /std:c++17 /EHsc ^
        /Fe:build\emulator.exe ^
        src\main.cpp ^
        src\Shell.cpp ^
        src\VirtualFileSystem.cpp

    if errorlevel 1 exit /b 1

    build\emulator.exe %*
    exit /b %errorlevel%
)

echo Error: C++ compiler not found.
echo Install g++ or run this script from a Visual Studio Developer Command Prompt.
exit /b 1