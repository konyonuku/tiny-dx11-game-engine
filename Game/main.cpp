#include <cstddef>

#include "Core/Application.h"
#include "Core/Log.h"
#include "Math/Transform.h"
#include "Math/MathUtils.h"
#include "Graphics/Camera.h"

#include "DemoWorld.h"


class SandboxApp : public Application
{
protected:
    bool OnStart() override
    {
        if(!mDemoWorld.Initialize(Device())) return false;

        mCamera.SetLookAt(
            {0.0f, 0.0f, -4.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        );
        
        float fovY = ToRadian(60.0f);
        mCamera.SetPerspective(fovY, 1.f, 0.1f, 100.f);
        OnResize(GetWindow().ClientWidth(), GetWindow().ClientHeight());

        Transform firstCubeTransform;
        if(!mDemoWorld.CreateCube(firstCubeTransform, {0, 0, 0}, false)) return false;

        Transform secondCubeTransform;
        secondCubeTransform.position = {-2.0f, 0.0f, 1.0f};
        if(!mDemoWorld.CreateCube(secondCubeTransform, {2.0f, 0.0f, 0.0f}, false)) return false;

        Transform thirdCubeTransform;
        thirdCubeTransform.position = {2.0f, -1.0f, 0.0f};
        thirdCubeTransform.scale = thirdCubeTransform.scale * 0.5f;
        if(!mDemoWorld.CreateCube(thirdCubeTransform, {0.0f, 3.0f, 0.0f}, true)) return false;

        return true;
    }

    void OnResize(uint32_t width, uint32_t height) override
    {
        if(height == 0) return;
        mCamera.SetAspect(static_cast<float>(width)/static_cast<float>(height));
    }

    void OnUpdate(float deltaTime) override
    {
        const InputState& in = Input();

        mDemoWorld.Update(deltaTime);
        
        if(in.IsDown(Key::Up)) mDemoWorld.MoveSelectedCube({0,0,1}, deltaTime);
        if(in.IsDown(Key::Left)) mDemoWorld.MoveSelectedCube({-1,0,0}, deltaTime);
        if(in.IsDown(Key::Down)) mDemoWorld.MoveSelectedCube({0,0,-1}, deltaTime);
        if(in.IsDown(Key::Right)) mDemoWorld.MoveSelectedCube({1,0,0}, deltaTime);

        if(in.WasPressed(Key::R)) mDemoWorld.Reset();
        if(in.WasPressed(Key::Space)) mDemoWorld.ToggleSelectedCubeRotation();
        if(in.WasPressed(Key::Num1)) mDemoWorld.SelectCube(0);
        if(in.WasPressed(Key::Num2)) mDemoWorld.SelectCube(1);
        if(in.WasPressed(Key::Num3)) mDemoWorld.SelectCube(2);

        // Orbit Camera
        // mCameraAngle += mCameraSpeed * deltaTime;
        // Vector3 eye {std::cos(mCameraAngle) * mCameraRadius, 0.f, std::sin(mCameraAngle) * mCameraRadius};
        // mCamera.SetLookAt(
        //     eye,
        //     {0.f, 0.f, 0.f},
        //     {0.f, 1.f, 0.f}
        // );
    }

    void OnRender(Renderer& renderer) override
    {
        const bool rendered = mDemoWorld.Render(renderer, mCamera);
        if(!rendered && !mRenderFailureReported)
            Core::LogError("Actor world rendering failed.");
        mRenderFailureReported = !rendered;
    }

private:
    DemoWorld        mDemoWorld;
    Camera           mCamera;
    bool             mRenderFailureReported = false;
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
