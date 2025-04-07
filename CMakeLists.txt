cmake_minimum_required(VERSION 3.10)
project(MyOpenGLApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output all executables into build/bin/
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(include)
include_directories(external/glfw/include)


# Allow user to specify which main file to use
if(NOT DEFINED MAIN_SRC)
    set(MAIN_SRC src/main.cpp) # Default fallback
endif()

# Source files
# Get the directory where MAIN_SRC lives
get_filename_component(MAIN_SRC_DIR ${MAIN_SRC} DIRECTORY)
file(GLOB_RECURSE SOURCES 
    "${MAIN_SRC_DIR}/*.cpp" 
    "${MAIN_SRC_DIR}/*.h"
    "src/glad.c"
)

# set(SOURCES
#     ${MAIN_SRC}
#     src/glad.c
# )

# Create executable
add_executable(MyOpenGLApp ${SOURCES})

# Link OpenGL (Windows)
target_link_libraries(MyOpenGLApp 
    ${CMAKE_SOURCE_DIR}/external/glfw/lib-mingw/libglfw3dll.a
    opengl32
    gdi32
    winmm
)

# Copy the dll on the same level of exe file
add_custom_command(TARGET MyOpenGLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/glfw/lib-mingw/glfw3.dll"
        $<TARGET_FILE_DIR:MyOpenGLApp>
)