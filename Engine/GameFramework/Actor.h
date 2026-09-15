#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Math/Transform.h"
#include "ActorComponent.h"


class World;

class Actor
{
public:
    virtual ~Actor() = default;
    World* GetWorld() const;
    Transform& GetTransform();
    const Transform& GetTransform() const;
    bool IsPendingDestroy() const;

    template<class T, class... Args>
    T* AddComponent(Args&&... args);

protected:
    virtual void BeginPlay();
    virtual void Tick(float deltaTime);
    virtual void EndPlay();

private:
    friend class World;
    void DispatchBeginPlay();
    void DispatchTick(float deltaTime);
    void DispatchEndPlay();
    void CollectRenderItems(std::vector<RenderItem>& items) const;
    

    World* mWorld = nullptr;
    Transform mTransform;
    std::vector<std::unique_ptr<ActorComponent>> mComponents;
    bool mHasBegunPlay = false;
    bool mHasEndedPlay = false;
    bool mPendingDestroy = false;
};


template <class T, class... Args>
inline T *Actor::AddComponent(Args &&...args)
{
    static_assert(std::is_base_of_v<ActorComponent, T>, "T must be ActorComponenr or derived");

    if(mHasBegunPlay || mPendingDestroy) return nullptr;

    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* result = component.get();
    component->mOwner = this;
    mComponents.push_back(std::move(component));

    return result;
}
