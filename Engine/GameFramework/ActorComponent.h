#pragma once

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

private:
    friend class Actor;
    void DispatchBeginPlay();
    void DispatchTick(float deltaTime);
    void DispatchEndPlay();

    Actor* mOwner = nullptr;
    //UE enum EActorBeginPlayState { HasNotBegunPlay, BeginningPlay, HasBegunPlay } 
    bool mHasBegunPlay = false;
    bool mHasEndedPlay = false;
};