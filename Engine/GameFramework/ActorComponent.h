#pragma once
#include <vector>

#include "Graphics/RenderTypes.h"

class Actor;


class ActorComponent
{
public:
    virtual ~ActorComponent() = default;
    Actor* GetOwner() const;

protected:
    virtual void BeginPlay();
    virtual void Tick(float deltaTime);
    virtual void EndPlay();
    virtual void CollectRenderItems(std::vector<RenderItem>& items) const; //StaticMeshComponent

private:
    friend class Actor;
    void DispatchBeginPlay();
    void DispatchTick(float deltaTime);
    void DispatchEndPlay();
    void DispatchCollectRenderItems(std::vector<RenderItem>& items) const;

    Actor* mOwner = nullptr;
    //UE enum EActorBeginPlayState { HasNotBegunPlay, BeginningPlay, HasBegunPlay } 
    bool mHasBegunPlay = false;
    bool mHasEndedPlay = false;
};