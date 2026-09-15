#include "World.h"

#include "Graphics/Renderer.h"
#include "Graphics/RenderTypes.h"


World::~World()
{
    Clear();
}

void World::DestroyActor(Actor * actor)
{
    if(actor == nullptr) return;

    for(auto& a : mActorsToSpawn) {
        if(a.get() == actor) {
            actor->mPendingDestroy = true;
            return;
        }
    }

    for(auto& a : mActors) {
        if(a.get() == actor) {
            actor->mPendingDestroy = true;
            return;
        }    
    }
}

void World::Tick(float deltaTime)
{
    if(mIsTicking || mIsClearing) return;
    mIsTicking = true;

    mActors.reserve(mActorsToSpawn.size() + mActors.size());
    for(auto& actor : mActorsToSpawn) {
        mActors.push_back(std::move(actor));        
    }
    mActorsToSpawn.clear();

    for(auto& actor : mActors) {
        if(actor->mPendingDestroy) continue;
        actor->DispatchBeginPlay();
    }

    for(auto& actor : mActors) {
        if(actor->mPendingDestroy) continue;
        actor->DispatchTick(deltaTime);
    }

    FlushDestroyedActors();

    mIsTicking = false;
}

void World::Clear()
{
    if(mIsClearing || mIsTicking) return;
    mIsClearing = true;

    for(auto& a : mActorsToSpawn) {
        if(!a->mPendingDestroy) a->mPendingDestroy = true;
    }
    for(auto& a : mActors) {
        if(!a->mPendingDestroy) a->mPendingDestroy = true;
    }
    
    FlushDestroyedActors();

    mIsClearing = false;
}

std::size_t World::GetActorCount() const
{
    std::size_t count = 0;

    for(const auto& a : mActorsToSpawn) {
        if(!a->mPendingDestroy) ++count;
    }

    for(const auto& a : mActors) {
        if(!a->mPendingDestroy) ++count;
    }  

    return count;
}

bool World::Render(Renderer& renderer) const
{
    if(mIsTicking || mIsClearing) return false;

    std::vector<RenderItem> renderItems;
    for(const auto& actor : mActors) {
        actor->CollectRenderItems(renderItems);
    }

    for(const auto& item : renderItems) {
        if(!item.mesh || !item.material) return false;
        if(!renderer.DrawMesh(*item.mesh, *item.material, item.world)) return false;
    }

    return true;
}

void World::FlushDestroyedActors()
{
    while(true) {
        std::vector<std::unique_ptr<Actor>> actorsToDestroy;

        auto it = mActorsToSpawn.begin();
        while(it != mActorsToSpawn.end()) {
            if((*it)->mPendingDestroy) {
                actorsToDestroy.push_back(std::move(*it));
                it = mActorsToSpawn.erase(it);
            } else {
                ++it;
            }
        }
        it = mActors.begin();
        while(it != mActors.end()) {
            if((*it)->mPendingDestroy) {
                actorsToDestroy.push_back(std::move(*it));
                it = mActors.erase(it);
            } else {
                ++it;                
            }
        } //

        if(actorsToDestroy.empty()) break;
        for(auto& actor : actorsToDestroy) {
            actor->DispatchEndPlay();
        }
    } //end while
}
