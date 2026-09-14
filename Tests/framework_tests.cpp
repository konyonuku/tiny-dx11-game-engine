#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "GameFramework/World.h"

namespace
{
    using Events = std::vector<std::string>;

    void Expect(bool condition, const std::string& message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    void ExpectEvents(
        const Events& actual,
        std::initializer_list<const char*> expected,
        const std::string& context)
    {
        Expect(actual.size() == expected.size(),
               context + ": event count differs (actual " +
                   std::to_string(actual.size()) + ", expected " +
                   std::to_string(expected.size()) + ")");

        std::size_t index = 0;
        for (const char* expectedEvent : expected)
        {
            Expect(actual[index] == expectedEvent,
                   context + ": event " + std::to_string(index) +
                       " was '" + actual[index] + "', expected '" +
                       expectedEvent + "'");
            ++index;
        }
    }

    class ProbeComponent final : public ActorComponent
    {
    public:
        ProbeComponent(Events& events, std::string name,
                       int* destructionCount = nullptr)
            : mEvents(events),
              mName(std::move(name)),
              mDestructionCount(destructionCount)
        {
        }

        ~ProbeComponent() override
        {
            if (mDestructionCount)
                ++(*mDestructionCount);
        }

        std::function<void()> onBeginPlay;
        std::function<void()> onTick;
        std::function<void()> onEndPlay;

    protected:
        void BeginPlay() override
        {
            mEvents.push_back(mName + ".Begin");
            if (onBeginPlay)
                onBeginPlay();
        }

        void Tick(float) override
        {
            mEvents.push_back(mName + ".Tick");
            if (onTick)
                onTick();
        }

        void EndPlay() override
        {
            mEvents.push_back(mName + ".End");
            if (onEndPlay)
                onEndPlay();
        }

    private:
        Events& mEvents;
        std::string mName;
        int* mDestructionCount;
    };

    class ProbeActor final : public Actor
    {
    public:
        ProbeActor(Events& events, std::string name,
                   int* destructionCount = nullptr)
            : mEvents(events),
              mName(std::move(name)),
              mDestructionCount(destructionCount)
        {
        }

        ~ProbeActor() override
        {
            if (mDestructionCount)
                ++(*mDestructionCount);
        }

        std::function<void()> onBeginPlay;
        std::function<void()> onTick;
        std::function<void()> onEndPlay;

    protected:
        void BeginPlay() override
        {
            mEvents.push_back(mName + ".Begin");
            if (onBeginPlay)
                onBeginPlay();
        }

        void Tick(float) override
        {
            mEvents.push_back(mName + ".Tick");
            if (onTick)
                onTick();
        }

        void EndPlay() override
        {
            mEvents.push_back(mName + ".End");
            if (onEndPlay)
                onEndPlay();
        }

    private:
        Events& mEvents;
        std::string mName;
        int* mDestructionCount;
    };

    void TestNormalLifecycleAndTransform()
    {
        Events events;
        int destroyedActors = 0;
        World world;

        auto* actorA = world.SpawnActor<ProbeActor>(events, "A", &destroyedActors);
        actorA->GetTransform().position.x = 3.0f;

        bool transformObserved = false;
        actorA->onBeginPlay = [actorA, &transformObserved]() {
            transformObserved = actorA->GetTransform().position.x == 3.0f;
        };

        auto* component1 = actorA->AddComponent<ProbeComponent>(events, "C1");
        actorA->AddComponent<ProbeComponent>(events, "C2");
        world.SpawnActor<ProbeActor>(events, "B", &destroyedActors);

        Expect(component1->GetOwner() == actorA, "component owner was not assigned");
        Expect(world.GetActorCount() == 2, "pending actors were not counted");

        world.Tick(0.25f);
        Expect(transformObserved, "spawn-time transform was not visible in BeginPlay");
        ExpectEvents(events,
                     {"A.Begin", "C1.Begin", "C2.Begin", "B.Begin",
                      "A.Tick", "C1.Tick", "C2.Tick", "B.Tick"},
                     "first tick order");

        Expect(actorA->AddComponent<ProbeComponent>(events, "Late") == nullptr,
               "component was added after BeginPlay");

        events.clear();
        world.Tick(0.5f);
        ExpectEvents(events,
                     {"A.Tick", "C1.Tick", "C2.Tick", "B.Tick"},
                     "second tick order");

        events.clear();
        world.Clear();
        ExpectEvents(events,
                     {"C2.End", "C1.End", "A.End", "B.End"},
                     "clear order");
        Expect(destroyedActors == 2, "actors were not destroyed by Clear");
    }

    void TestSelfDestroyStopsComponentTick()
    {
        Events events;
        int destroyedActors = 0;
        World world;

        auto* actor = world.SpawnActor<ProbeActor>(events, "A", &destroyedActors);
        actor->AddComponent<ProbeComponent>(events, "C1");
        actor->AddComponent<ProbeComponent>(events, "C2");
        actor->onTick = [&world, actor]() { world.DestroyActor(actor); };

        world.Tick(0.1f);

        ExpectEvents(events,
                     {"A.Begin", "C1.Begin", "C2.Begin", "A.Tick",
                      "C2.End", "C1.End", "A.End"},
                     "self destroy order");
        Expect(world.GetActorCount() == 0, "self-destroyed actor remained in World");
        Expect(destroyedActors == 1, "self-destroyed actor destructor count differs");
    }

    void TestDestroyOtherBeforeItsTick()
    {
        Events events;
        World world;

        auto* actorA = world.SpawnActor<ProbeActor>(events, "A");
        auto* actorB = world.SpawnActor<ProbeActor>(events, "B");
        actorA->onTick = [&world, actorB]() { world.DestroyActor(actorB); };

        world.Tick(0.1f);

        ExpectEvents(events,
                     {"A.Begin", "B.Begin", "A.Tick", "B.End"},
                     "destroy later actor");
        Expect(world.GetActorCount() == 1, "destroyed later actor remained in World");
    }

    void TestDestroyDuringBeginPlay()
    {
        Events events;
        World world;

        auto* actor = world.SpawnActor<ProbeActor>(events, "A");
        actor->AddComponent<ProbeComponent>(events, "C");
        actor->onBeginPlay = [&world, actor]() { world.DestroyActor(actor); };

        world.Tick(0.1f);

        ExpectEvents(events, {"A.Begin", "A.End"}, "destroy during BeginPlay");
        Expect(world.GetActorCount() == 0, "BeginPlay-destroyed actor remained in World");
    }

    void TestDestroyBeforeFirstTick()
    {
        Events events;
        int destroyedActors = 0;
        int destroyedComponents = 0;
        World world;

        auto* actor = world.SpawnActor<ProbeActor>(events, "A", &destroyedActors);
        actor->AddComponent<ProbeComponent>(events, "C", &destroyedComponents);
        world.DestroyActor(actor);
        world.DestroyActor(actor);

        Expect(world.GetActorCount() == 0, "pending-destroy actor was counted");
        world.Tick(0.1f);

        Expect(events.empty(), "unstarted actor received a lifecycle event");
        Expect(destroyedActors == 1, "unstarted actor was not destroyed exactly once");
        Expect(destroyedComponents == 1,
               "unstarted component was not destroyed exactly once");
    }

    void TestSpawnDuringBeginPlayIsDeferred()
    {
        Events events;
        World world;

        ProbeActor* actorB = nullptr;
        auto* actorA = world.SpawnActor<ProbeActor>(events, "A");
        actorA->onBeginPlay = [&]() {
            actorB = world.SpawnActor<ProbeActor>(events, "B");
        };

        world.Tick(0.1f);
        Expect(actorB != nullptr, "actor was not spawned during BeginPlay");
        ExpectEvents(events, {"A.Begin", "A.Tick"}, "deferred spawn first tick");

        events.clear();
        world.Tick(0.1f);
        ExpectEvents(events,
                     {"B.Begin", "A.Tick", "B.Tick"},
                     "deferred spawn second tick");
    }

    void TestPendingSpawnDestroyedDuringTick()
    {
        Events events;
        int destroyedB = 0;
        World world;

        auto* actorA = world.SpawnActor<ProbeActor>(events, "A");
        actorA->onTick = [&]() {
            auto* actorB = world.SpawnActor<ProbeActor>(events, "B", &destroyedB);
            world.DestroyActor(actorB);
        };

        world.Tick(0.1f);

        ExpectEvents(events, {"A.Begin", "A.Tick"}, "destroy pending spawn");
        Expect(destroyedB == 1, "pending spawn was not destroyed in the same frame");
        Expect(world.GetActorCount() == 1, "destroyed pending spawn was retained");
    }

    void TestEndPlayCascadesInSameFlush()
    {
        Events events;
        int destroyedActors = 0;
        World world;

        auto* actorA = world.SpawnActor<ProbeActor>(events, "A", &destroyedActors);
        auto* actorB = world.SpawnActor<ProbeActor>(events, "B", &destroyedActors);
        actorA->onEndPlay = [&world, actorB]() { world.DestroyActor(actorB); };

        world.Tick(0.1f);
        events.clear();
        world.DestroyActor(actorA);
        world.DestroyActor(actorA);
        world.Tick(0.1f);

        ExpectEvents(events, {"B.Tick", "A.End", "B.End"}, "cascading EndPlay");
        Expect(world.GetActorCount() == 0, "cascaded actor remained in World");
        Expect(destroyedActors == 2, "cascaded actors were not destroyed exactly once");
    }

    void TestClearReuseForeignDestroyAndWorldDestruction()
    {
        Events events;

        {
            World world;
            World otherWorld;

            auto* actorA = world.SpawnActor<ProbeActor>(events, "A");
            actorA->AddComponent<ProbeComponent>(events, "C");
            world.Tick(0.1f);

            otherWorld.DestroyActor(actorA);
            otherWorld.DestroyActor(nullptr);
            Expect(world.GetActorCount() == 1,
                   "foreign World destroyed an actor it does not own");

            events.clear();
            world.Clear();
            world.Clear();
            ExpectEvents(events, {"C.End", "A.End"}, "repeated Clear");
            Expect(world.GetActorCount() == 0, "Clear did not empty World");

            world.SpawnActor<ProbeActor>(events, "B");
            world.Tick(0.1f);
            Expect(world.GetActorCount() == 1, "World was not reusable after Clear");
        }

        ExpectEvents(events,
                     {"C.End", "A.End", "B.Begin", "B.Tick", "B.End"},
                     "World destruction");
    }
}

int main()
{
    using Test = std::pair<const char*, void (*)()>;
    const std::vector<Test> tests = {
        {"normal lifecycle and transform", TestNormalLifecycleAndTransform},
        {"self destroy", TestSelfDestroyStopsComponentTick},
        {"destroy later actor", TestDestroyOtherBeforeItsTick},
        {"destroy during BeginPlay", TestDestroyDuringBeginPlay},
        {"destroy before first tick", TestDestroyBeforeFirstTick},
        {"deferred spawn", TestSpawnDuringBeginPlayIsDeferred},
        {"destroy pending spawn", TestPendingSpawnDestroyedDuringTick},
        {"cascading EndPlay", TestEndPlayCascadesInSameFlush},
        {"Clear, foreign destroy, and World destruction",
         TestClearReuseForeignDestroyAndWorldDestruction},
    };

    int failures = 0;
    for (const auto& [name, test] : tests)
    {
        try
        {
            test();
            std::cout << "[PASS] " << name << '\n';
        }
        catch (const std::exception& error)
        {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }

    if (failures == 0)
        std::cout << "All framework lifecycle tests passed.\n";

    return failures == 0 ? 0 : 1;
}
