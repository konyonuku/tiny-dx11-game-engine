#pragma once
#include <filesystem>
#include <optional>
#include <string>

#include "Graphics/MeshData.h"

// Wavefront OBJ -> MeshData. No GPU access.
//
// Conversion (OBJ/Blender: right-handed, CCW front, UV origin bottom-left):
//   position.z, normal.z  -> negated          (RH -> LH)
//   triangle (a, b, c)    -> (a, c, b)        (mirroring flips winding back to CW front)
//   uv.v                  -> 1 - v            (bottom-left -> top-left origin)
// Missing normal -> face normal of that triangle. Missing uv -> (0, 0).
// Polygons are fan-triangulated. All shapes merge into one mesh; .mtl is ignored.
namespace ObjLoader
{
    std::optional<MeshData> LoadFromFile(const std::filesystem::path& path);

    // sourceName is only used in log messages.
    std::optional<MeshData> ParseFromString(const std::string& objText, const std::string& sourceName = "<memory>");
}
