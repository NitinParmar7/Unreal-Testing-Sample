#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "TestWorld.h"

/** Simulates Duration seconds, one Tick per engine frame, then calls OnComplete. */
inline void AdvanceWorldFor(FTestWorld* TestWorld, float Duration, float FrameDelta, TFunction<void()> OnComplete)
{
    const int32 FrameCount = FMath::CeilToInt(Duration / FrameDelta);
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
        [TestWorld, FramesRemaining = FrameCount, FrameDelta, bPrimed = false, OnComplete = MoveTemp(OnComplete)](float) mutable
        {
            if (!bPrimed)
            {
                TestWorld->Tick(0.f);       // priming tick, no simulated time passes
                bPrimed = true;
                return true;
            }

            TestWorld->Tick(FrameDelta);
            if (--FramesRemaining == 0)
            {
                OnComplete();
                return false;
            }

            return true;
        }));
}
