#include <cmath>

#include "Core/Application.h"
#include "Core/GameTimer.h"
#include "Math/Transform.h"
#include "TriangleRenderer.h"
#include "Graphics/Camera.h"


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
        
        constexpr float      pi = 3.1415926535f;
        mCameraAngle = -pi / 2;
        const float aspect = static_cast<float>(GetWindow().ClientWidth())/static_cast<float>(GetWindow().ClientHeight());
        mCamera.SetPerspective(60.f*pi/180.f, aspect, 0.1f, 100.f);
        
        return true;
    }

    void OnUpdate(float deltaTime) override
    {
        mElapsedTime += deltaTime;
        if(mElapsedTime < mDelay) return;

        mTransform.rotation.x += mRotationSpeed*0.5f * deltaTime;
        mTransform.rotation.y += mRotationSpeed * deltaTime;
        // mTransform.rotation.z += mRotationSpeed * deltaTime;

        mCameraAngle += mCameraSpeed * deltaTime;
        Vector3 eye {std::cos(mCameraAngle) * mCameraRadius, 0.f, std::sin(mCameraAngle) * mCameraRadius};
        mCamera.SetLookAt(
            eye,
            {0.f, 0.f, 0.f},
            {0.f, 1.f, 0.f}
        );
    }

    void OnRender(Renderer& renderer) override
    {
        const Matrix4x4 world = mTransform.WorldMatrix();
        const Matrix4x4 view = mCamera.ViewMatrix();
        const Matrix4x4 projection = mCamera.ProjectionMatrix();
        const Matrix4x4 wvp = world * view * projection;

        mTriangle.Render(renderer.Context(), wvp);
    }

private:
    TriangleRenderer mTriangle;
    Transform        mTransform;
    float            mRotationSpeed = 0.4f;
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
