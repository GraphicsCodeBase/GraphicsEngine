@echo off
setlocal

echo [INFO] Setting up build folder...
if not exist build (
    mkdir build
)

cd build

echo [INFO] Running CMake generation...
cmake .. -G "Visual Studio 17 2022" -A x64

REM Check for failure
if %errorlevel% neq 0 (
    echo [ERROR] CMake failed with code %errorlevel%
    pause
    exit /b %errorlevel%
)

echo [SUCCESS] CMake ran successfully!
pause
