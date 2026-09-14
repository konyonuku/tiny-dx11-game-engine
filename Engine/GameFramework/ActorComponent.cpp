#include "ActorComponent.h"

Actor *ActorComponent::GetOwner() const
{
    return mOwner;
}

void ActorComponent::BeginPlay()
{
}

void ActorComponent::Tick(float deltaTime)
{
}

void ActorComponent::EndPlay()
{
}

void ActorComponent::DispatchBeginPlay()
{
    if(mHasBegunPlay || mHasEndedPlay) return; 
    mHasBegunPlay = true;
    BeginPlay();
}

void ActorComponent::DispatchTick(float deltaTime)
{
    if(!mHasBegunPlay || mHasEndedPlay) return;
    Tick(deltaTime);
}

void ActorComponent::DispatchEndPlay()
{
    if(!mHasBegunPlay || mHasEndedPlay) return;
    mHasEndedPlay = true;
    EndPlay();
}
