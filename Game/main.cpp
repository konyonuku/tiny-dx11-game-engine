#include <cstddef>

#include "Core/Application.h"
#include "Math/Transform.h"
#include "Math/MathUtils.h"
#include "Graphics/Camera.h"

#include "World.h"


class SandboxApp : public Application
{
protected:
    bool OnStart() override
    {
        if(!mWorld.Initialize(Device())) return false;


        mCamera.SetLookAt(
            {0.0f, 0.0f, -4.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        );
        
        float fovY = ToRadian(60.0f);
        mCamera.SetPerspective(fovY, 1.f, 0.1f, 100.f);
        OnResize(GetWindow().ClientWidth(), GetWindow().ClientHeight());


        Transform firstCubeTransform;
        mWorld.CreateCube(firstCubeTransform, {0, 0, 0}, false);

        Transform secondCubeTransform;
        secondCubeTransform.position = {-2.0f, 0.0f, 1.0f};
        mWorld.CreateCube(secondCubeTransform, {2.0f, 0.0f, 0.0f}, false);

        Transform thirdCubeTransform;
        thirdCubeTransform.position = {2.0f, -1.0f, 0.0f};
        thirdCubeTransform.scale = thirdCubeTransform.scale * 0.5f;
        mWorld.CreateCube(thirdCubeTransform, {0.0f, 3.0f, 0.0f}, true);


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

        mWorld.Update(deltaTime);
        
        if(in.IsDown(Key::Up)) mWorld.MoveSelectedCube({0,0,1}, deltaTime);
        if(in.IsDown(Key::Left)) mWorld.MoveSelectedCube({-1,0,0}, deltaTime);
        if(in.IsDown(Key::Down)) mWorld.MoveSelectedCube({0,0,-1}, deltaTime);
        if(in.IsDown(Key::Right)) mWorld.MoveSelectedCube({1,0,0}, deltaTime);

        if(in.WasPressed(Key::R)) mWorld.Reset();
        if(in.WasPressed(Key::Space)) mWorld.ToggleSelectedCubeRotation();
        if(in.WasPressed(Key::Num1)) mWorld.SelectCube(0);
        if(in.WasPressed(Key::Num2)) mWorld.SelectCube(1);
        if(in.WasPressed(Key::Num3)) mWorld.SelectCube(2);

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
        mWorld.Render(renderer, mCamera);
    }

private:
    World            mWorld;
    Camera           mCamera;
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
