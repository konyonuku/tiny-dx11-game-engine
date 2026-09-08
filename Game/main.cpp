#include <cmath>

#include "Core/Application.h"
#include "Core/GameTimer.h"
#include "Math/Transform.h"
#include "Math/MathUtils.h"
#include "Graphics/Camera.h"

#include "CubeRenderer.h"


class SandboxApp : public Application
{
protected:
    bool OnStart() override
    {
        if(!mTriangle.Create(Device())) return false;

        mCamera.SetLookAt(
            {0.0f, 0.0f, -3.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        );
        
        mCameraAngle = -PI / 2;
        float fovY = ToRadian(60.0f);
        mCamera.SetPerspective(fovY, 1.f, 0.1f, 100.f);
        OnResize(GetWindow().ClientWidth(), GetWindow().ClientHeight());

        return true;
    }

    void OnResize(uint32_t width, uint32_t height) override
    {
        if(height == 0) return;
        mCamera.SetAspect(static_cast<float>(width)/static_cast<float>(height));
    }

    void OnUpdate(float deltaTime) override
    {
        mElapsedTime += deltaTime;
        if(mElapsedTime < mDelay) return;

        mTransform.rotation.x += mRotationSpeed*0.5f * deltaTime;
        mTransform.rotation.y += mRotationSpeed * deltaTime;
        // mTransform.rotation.z += mRotationSpeed * deltaTime;

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
        const Matrix4x4 world = mTransform.WorldMatrix();
        const Matrix4x4 view = mCamera.ViewMatrix();
        const Matrix4x4 projection = mCamera.ProjectionMatrix();
        const Matrix4x4 wvp = world * view * projection;

        mTriangle.Render(renderer.Context(), world, wvp, mCamera.Position());
    }

private:
    TriangleRenderer mTriangle;
    Transform        mTransform;
    float            mRotationSpeed = 2.0f;
    float            mDelay = 1.f;
    float            mElapsedTime = 0.f;

    Camera           mCamera;
    float            mCameraAngle = 0;
    float            mCameraRadius = 3.f;
    float            mCameraSpeed = 2.5f;
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
