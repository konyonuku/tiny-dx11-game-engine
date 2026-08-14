add_library(Engine STATIC
    Core/Log.h
    Core/GameTimer.h Core/GameTimer.cpp
    Core/Application.h Core/Application.cpp
    Platform/Window.h Platform/Window.cpp
    Graphics/GraphicsDevice.h Graphics/GraphicsDevice.cpp
    Graphics/SwapChain.h Graphics/SwapChain.cpp
    Graphics/Renderer.h Graphics/Renderer.cpp
    Graphics/ShaderCompiler.h Graphics/ShaderCompiler.cpp
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Engine PUBLIC
    d3d11
    dxgi
    d3dcompiler
)
