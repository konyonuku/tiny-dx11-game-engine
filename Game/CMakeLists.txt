add_executable(Game
    main.cpp
    TriangleRenderer.h TriangleRenderer.cpp
)

target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Game PRIVATE Engine)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:Game>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Triangle.hlsl"
            "$<TARGET_FILE_DIR:Game>/Shaders/Triangle.hlsl"
)
