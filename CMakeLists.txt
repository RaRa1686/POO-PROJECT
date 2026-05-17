cmake_minimum_required(VERSION 3.16)
project(HeadFootball)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

include_directories(include)

add_executable(HeadFootball
    src/Main.cpp
    src/GameObject.cpp
    src/Ball.cpp
    src/Player.cpp
    src/HumanPlayer.cpp
    src/AIPlayer.cpp
    src/Game.cpp
)

target_link_libraries(HeadFootball SFML::Graphics SFML::Window SFML::System SFML::Audio)