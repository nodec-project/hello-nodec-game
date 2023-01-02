cmake_minimum_required(VERSION 3.10)

project(hello_nodec_game)

# TODO: Use CMakePresets.json for settings.
set(NODEC_GAME_EDITOR_ENABLED ON CACHE BOOL "" FORCE)

add_subdirectory(nodec_game_engine)

add_subdirectory(apps)
