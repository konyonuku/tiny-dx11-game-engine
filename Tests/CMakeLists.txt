add_executable(framework_tests
    framework_tests.cpp
)

target_link_libraries(framework_tests PRIVATE Engine)

add_test(NAME framework_lifecycle COMMAND framework_tests)
