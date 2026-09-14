#include "Actor.h"

World *Actor::GetWorld() const
{
    return mWorld;
}

Transform &Actor::GetTransform()
{
    return mTransform;
}

const Transform &Actor::GetTransform() const
{
    return mTransform;
}

bool Actor::IsPendingDestroy() const
{
    return mPendingDestroy;
}

void Actor::BeginPlay()
{
}

void Actor::Tick(float deltaTime)
{
}

void Actor::EndPlay()
{
}

void Actor::DispatchBeginPlay()
{
    if(mHasBegunPlay || mHasEndedPlay || mPendingDestroy) return; 
    mHasBegunPlay = true;
    BeginPlay();
    
    if(mPendingDestroy) return;

    for(auto& component : mComponents) {
        component->DispatchBeginPlay();

        if(mPendingDestroy) return;
    }
}

void Actor::DispatchTick(float deltaTime)
{
    if(!mHasBegunPlay || mHasEndedPlay || mPendingDestroy) return;
    Tick(deltaTime);

    if(mPendingDestroy) return;

    for(auto& component : mComponents) {
        component->DispatchTick(deltaTime);
        
        if(mPendingDestroy) return;
    }
}

void Actor::DispatchEndPlay()
{
    if(!mHasBegunPlay || mHasEndedPlay) return;
    mHasEndedPlay = true;
    
    for(auto it = mComponents.rbegin(); it != mComponents.rend(); ++it) {
        (*it)->DispatchEndPlay();
    }
    
    EndPlay();
}
