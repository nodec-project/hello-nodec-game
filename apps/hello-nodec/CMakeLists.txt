cmake_minimum_required(VERSION 3.10)

project(hello-nodec LANGUAGES CXX)

add_executable(${PROJECT_NAME} WIN32
    src/app.cpp
)

target_link_libraries(${PROJECT_NAME}
    nodec_game_engine
)