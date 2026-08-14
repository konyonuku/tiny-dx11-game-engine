#include "Core/GameTimer.h"

GameTimer::GameTimer()
    : mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPrevTime(0), mCurrTime(0)
{
    __int64 freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

    mSecondsPerCount = 1.0 / (double)freq;
}

void GameTimer::Reset()
{
    __int64 cur;
    QueryPerformanceCounter((LARGE_INTEGER*)&cur);

    mBaseTime = cur;
    mPrevTime = cur;
    mCurrTime = cur;
}

void GameTimer::Tick()
{
    __int64 cur;
    QueryPerformanceCounter((LARGE_INTEGER*)&cur);

    mCurrTime = cur;
    mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
    mPrevTime = mCurrTime;

    if (mDeltaTime < 0.0) mDeltaTime = 0.0;
}

float GameTimer::DeltaTime() const
{
    return (float)mDeltaTime;
}

float GameTimer::TotalTime() const
{
    return (float)((mCurrTime - mBaseTime) * mSecondsPerCount);
}
