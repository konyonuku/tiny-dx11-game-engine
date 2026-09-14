#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "GameFramework/Actor.h"


class World
{
public:
    ~World();

    template<class T, class... Args>
    T* SpawnActor(Args&&... args);

    void DestroyActor(Actor* actor);
    void Tick(float deltaTime);
    void Clear();
    std::size_t GetActorCount() const;

private:
    void FlushDestroyedActors();
    std::vector<std::unique_ptr<Actor>> mActors;
    std::vector<std::unique_ptr<Actor>> mActorsToSpawn;
    bool mIsTicking = false;
    bool mIsClearing = false;
};


template <class T, class... Args>
inline T *World::SpawnActor(Args &&...args)
{
    static_assert(std::is_base_of_v<Actor, T>, "T must be Actor or derived");

    if(mIsClearing) return nullptr;

    auto actor = std::make_unique<T>(std::forward<Args>(args)...);
    T* result = actor.get();
    actor->mWorld = this;
    mActorsToSpawn.push_back(std::move(actor));

    return result;
}
