@echo off
if not exist build mkdir build
g++ -std=c++17 -Wall -Wextra -pedantic src\main.cpp src\Shell.cpp -o build\emulator.exe
if errorlevel 1 exit /b 1
build\emulator.exe %*
