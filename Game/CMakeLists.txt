add_executable(Game
    main.cpp
    CubeRenderer.h CubeRenderer.cpp
)

target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Game PRIVATE Engine)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:Game>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Default.hlsl"
            "$<TARGET_FILE_DIR:Game>/Shaders/Default.hlsl"
)
