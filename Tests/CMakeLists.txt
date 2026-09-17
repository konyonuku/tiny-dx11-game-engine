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
