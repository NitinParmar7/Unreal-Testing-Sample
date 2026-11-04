#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

/** RAII private UWorld for tests. The test controls time by calling Tick(). */
class FTestWorld
{
public:
    FTestWorld()
    {
        World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);

        FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
        Context.SetCurrentWorld(World);

        FURL URL;
        World->InitializeActorsForPlay(URL);
        World->BeginPlay();
    }

    ~FTestWorld()
    {
        if (World)
        {
            GEngine->DestroyWorldContext(World);
            World->DestroyWorld(/*bInformEngineOfWorld*/ false);
            World = nullptr;
        }
    }

    FTestWorld(const FTestWorld&) = delete;
    FTestWorld& operator=(const FTestWorld&) = delete;

    UWorld* Get() const { return World; }

    /** Advance the world (and its timer manager) by DeltaTime seconds of simulated time. */
    void Tick(float DeltaTime) const
    {
        World->Tick(LEVELTICK_All, DeltaTime);
    }

private:
    UWorld* World = nullptr;
};