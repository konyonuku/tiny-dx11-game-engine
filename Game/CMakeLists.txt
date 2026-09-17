add_executable(Game
    main.cpp
    Cube.h Cube.cpp
    DemoWorld.h DemoWorld.cpp
    DemoAssets.h DemoAssets.cpp
)

target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Game PRIVATE Engine)

set_target_properties(Game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Game>"
)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:Game>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Default.hlsl"
            "$<TARGET_FILE_DIR:Game>/Shaders/Default.hlsl"
)
