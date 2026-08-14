#pragma once
#include <windows.h>

class GameTimer
{
public:
    GameTimer();

    void Reset();
    void Tick();

    float DeltaTime() const;
    float TotalTime() const;

private:
    double  mSecondsPerCount;
    double  mDeltaTime;
    __int64 mBaseTime;
    __int64 mPrevTime;
    __int64 mCurrTime;
};
