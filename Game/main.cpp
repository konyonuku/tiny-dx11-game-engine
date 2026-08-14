#include "Core/Application.h"

#include "TriangleRenderer.h"

class SandboxApp : public Application
{
protected:
    bool OnStart() override
    {
        return mTriangle.Create(Device());
    }

    void OnRender(Renderer& renderer) override
    {
        mTriangle.Render(renderer.Context());
    }

private:
    TriangleRenderer mTriangle;
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
