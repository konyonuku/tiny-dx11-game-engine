add_executable(Game
    main.cpp
    Cube.h Cube.cpp
    DemoWorld.h DemoWorld.cpp
    DemoAssets.h DemoAssets.cpp
)

target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Game PRIVATE Engine)

# Fixed per-config output directory so the asset copy step can name it
# without referring to the Game target (that would create a dependency cycle).
set(GAME_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
    VS_DEBUGGER_WORKING_DIRECTORY "${GAME_OUTPUT_DIR}"
)

# Copy <repo>/Assets to <Game.exe dir>/Assets. Runs whenever any asset file changes,
# even when no C++ changed. CONFIGURE_DEPENDS picks up added/removed asset files.
file(GLOB_RECURSE GAME_ASSET_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/Assets/*")
set(GAME_ASSET_STAMP "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GameAssets-$<CONFIG>.stamp")

add_custom_command(
    OUTPUT "${GAME_ASSET_STAMP}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_SOURCE_DIR}/Assets"
            "${GAME_OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E touch "${GAME_ASSET_STAMP}"
    DEPENDS ${GAME_ASSET_FILES}
    COMMENT "Copying Assets next to Game"
    VERBATIM
)

add_custom_target(GameAssets DEPENDS "${GAME_ASSET_STAMP}")
add_dependencies(Game GameAssets)
