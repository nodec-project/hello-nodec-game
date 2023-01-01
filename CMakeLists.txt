cmake_minimum_required(VERSION 3.10)

project(hello_nodec_game)

set(CMAKE_CXX_STANDARD 14)

option(NODEC_GAME_EDITOR_ENABLED ON)

add_subdirectory(nodec/nodec)

add_subdirectory(nodec/modules/nodec_engine)
add_subdirectory(nodec/modules/nodec_input)
add_subdirectory(nodec/modules/nodec_rendering)
add_subdirectory(nodec/modules/nodec_resources)
add_subdirectory(nodec/modules/nodec_scene)
add_subdirectory(nodec/modules/nodec_scene_audio)
add_subdirectory(nodec/modules/nodec_scene_serialization)
add_subdirectory(nodec/modules/nodec_screen)
add_subdirectory(nodec/modules/nodec_serialization)
add_subdirectory(nodec/modules/nodec_world)
add_subdirectory(nodec/modules/nodec_animations)
add_subdirectory(nodec/modules/nodec_physics)

if(NODEC_GAME_EDITOR_ENABLED)
    add_subdirectory(nodec/modules/assimp/assimp)
    add_subdirectory(nodec/modules/imgui)
    add_subdirectory(nodec/modules/imessentials)
    add_subdirectory(nodec/modules/imwindows)
    add_subdirectory(nodec/modules/imguizmo)
    add_subdirectory(nodec/modules/nodec_scene_editor)
endif()

add_subdirectory(apps)
