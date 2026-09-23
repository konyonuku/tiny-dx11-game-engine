#pragma once
#include <cstdint>
#include <vector>

#include "Graphics/Vertex.h"

// CPU-side mesh in engine convention (left-handed, clockwise front face, UV origin top-left).
// Loaders produce it; Mesh::Create uploads it to the GPU.
struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
