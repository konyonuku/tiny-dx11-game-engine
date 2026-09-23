#include "Resources/ObjLoader.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <sstream>
#include <unordered_map>

#include "Core/Log.h"
#include "Math/Vector.h"

// The only translation unit that compiles the tinyobjloader implementation (ODR).
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


namespace
{
    // One GPU vertex per unique (position, uv, normal) index combination.
    struct IndexKey
    {
        int position;
        int uv;
        int normal;

        bool operator==(const IndexKey& other) const
        {
            return position == other.position && uv == other.uv && normal == other.normal;
        }
    };

    struct IndexKeyHash
    {
        std::size_t operator()(const IndexKey& key) const
        {
            std::size_t hash = std::hash<int>{}(key.position);
            hash = hash * 31 + std::hash<int>{}(key.uv);
            hash = hash * 31 + std::hash<int>{}(key.normal);
            return hash;
        }
    };

    bool InRange(int index, std::size_t count) { return index >= 0 && static_cast<std::size_t>(index) < count; }

    Vector3 ToVector3(const float (&v)[3]) { return {v[0], v[1], v[2]}; }

    // Engine convention: clockwise front face, so the outward normal is (b - a) x (c - a).
    Vector3 FaceNormal(const Vertex& a, const Vertex& b, const Vertex& c)
    {
        const Vector3 pa = ToVector3(a.position);
        return (ToVector3(b.position) - pa).Cross(ToVector3(c.position) - pa).Normalized();
    }
}

namespace ObjLoader
{
    std::optional<MeshData> LoadFromFile(const std::filesystem::path& path)
    {
        // Read through std::filesystem::path so non-ASCII Windows paths work.
        std::ifstream file(path, std::ios::binary);
        if(!file) {
            Core::LogError("OBJ file not found: %s", path.string().c_str());
            return std::nullopt;
        }

        std::ostringstream text;
        text << file.rdbuf();
        return ParseFromString(text.str(), path.string());
    }

    std::optional<MeshData> ParseFromString(const std::string& objText, const std::string& sourceName)
    {
        tinyobj::ObjReaderConfig config;
        config.triangulate = true;
        config.vertex_color = false;

        // Empty mtl text: `mtllib` lines are ignored. Material belongs to the engine.
        tinyobj::ObjReader reader;
        if(!reader.ParseFromString(objText, std::string(), config)) {
            Core::LogError("OBJ parse failed: %s: %s", sourceName.c_str(), reader.Error().c_str());
            return std::nullopt;
        }

        const tinyobj::attrib_t& attrib = reader.GetAttrib();
        const std::size_t positionCount = attrib.vertices.size() / 3;
        const std::size_t uvCount = attrib.texcoords.size() / 2;
        const std::size_t normalCount = attrib.normals.size() / 3;

        MeshData mesh;
        std::unordered_map<IndexKey, uint32_t, IndexKeyHash> vertexLookup;

        for(const tinyobj::shape_t& shape : reader.GetShapes()) {
            const std::vector<tinyobj::index_t>& corners = shape.mesh.indices;
            for(std::size_t face = 0; face < shape.mesh.num_face_vertices.size(); ++face) {
                if(shape.mesh.num_face_vertices[face] != 3) {
                    Core::LogError("OBJ face was not triangulated: %s", sourceName.c_str());
                    return std::nullopt;
                }
            }

            for(std::size_t first = 0; first + 2 < corners.size(); first += 3) {
                Vertex triangle[3] {};
                bool needsFaceNormal[3] {};

                for(int corner = 0; corner < 3; ++corner) {
                    const tinyobj::index_t& index = corners[first + corner];
                    if(!InRange(index.vertex_index, positionCount)
                       || (index.texcoord_index >= 0 && !InRange(index.texcoord_index, uvCount))
                       || (index.normal_index >= 0 && !InRange(index.normal_index, normalCount))) {
                        Core::LogError("OBJ index out of range: %s", sourceName.c_str());
                        return std::nullopt;
                    }

                    Vertex& vertex = triangle[corner];
                    const std::size_t p = static_cast<std::size_t>(index.vertex_index) * 3;
                    vertex.position[0] = attrib.vertices[p + 0];
                    vertex.position[1] = attrib.vertices[p + 1];
                    vertex.position[2] = -attrib.vertices[p + 2];

                    if(index.texcoord_index >= 0) {
                        const std::size_t t = static_cast<std::size_t>(index.texcoord_index) * 2;
                        vertex.uv[0] = attrib.texcoords[t + 0];
                        vertex.uv[1] = 1.0f - attrib.texcoords[t + 1];
                    }

                    if(index.normal_index >= 0) {
                        const std::size_t n = static_cast<std::size_t>(index.normal_index) * 3;
                        const Vector3 normal = Vector3{attrib.normals[n + 0], attrib.normals[n + 1], -attrib.normals[n + 2]}.Normalized();
                        vertex.normal[0] = normal.x;
                        vertex.normal[1] = normal.y;
                        vertex.normal[2] = normal.z;
                    } else {
                        needsFaceNormal[corner] = true;
                    }
                }

                // Winding flip: OBJ (a, b, c) -> engine (a, c, b).
                const int order[3] = {0, 2, 1};
                const Vector3 faceNormal = FaceNormal(triangle[0], triangle[2], triangle[1]);

                for(int corner : order) {
                    Vertex& vertex = triangle[corner];
                    const tinyobj::index_t& index = corners[first + corner];

                    if(needsFaceNormal[corner]) {
                        // Face normals differ per triangle, so these corners are never shared.
                        vertex.normal[0] = faceNormal.x;
                        vertex.normal[1] = faceNormal.y;
                        vertex.normal[2] = faceNormal.z;
                        mesh.indices.push_back(static_cast<uint32_t>(mesh.vertices.size()));
                        mesh.vertices.push_back(vertex);
                        continue;
                    }

                    const IndexKey key {index.vertex_index, index.texcoord_index, index.normal_index};
                    const auto [found, inserted] = vertexLookup.try_emplace(key, static_cast<uint32_t>(mesh.vertices.size()));
                    if(inserted)
                        mesh.vertices.push_back(vertex);
                    mesh.indices.push_back(found->second);
                }
            }
        }

        if(mesh.indices.empty()) {
            Core::LogError("OBJ has no triangles: %s", sourceName.c_str());
            return std::nullopt;
        }

        return mesh;
    }
}
