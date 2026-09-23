#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Math/Vector.h"
#include "Resources/ObjLoader.h"

namespace fs = std::filesystem;

// Pure CPU tests: OBJ text -> MeshData conversion rules. No D3D device.
namespace
{
    const fs::path kSourceAssets = fs::path(JITAI_SOURCE_ASSET_DIR);

    void Expect(bool condition, const std::string& message)
    {
        if(!condition) throw std::runtime_error(message);
    }

    bool Near(float a, float b) { return std::fabs(a - b) < 1e-4f; }

    bool Near(const Vector3& a, const Vector3& b) { return Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z); }

    Vector3 Position(const Vertex& v) { return {v.position[0], v.position[1], v.position[2]}; }
    Vector3 Normal(const Vertex& v) { return {v.normal[0], v.normal[1], v.normal[2]}; }

    MeshData Parse(const std::string& text)
    {
        std::optional<MeshData> mesh = ObjLoader::ParseFromString(text, "test");
        Expect(mesh.has_value(), "parse must succeed");
        return std::move(*mesh);
    }

    const Vertex& Corner(const MeshData& mesh, std::size_t triangle, std::size_t corner)
    {
        return mesh.vertices[mesh.indices[triangle * 3 + corner]];
    }

    // Engine front face is clockwise: (b - a) x (c - a) points out of the front side.
    Vector3 WindingNormal(const MeshData& mesh, std::size_t triangle)
    {
        const Vector3 a = Position(Corner(mesh, triangle, 0));
        const Vector3 b = Position(Corner(mesh, triangle, 1));
        const Vector3 c = Position(Corner(mesh, triangle, 2));
        return (b - a).Cross(c - a).Normalized();
    }

    void TestQuadBecomesTwoTriangles()
    {
        const MeshData mesh = Parse(
            "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n"
            "f 1 2 3 4\n");
        Expect(mesh.indices.size() == 6, "quad must become 2 triangles");

        const MeshData pentagon = Parse(
            "v 0 0 0\nv 1 0 0\nv 1.5 1 0\nv 0.5 2 0\nv -0.5 1 0\n"
            "f 1 2 3 4 5\n");
        Expect(pentagon.indices.size() == 9, "pentagon must fan into 3 triangles");
    }

    void TestVertexDeduplication()
    {
        // Two quads share an edge (positions 2, 3) but have different normals.
        const MeshData mesh = Parse(
            "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\nv 1 0 -1\nv 1 1 -1\n"
            "vn 0 0 1\nvn 1 0 0\n"
            "f 1//1 2//1 3//1 4//1\n"
            "f 2//2 5//2 6//2 3//2\n");
        Expect(mesh.indices.size() == 12, "two quads must give 12 indices");
        Expect(mesh.vertices.size() == 8, "identical (v, vt, vn) corners must be shared; different normals must not");
    }

    void TestZFlipAndWindingFlip()
    {
        const MeshData mesh = Parse(
            "v 1 2 3\nv 4 5 6\nv 7 8 9\n"
            "vn 0 0 1\n"
            "f 1//1 2//1 3//1\n");
        Expect(mesh.vertices.size() == 3 && mesh.indices.size() == 3, "one triangle expected");

        Expect(Near(Position(Corner(mesh, 0, 0)), {1, 2, -3}), "corner a keeps its place, z negated");
        Expect(Near(Position(Corner(mesh, 0, 1)), {7, 8, -9}), "winding flip: second corner must be OBJ corner c");
        Expect(Near(Position(Corner(mesh, 0, 2)), {4, 5, -6}), "winding flip: third corner must be OBJ corner b");
        Expect(Near(Normal(Corner(mesh, 0, 0)), {0, 0, -1}), "normal z must be negated");
    }

    void TestUvVFlip()
    {
        const MeshData mesh = Parse(
            "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
            "vt 0.25 0.1\nvt 1 0\nvt 0 1\n"
            "f 1/1 2/2 3/3\n");
        const Vertex& a = Corner(mesh, 0, 0);
        Expect(Near(a.uv[0], 0.25f) && Near(a.uv[1], 0.9f), "v must become 1 - v, u unchanged");
    }

    void TestMissingNormalUsesOutwardFaceNormal()
    {
        // CCW seen from +Y (OBJ convention) -> outward normal +Y, stays +Y in engine space.
        const MeshData up = Parse("v 0 0 0\nv 1 0 0\nv 0 0 -1\nf 1 2 3\n");
        for(std::size_t corner = 0; corner < 3; ++corner)
            Expect(Near(Normal(Corner(up, 0, corner)), {0, 1, 0}), "face normal must point up");

        // CCW seen from OBJ +Z -> engine -Z (toward a camera at negative z).
        const MeshData front = Parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
        Expect(Near(Normal(Corner(front, 0, 0)), {0, 0, -1}), "OBJ +Z face must face engine -Z");
        Expect(Near(WindingNormal(front, 0), {0, 0, -1}), "front face (CW) must agree with the normal");
    }

    void TestFaceNormalCornersAreNotShared()
    {
        // Both triangles use positions 1 and 3 but lie in different planes.
        const MeshData mesh = Parse(
            "v 0 0 0\nv 1 0 0\nv 0 1 0\nv 0 0 -1\n"
            "f 1 2 3\nf 1 3 4\n");
        Expect(mesh.vertices.size() == 6, "computed face normals differ, so corners must not be shared");
    }

    void TestMissingUvIsZero()
    {
        const MeshData mesh = Parse("v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2//1 3//1\n");
        for(const Vertex& v : mesh.vertices)
            Expect(v.uv[0] == 0.0f && v.uv[1] == 0.0f, "missing uv must be (0, 0)");
    }

    void TestNegativeRelativeIndices()
    {
        const MeshData absolute = Parse("v 1 2 3\nv 4 5 6\nv 7 8 9\nf 1 2 3\n");
        const MeshData relative = Parse("v 1 2 3\nv 4 5 6\nv 7 8 9\nf -3 -2 -1\n");
        Expect(relative.indices.size() == 3, "relative face must parse");
        for(std::size_t corner = 0; corner < 3; ++corner)
            Expect(Near(Position(Corner(relative, 0, corner)), Position(Corner(absolute, 0, corner))),
                   "-1 must mean the last vertex defined so far");
    }

    void TestInvalidInputsFail()
    {
        Expect(!ObjLoader::ParseFromString("", "empty"), "empty text must fail");
        Expect(!ObjLoader::ParseFromString("# only a comment\nv 0 0 0\n", "no faces"), "no faces must fail");
        Expect(!ObjLoader::ParseFromString("this is not an obj file\n\x01\x02", "garbage"), "garbage must fail");
        Expect(!ObjLoader::ParseFromString("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 9\n", "range"), "out-of-range index must fail");
        Expect(!ObjLoader::ParseFromString("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1/5 2/5 3/5\n", "uv range"), "out-of-range uv index must fail");
        Expect(!ObjLoader::ParseFromString("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 -9\n", "relative"), "invalid relative index must fail");
        Expect(!ObjLoader::LoadFromFile(kSourceAssets / "Models/does_not_exist.obj"), "missing file must fail");
    }

    void TestMtlLibIsIgnored()
    {
        const MeshData mesh = Parse("mtllib missing.mtl\nusemtl Red\nv 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
        Expect(mesh.indices.size() == 3, "missing .mtl must not block the mesh");
    }

    void TestWedgeAsset()
    {
        const std::optional<MeshData> mesh = ObjLoader::LoadFromFile(kSourceAssets / "Models/wedge.obj");
        Expect(mesh.has_value(), "wedge.obj must load");
        Expect(mesh->indices.size() == 24, "wedge = 3 quads + 2 triangles = 8 triangles");
        Expect(mesh->vertices.size() == 18, "wedge = 4 + 4 + 4 + 3 + 3 unique corners");

        // Every triangle's clockwise winding must agree with its authored normal: no inside-out faces.
        for(std::size_t triangle = 0; triangle < mesh->indices.size() / 3; ++triangle)
            Expect(WindingNormal(*mesh, triangle).Dot(Normal(Corner(*mesh, triangle, 0))) > 0.99f,
                   "triangle " + std::to_string(triangle) + " winding disagrees with its normal");

        // The tall wall (top edge, y = +0.5) is at engine +Z, away from the camera.
        for(const Vertex& v : mesh->vertices)
            if(Near(v.position[1], 0.5f))
                Expect(Near(v.position[2], 0.5f), "top edge must be at engine z = +0.5");
    }
}

int main()
{
    using Test = std::pair<const char*, void (*)()>;
    const std::vector<Test> tests = {
        {"quad becomes two triangles", TestQuadBecomesTwoTriangles},
        {"vertex deduplication", TestVertexDeduplication},
        {"z flip and winding flip", TestZFlipAndWindingFlip},
        {"uv v flip", TestUvVFlip},
        {"missing normal uses outward face normal", TestMissingNormalUsesOutwardFaceNormal},
        {"face-normal corners are not shared", TestFaceNormalCornersAreNotShared},
        {"missing uv is zero", TestMissingUvIsZero},
        {"negative relative indices", TestNegativeRelativeIndices},
        {"invalid inputs fail", TestInvalidInputsFail},
        {"mtllib is ignored", TestMtlLibIsIgnored},
        {"wedge asset", TestWedgeAsset},
    };

    int failures = 0;
    for (const auto& [name, test] : tests)
    {
        try
        {
            test();
            std::cout << "[PASS] " << name << '\n';
        }
        catch (const std::exception& error)
        {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }

    if (failures == 0)
        std::cout << "All OBJ loader tests passed.\n";

    return failures == 0 ? 0 : 1;
}
