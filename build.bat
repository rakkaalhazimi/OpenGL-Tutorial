@echo off


:: Check for argument
if "%1"=="" (
    echo Please provide a version number like: build.bat 1
    exit /b 1
)

:: Map version number to file path
set "VERSION=%1"

if "%VERSION%"=="1" (
    set "MAIN=src/YoutubeOpenGL001 - Create Window/main.cpp"
) else if "%VERSION%"=="2" (
    set "MAIN=src/YoutubeOpenGL002 - Triangle/main.cpp"
) else if "%VERSION%"=="3" (
    set "MAIN=src/YoutubeOpenGL003 - Index Buffer/main.cpp"
) else if "%VERSION%"=="4" (
    set "MAIN=src/YoutubeOpenGL004 - Organizing/main.cpp"
) else if "%VERSION%"=="5" (
    set "MAIN=src/YoutubeOpenGL005 - Shader/main.cpp"
) else if "%VERSION%"=="6" (
    set "MAIN=src/YoutubeOpenGL006 - Texture/main.cpp"
) else if "%VERSION%"=="7" (
    set "MAIN=src/YoutubeOpenGL007 - Going 3D/main.cpp"
) else (
    echo Unknown version: %VERSION%
    exit /b 1
)


:: Run CMake with the selected main file
cmake -DMAIN_SRC="%MAIN%" -G "MinGW Makefiles" .
cmake --build .

:: Run the output executable
MyOpenGLApp.exe