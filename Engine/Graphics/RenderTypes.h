#pragma once
#include <memory>

#include "Math/Matrix.h"
#include "Math/Vector.h"

class Mesh;
class Material;


struct FrameConstants
{
    Matrix4x4 viewProjection;
    Vector3 lightDirection;
    float lightIntensity;
    Vector3 lightColor;
    float ambientIntensity;
    Vector3 cameraPosition;
    float padding;
};

struct ObjectConstants
{
    Matrix4x4 world;
};

struct RenderItem
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    Matrix4x4 world;
};

static_assert(sizeof(FrameConstants) == (64+16+16+16)); //64+16+16+16
static_assert(sizeof(ObjectConstants) == 64);

/*
Frame Start
Bind Frame Constants(View, Projection, Light, Camera Position)
Bind ActorA's ObjectConstants
Draw ActorA
Bind ActorB's ObjectConstants
Draw ActorB
...
*/