add_executable(framework_tests
    framework_tests.cpp
)

target_link_libraries(framework_tests PRIVATE Engine)

add_test(NAME framework_lifecycle COMMAND framework_tests)

add_executable(render_collection_tests
    render_collection_tests.cpp
)

target_link_libraries(render_collection_tests PRIVATE Engine)

add_test(NAME render_collection COMMAND render_collection_tests)

add_executable(resource_cache_tests
    resource_cache_tests.cpp
)

target_link_libraries(resource_cache_tests PRIVATE Engine)
target_compile_definitions(resource_cache_tests PRIVATE
    JITAI_SOURCE_ASSET_DIR="${PROJECT_SOURCE_DIR}/Assets"
)

add_test(NAME resource_cache COMMAND resource_cache_tests)
