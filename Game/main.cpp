#include "Core/Application.h"

#include "Core/GameTimer.h"
#include "Math/Transform.h"
#include "TriangleRenderer.h"


class SandboxApp : public Application
{
protected:
    bool OnStart() override
    {
        return mTriangle.Create(Device());
    }

    void OnUpdate(float deltaTime) override
    {
        mTransform.rotation.z += mRotationSpeed * deltaTime;
    }

    void OnRender(Renderer& renderer) override
    {
        mTriangle.Render(renderer.Context(), mTransform.WorldMatrix());
    }

private:
    TriangleRenderer mTriangle;
    Transform        mTransform;
    float            mRotationSpeed = 1.0f;
};

int main()
{
    Application::Desc desc;
    desc.title  = L"tiny-dx11-game-engine";
    desc.width  = 640;
    desc.height = 480;

    SandboxApp app;
    return app.Run(desc);
}
