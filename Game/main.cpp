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
        mWorld.Update(deltaTime);
        
        if(mUpDown) mWorld.MoveSelectedCube({0,0,1}, deltaTime);
        if(mLeftDown) mWorld.MoveSelectedCube({-1,0,0}, deltaTime);
        if(mDownDown) mWorld.MoveSelectedCube({0,0,-1}, deltaTime);
        if(mRightDown) mWorld.MoveSelectedCube({1,0,0}, deltaTime);


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

    void OnKeyEvent(uint32_t key, bool isDown) override
    {
        switch(key) {
            case VK_LEFT:  mLeftDown  = isDown; return;
            case VK_RIGHT: mRightDown = isDown; return;
            case VK_UP:    mUpDown    = isDown; return;
            case VK_DOWN:  mDownDown  = isDown; return;
        }

        if(!isDown) {
            switch(key) {
                case 'R':
                    ClearInput();
                    mWorld.Reset();
                    return;
                case VK_SPACE:
                    mWorld.ToggleSelectedCubeRotation();
                    return;
                case '1':
                case '2':
                case '3':
                    mWorld.SelectCube(static_cast<std::size_t>(key - '1'));
                    return;
            }
        }
    }

    void OnKillFocus() override
    {
        ClearInput();
    }

private:
    void ClearInput()
    {
        mLeftDown = false;
        mRightDown = false;
        mUpDown = false;
        mDownDown = false;
    }

    World            mWorld;
    Camera           mCamera;

    bool mLeftDown  = false;
    bool mRightDown = false;
    bool mUpDown    = false;
    bool mDownDown  = false;
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
