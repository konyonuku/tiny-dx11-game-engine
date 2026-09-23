add_library(Engine STATIC
    Core/Log.h
    Core/GameTimer.h Core/GameTimer.cpp
    Core/Application.h Core/Application.cpp
    Platform/Window.h Platform/Window.cpp
    Platform/InputState.h Platform/InputState.cpp
    Platform/Paths.h Platform/Paths.cpp
    Graphics/GraphicsDevice.h Graphics/GraphicsDevice.cpp
    Graphics/SwapChain.h Graphics/SwapChain.cpp
    Graphics/Renderer.h Graphics/Renderer.cpp
    Graphics/ShaderCompiler.h Graphics/ShaderCompiler.cpp

    Math/Vector.h Math/Matrix.h Math/Transform.h Math/MathUtils.h
    Graphics/Buffer.h Graphics/Buffer.cpp
    Graphics/Camera.h Graphics/Camera.cpp
    Graphics/Texture2D.h Graphics/Texture2D.cpp
    Graphics/Mesh.h Graphics/Mesh.cpp
    Graphics/Material.h Graphics/Material.cpp
    Graphics/Shader.h Graphics/Shader.cpp
    Graphics/RenderState.h Graphics/RenderState.cpp
    Graphics/RenderTypes.h
    Graphics/Vertex.h

    GameFramework/World.h GameFramework/World.cpp
    GameFramework/Actor.h GameFramework/Actor.cpp
    GameFramework/ActorComponent.h GameFramework/ActorComponent.cpp
    GameFramework/StaticMeshComponent.h GameFramework/StaticMeshComponent.cpp

    Resources/ResourceManager.h Resources/ResourceManager.cpp

    ${PROJECT_SOURCE_DIR}/External/stb_image.h
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Engine PRIVATE ${PROJECT_SOURCE_DIR}/External)

target_link_libraries(Engine PUBLIC
    d3d11
    dxgi
    d3dcompiler
)
