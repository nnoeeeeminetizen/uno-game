cmake_minimum_required(VERSION 3.10)
project(UNO_Game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Card.cpp
    src/Deck.cpp
    src/Player.cpp
    src/Game.cpp
    src/AI.cpp
)

# Header files
set(HEADERS
    include/Card.h
    include/Deck.h
    include/Player.h
    include/Game.h
    include/AI.h
    include/Utils.h
)

# Create executable
add_executable(uno-game ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(uno-game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compiler flags
if(MSVC)
    target_compile_options(uno-game PRIVATE /W4)
else()
    target_compile_options(uno-game PRIVATE -Wall -Wextra -Wpedantic)
endif()
