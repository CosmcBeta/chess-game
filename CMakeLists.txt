cmake_minimum_required(VERSION 3.28)
project(ChessGame LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Try system-installed SFML first
find_package(SFML 3 COMPONENTS Graphics Audio QUIET)

if (NOT SFML_FOUND)
    message(STATUS "SFML not found on system. Fetching with FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
    )
    FetchContent_MakeAvailable(SFML)
endif()

# Source files
file(GLOB SRC_FILES src/*.cpp)

# Executable
add_executable(chess main.cpp ${SRC_FILES})
target_compile_features(chess PRIVATE cxx_std_17)
target_include_directories(chess PRIVATE src)

# Link SFML
target_link_libraries(chess PRIVATE SFML::Graphics SFML::Audio)
