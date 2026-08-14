cmake_minimum_required(VERSION 3.10.0)
project(tiny-dx11-game-engine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

add_subdirectory(Engine)
add_subdirectory(Game)
