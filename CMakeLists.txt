cmake_minimum_required(VERSION 3.13)

# NOTE: Change your project name.
project(hello_nodec_game LANGUAGES CXX)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# The references about the best practice for executable to find dlls.
# * https://stackoverflow.com/questions/39807664/how-to-best-tell-cmake-where-to-find-dll
set(EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_DIRECTORY} CACHE STRING "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_DIRECTORY} CACHE STRING "")

if(MSVC)
    # For msvc (Windows), we should set the followings to output to a single directory not a subdirectories.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_DIRECTORY}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_DIRECTORY}")

    add_compile_options(/bigobj)

    # Since the static libraries is linked, no need to place them in the executable directory.
    # set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_DIRECTORY}")
    # set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_DIRECTORY}")
endif()

# TODO: Use CMakePresets.json for settings.
set(NODEC_GAME_EDITOR_ENABLED ON CACHE BOOL "" FORCE)

add_subdirectory(nodec_game_engine)

add_executable(${PROJECT_NAME} WIN32
    src/app.cpp
)


if(NODEC_GAME_EDITOR_ENABLED)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        EDITOR_MODE
    )
endif()

target_link_libraries(${PROJECT_NAME}
    nodec_game_engine
)

if(WIN32)
    # NOTE: Why use junction.
    # I honestly used cmake's "create_symlink" command. However, I got an garbled error.
    # Then, I tried to create a symbolic link using the native command "mklink".
    # However, to create a symbolic link, need to acquire the admin privilege.
    # So, I endup using a junction link...
    add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
        COMMAND mklink /J \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/org.nodec.game-engine\" \"${CMAKE_CURRENT_SOURCE_DIR}/nodec_game_engine/game_engine/resources/org.nodec.game-engine\"
        COMMAND mklink /J \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/org.nodec.game-editor\" \"${CMAKE_CURRENT_SOURCE_DIR}/nodec_game_engine/game_editor/resources/org.nodec.game-editor\"
        COMMAND mklink /J \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/org.nodec.hello-nodec-game\" \"${CMAKE_CURRENT_SOURCE_DIR}/resources/org.nodec.hello-nodec-game\"
    )
endif()
