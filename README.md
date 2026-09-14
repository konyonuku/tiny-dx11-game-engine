# tiny-dx11-game-engine

A small 3D game engine built with C++ and DirectX 11, developed for a 3D Sokoban Demo.

<br>

## Development Roadmap

| # | Goal | Main Tasks | Status | Note |
|---|---|---|---|---|
| **Week 1** | DX11 Rendering Foundation | DX11 bootstrap, engine structure, custom Vector/Matrix, vertex/index/constant buffers, Transform, Camera, depth buffer | **Done** | colored 3D cube rendering<br>(8.14 ~ 8.30) |
| **Week 2** | Basic 3D Renderer | Texture/Sampler, diffuse, Blinn–Phong lighting, Mesh/Material/Shader abstractions, rasterizer/depth-stencil state, window resize handling | **Done** | multi-cube demo<br>(9.1 ~ 9.11) |
| **Week 3** | Gameplay Architecture & Asset Pipeline | Frame-based InputState, World/Actor/ActorComponent lifecycle, StaticMeshComponent, shared frame/object render data, ResourceManager, tinyobjloader, ImGui debug panel, first PlayerController/Pawn | **In Progress** |  |
| **Week 4** | Playable 3D Sokoban | SokobanBoard, text stage loading, board-to-world representation, player/box Actors, movement/pushing rules, collision blocking, clear condition, restart, movement interpolation |  |  |
| **Week 5** | Undo & Persistent Ghost Replay | MoveHistory, Undo with history branching, ReplayRecorder, versioned replay file I/O, frame-rate-independent ReplayPlayer, transparent GhostActor, opaque/transparent render passes |  |  |
| **Week 6** | Polish & Portfolio | Multiple stages, stage selection, regression testing, resize/minimize/focus handling, UI polish, Refactoring |  |  |

